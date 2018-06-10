/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   main.cpp                                      <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:50:59 by prp              2E 54 65 63 68          */
/*   Updated: 2018/06/09 22:22:37 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstring>
#include <iomanip>

#include <fftw3.h>

#include "Configuration.hpp"
#include "FFTransformer.hpp"
#include "PulseAudioSource.hpp"
#include "SoftPWMControl.hpp"

bool exit_signal = false;

void interrupt_signal(int signal_code) {
	if (signal_code == SIGINT)
		exit_signal = true;
}

int main(int argc, char const* argv[]) {
	// Parse Configs.
	Lightshow::Configuration config;
	if (argc > 1)
		config = Lightshow::Configuration(argv[1]);
	// Setup Signal Handlers.
	std::signal(SIGINT, interrupt_signal);
	// Calculate Buffer size.
	size_t buffer_size = config.get_sample_freq() / config.get_framerate();
	// Default Sound Spec Instance.
	auto spec = Lightshow::PulseAudioSource::default_spec;
	// Buffer Initialization
	Lightshow::PCMStereoSample input_buffer[buffer_size];
	size_t sizeof_ibuff = buffer_size * sizeof(Lightshow::PCMStereoSample);
	// Create FFTransformer
	Lightshow::FFTransformer transformer(1, buffer_size);
	// Setup Buffer Filler;
	auto fill_func = [](void* input_buffer, size_t index) -> double {
		auto buffer = (Lightshow::PCMStereoSample*)input_buffer;
		return buffer[index].left + buffer[index].right;
	};
	// Create Source...
	Lightshow::PulseAudioSource source(
		argv[0],
		"recording",
		&spec,
		Lightshow::PulseAudioSource::get_default_source_name());
	// Setup GPIO
	GPIOInterface blueLights("17", GPIO_DIR_OUT);
	GPIOInterface redLights("22", GPIO_DIR_OUT);
	GPIOInterface greenLights("27", GPIO_DIR_OUT);
	// Setup PWM interface.
	SoftPWMControl bluePWM(&blueLights);
	SoftPWMControl redPWM(&redLights);
	SoftPWMControl greenPWM(&greenLights);
	// Initialize PWM.
	bluePWM.setDutyCycle(0);
	redPWM.setDutyCycle(0);
	greenPWM.setDutyCycle(0);
	bluePWM.init();
	redPWM.init();
	greenPWM.init();
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

			size_t iter = config.get_floor() * out_count / 100;

			double lsum = 0.0;
			size_t lco  = config.get_low_cutoff() * out_count / 100;
			while (iter < lco) {
				double real		 = output[iter][0];
				double imaginary = output[iter][1];
				lsum += std::sqrt(real * real + imaginary * imaginary);
				++iter;
			}
			lsum *= config.get_low_mult();

			double msum = 0.0;
			size_t mco  = config.get_low_cutoff() * out_count / 100;
			while (iter < mco) {
				double real		 = output[iter][0];
				double imaginary = output[iter][1];
				msum += std::sqrt(real * real + imaginary * imaginary);
				++iter;
			}
			msum *= config.get_mid_mult();

			double hsum = 0.0;
			size_t hco  = config.get_low_cutoff() * out_count / 100;
			while (iter < hco) {
				double real		 = output[iter][0];
				double imaginary = output[iter][1];
				hsum += std::sqrt(real * real + imaginary * imaginary);
				++iter;
			}
			hsum *= config.get_high_mult();
			std::cout << std::setfill(' ') << std::setprecision(3) << std::fixed
					  << "Output: l = " << std::setw(8)
					  << (lsum > 1.0 ? 1.0 : lsum) << ", m = " << std::setw(8)
					  << (msum > 1.0 ? 1.0 : msum) << ", h = " << std::setw(8)
					  << (hsum > 1.0 ? 1.0 : hsum) << '\r' << std::flush;
			redPWM.setDutyCycle((lsum > 1.0 ? 1.0 : lsum));
			greenPWM.setDutyCycle((msum > 1.0 ? 1.0 : msum));
			bluePWM.setDutyCycle((hsum > 1.0 ? 1.0 : hsum));
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
