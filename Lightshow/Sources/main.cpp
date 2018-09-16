/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   main.cpp                                      <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:50:59 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/16 10:54:54 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstring>
#include <iomanip>
#include <iostream>

#include <fftw3.h>

#include "Config.hpp"
#include "DebugTools.hpp"
#include "FFTransformer.hpp"
#include "Lightshow.hpp"
#include "PulseAudioSource.hpp"
#include "Socket.hpp"
#include "SoftPWMControl.hpp"

using namespace Lightshow;
using namespace TF::Network;
using namespace TF::Debug;

bool exit_signal = false;

void interrupt_signal(int signal_code) {
	if (signal_code == SIGINT)
		exit_signal = true;
}

int main(int argc, char const* argv[]) {
	// Parse Configs.
	Config config;
	Socket socket_thing(2533);
	socket_thing.set_on_recieve(
	    [](SocketAddress addr, size_t len, uint8_t* msg) {
		    (void)addr;
		    std::cout << "Recieved message! Length: " << len << "\n";
		    printf("%s\n", (char*)msg);
	    });
	// Begin listening for udp messages with passed buffer size.
	socket_thing.listen(1024);
	if (argc > 1)
		config = Config(argv[1]);
	// Setup Signal Handlers.
	std::signal(SIGINT, interrupt_signal);
	// Calculate Buffer size.
	size_t buffer_size = config.sample_freq / config.framerate;
	// Default Sound Spec Instance.
	auto spec = PulseAudioSource::default_spec;
	// Buffer Initialization
	PCMStereoSample input_buffer[buffer_size];
	size_t          sizeof_ibuff = buffer_size * sizeof(PCMStereoSample);
	// Create FFTransformer
	FFTransformer transformer(1, buffer_size);
	// Setup Buffer Filler;
	auto fill_func = [](void* input_buffer, size_t index) -> double {
		auto buffer = (PCMStereoSample*)input_buffer;
		return buffer[index].left + buffer[index].right;
	};
	// Create Source...
	PulseAudioSource source(argv[0],
	                        "recording",
	                        &spec,
	                        PulseAudioSource::get_default_source_name());
	// Setup GPIO
	GPIOInterface blueLights("17", GPIO_DIR_OUT);
	GPIOInterface redLights("22", GPIO_DIR_OUT);
	GPIOInterface greenLights("27", GPIO_DIR_OUT);
	// Setup PWM interface.
	SoftPWMControl bluePWM(blueLights);
	SoftPWMControl redPWM(redLights);
	SoftPWMControl greenPWM(greenLights);
	// Initialize PWM.
	bluePWM.start();
	redPWM.start();
	greenPWM.start();
	// Read loop...
	while (!exit_signal) {
		std::memset(input_buffer, 0, sizeof_ibuff);
		if (source.read_into(input_buffer, sizeof_ibuff)) {
			// Fill FFT Buffer
			transformer.fill_input_buffer(0, input_buffer, fill_func);
			// Run the FFT
			transformer.calculate_dft();
			// Get Channel outputs
			fftw_complex* output = transformer.get_output(0);

			auto out_count = transformer.get_output_count();

			size_t floor = config.floor * out_count / 100;

			size_t lco  = config.low_cutoff * out_count / 100;
			double lsum = sum_samples(output, floor, lco);
			lsum *= config.low_mult;

			size_t mco  = config.mid_cutoff * out_count / 100;
			double msum = sum_samples(output, lco, mco);
			msum *= config.mid_mult;

			size_t hco  = config.high_cutoff * out_count / 100;
			double hsum = sum_samples(output, mco, hco);
			hsum *= config.high_mult;

			auto packet_to_send = create_packet(lsum, msum, hsum);
			auto msg            = build_remote_msg(packet_to_send);
			auto extracted =
			    extract_remote_msg((uint8_t*)msg.data(), msg.length());
			if (memcmp(&packet_to_send, &extracted, sizeof(NetworkPacket))) {
				print_debug("DBG -> [main]: Extracted packet does not match "
				            "serealized packet.");
			}

			std::cout << std::setfill(' ') << std::setprecision(3) << std::fixed
			          << "Output: l = " << std::setw(8)
			          << (lsum > 1.0 ? 1.0 : lsum) << ", m = " << std::setw(8)
			          << (msum > 1.0 ? 1.0 : msum) << ", h = " << std::setw(8)
			          << (hsum > 1.0 ? 1.0 : hsum) << '\r' << std::flush;

			redPWM.set_duty_cycle((lsum > 1.0 ? 1.0 : lsum));
			greenPWM.set_duty_cycle((msum > 1.0 ? 1.0 : msum));
			bluePWM.set_duty_cycle((hsum > 1.0 ? 1.0 : hsum));
		} else {
			std::cout << "\nAborting Read...\n" << std::flush;
		}
	}
	redPWM.stop();
	greenPWM.stop();
	bluePWM.stop();
	std::cout << "\nExiting..." << std::endl;
	return 0;
}
