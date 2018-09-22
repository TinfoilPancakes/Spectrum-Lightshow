/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Lightshow.cpp                                 <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/11 13:29:03 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/22 09:37:11 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Lightshow.hpp"
#include "Crypto.hpp"
#include "DebugTools.hpp"
#include "FFTransformer.hpp"
#include "GPIOInterface.hpp"
#include "PulseAudioSource.hpp"
#include "Socket.hpp"
#include "SoftPWMControl.hpp"

#include <atomic>
#include <csignal>
#include <cstring>
#include <functional>
#include <iomanip>
#include <mutex>
#include <random>

using namespace Lightshow;
using namespace TF::Debug;

NetworkPacket Lightshow::create_packet(float r, float g, float b) {
	std::random_device                      rd;
	std::mt19937_64                         mt_generator(rd());
	std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX - 1);

	NetworkPacket packet;
	packet.r    = r;
	packet.g    = g;
	packet.b    = b;
	packet.seed = dist(mt_generator);

	return packet;
}

std::basic_string<uint8_t> Lightshow::build_remote_msg(NetworkPacket packet) {
	std::basic_string<uint8_t> raw_msg;
	raw_msg.append((uint8_t*)"WOOF", 4);
	raw_msg.append((uint8_t*)&packet.r, sizeof(packet.r));
	raw_msg.append((uint8_t*)&packet.g, sizeof(packet.g));
	raw_msg.append((uint8_t*)&packet.b, sizeof(packet.b));
	raw_msg.append((uint8_t*)&packet.seed, sizeof(packet.seed));
	uint8_t crc = 0;
	for (auto byte : raw_msg) {
		crc ^= byte;
	}
	raw_msg.push_back(crc);
	return raw_msg;
}

NetworkPacket Lightshow::extract_remote_msg(uint8_t* msg, size_t len) {
	NetworkPacket extracted;
	extracted.seed = UINT64_MAX;
	if (std::memcmp((char*)msg, "WOOF", 4)) {
		print_warning_line("WRN -> [extract_remote_msg]: Invalid packet.");
		return extracted;
	}

	uint8_t crc = 0;
	for (size_t i = 0; i < len - 1; ++i) {
		crc ^= msg[i];
	}

	if (crc != msg[len - 1]) {
		print_warning_line("WRN -> [extract_remote_msg]: Invalid Packet CRC");
		return extracted;
	}

	float* msg_content = (float*)&msg[4];

	extracted.r = msg_content[0];
	extracted.g = msg_content[1];
	extracted.b = msg_content[2];

	uint64_t* msg_seed = (uint64_t*)&msg_content[3];

	extracted.seed = *msg_seed;

	return extracted;
}

double Lightshow::sum_samples(fftw_complex* samples,
                              size_t        start_idx,
                              size_t        end_idx) {
	double sum = 0;
	while (start_idx < end_idx) {
		double real      = samples[start_idx][0];
		double imaginary = samples[start_idx][0];
		sum += std::sqrt(real * real + imaginary * imaginary);
		++start_idx;
	}
	return sum;
}

namespace {
std::function<void(int)> local_shutdown;
}

void Lightshow::run_local(Lightshow::Config config) {
	// Initialize termination condition.
	std::atomic_bool exit_signal;
	exit_signal = false;
	// Setup Signal Handler
	local_shutdown = [&exit_signal](int signal) {
		if (signal == SIGINT)
			exit_signal = true;
	};
	std::signal(SIGINT, [](int signal) { local_shutdown(signal); });
	// Setup Sample Buffer
	size_t buffer_size = config.sample_freq / config.framerate;
	auto   spec        = PulseAudioSource::default_spec;

	PCMStereoSample  input_buffer[buffer_size];
	size_t           sizeof_ibuff = buffer_size * sizeof(PCMStereoSample);
	PulseAudioSource source("lightshow",
	                        "recording",
	                        &spec,
	                        PulseAudioSource::get_default_source_name());
	// Setup FFT Processing
	FFTransformer transformer(1, buffer_size);
	// Setup Buffer Filler;
	auto fill_func = [](void* input_buffer, size_t index) -> double {
		auto buffer = (PCMStereoSample*)input_buffer;
		return buffer[index].left + buffer[index].right;
	};
	// Setup GPIO
	GPIOInterface blue_lights("17", GPIO_DIR_OUT);
	GPIOInterface red_lights("22", GPIO_DIR_OUT);
	GPIOInterface green_lights("27", GPIO_DIR_OUT);
	// Setup PWM interface.
	SoftPWMControl blue_pwm(blue_lights);
	SoftPWMControl red_pwm(red_lights);
	SoftPWMControl green_pwm(green_lights);

	// Initialize PWM.
	blue_pwm.start();
	red_pwm.start();
	green_pwm.start();

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

			float r = (lsum > 1.0 ? 1.0 : lsum);
			float g = (msum > 1.0 ? 1.0 : msum);
			float b = (hsum > 1.0 ? 1.0 : hsum);

			std::cout << std::setfill(' ') << std::setprecision(3) << std::fixed
			          << "Output: r (low) = " << std::setw(8) << r
			          << ", g (med) = " << std::setw(8) << g
			          << ", b (high) = " << std::setw(8) << b << '\r'
			          << std::flush;

			red_pwm.set_duty_cycle(r);
			green_pwm.set_duty_cycle(g);
			blue_pwm.set_duty_cycle(b);
		} else {
			std::cout << "\nAborting Read...\n" << std::flush;
		}
	}
	red_pwm.stop();
	green_pwm.stop();
	blue_pwm.stop();
	std::cout << "\nExiting..." << std::endl;
} // run_local

void Lightshow::run_tx(Lightshow::Config config) {
	using TF::Network::Socket;
	using TF::Network::SocketAddress;

	// Initialize termination condition.
	std::atomic_bool exit_signal;
	exit_signal = false;

	// Setup Signal Handler
	local_shutdown = [&exit_signal](int signal) {
		if (signal == SIGINT)
			exit_signal = true;
	};
	std::signal(SIGINT, [](int signal) { local_shutdown(signal); });

	// Setup Sample Buffer
	size_t buffer_size = config.sample_freq / config.framerate;
	auto   spec        = PulseAudioSource::default_spec;

	PCMStereoSample  input_buffer[buffer_size];
	size_t           sizeof_ibuff = buffer_size * sizeof(PCMStereoSample);
	PulseAudioSource source("lightshow",
	                        "recording",
	                        &spec,
	                        PulseAudioSource::get_default_source_name());

	// Setup FFT Processing
	FFTransformer transformer(1, buffer_size);

	// Setup Buffer Filler;
	auto fill_func = [](void* input_buffer, size_t index) -> double {
		auto buffer = (PCMStereoSample*)input_buffer;
		return buffer[index].left + buffer[index].right;
	};

	// Setup UDP Socket.
	Socket        udp_out(config.server_port);
	SocketAddress server_addr;
	server_addr.set_port(config.server_port);
	if (!isdigit(config.server_addr[0])) {
		std::cout << "Please enter target server ipv4 address: ";
		std::cin >> config.server_addr;
	}
	server_addr.set_address(config.server_addr);

	// Setup keystate
	auto current_key = config.initial_key;

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

			auto   out_count = transformer.get_output_count();
			size_t floor     = config.floor * out_count / 100;

			size_t lco  = config.low_cutoff * out_count / 100;
			double lsum = sum_samples(output, floor, lco);
			lsum *= config.low_mult;

			size_t mco  = config.mid_cutoff * out_count / 100;
			double msum = sum_samples(output, lco, mco);
			msum *= config.mid_mult;

			size_t hco  = config.high_cutoff * out_count / 100;
			double hsum = sum_samples(output, mco, hco);
			hsum *= config.high_mult;

			float r = (lsum > 1.0 ? 1.0 : lsum);
			float g = (msum > 1.0 ? 1.0 : msum);
			float b = (hsum > 1.0 ? 1.0 : hsum);

			auto packet   = create_packet(r, g, b);
			auto b_stream = build_remote_msg(packet);

			auto encrypted = TF::Crypto::encrypt(current_key,
			                                     (uint8_t*)b_stream.data(),
			                                     b_stream.length());
			udp_out.send_to(server_addr,
			                (uint8_t*)encrypted.data(),
			                encrypted.length());

			// udp_out.send_to(server_addr,
			//                 (uint8_t*)b_stream.data(),
			//                 b_stream.length());

			current_key = packet.seed;

			std::cout << std::setfill(' ') << std::setprecision(3) << std::fixed
			          << "Output: r (low) = " << std::setw(8) << r
			          << ", g (med) = " << std::setw(8) << g
			          << ", b (high) = " << std::setw(8) << b << '\r'
			          << std::flush;
		} else {
			std::cout << "\nAborting Read...\n" << std::flush;
		}
	}
	std::cout << "\nLast Key: " << std::hex << current_key << "\n";
	std::cout << "Exiting..." << std::endl;
} // run_tx

void Lightshow::run_rx(Lightshow::Config config) {
	using TF::Network::Socket;
	using TF::Network::SocketAddress;

	std::mutex main_thread_lock;
	main_thread_lock.lock();

	Socket udp_in(config.server_port);

	local_shutdown = [&main_thread_lock](int signal) {
		if (signal == SIGINT) {
			main_thread_lock.unlock();
		}
	};

	std::signal(SIGINT, [](int signal) { local_shutdown(signal); });

	// Setup GPIO
	GPIOInterface blue_lights("17", GPIO_DIR_OUT);
	GPIOInterface red_lights("22", GPIO_DIR_OUT);
	GPIOInterface green_lights("27", GPIO_DIR_OUT);

	// Setup PWM interface.
	SoftPWMControl blue_pwm(blue_lights);
	SoftPWMControl red_pwm(red_lights);
	SoftPWMControl green_pwm(green_lights);

	// Setup incoming data handler.
	uint64_t current_key = config.initial_key;
	udp_in.set_on_recieve([&](SocketAddress addr, size_t length, uint8_t* msg) {
		(void)addr;

		auto decrypted = TF::Crypto::decrypt(current_key, msg, length);
		auto packet =
		    extract_remote_msg((uint8_t*)decrypted.data(), decrypted.length());

		// auto packet = extract_remote_msg(msg, length);

		if (packet.seed != UINT64_MAX)
			current_key = packet.seed;

		red_pwm.set_duty_cycle(packet.r);
		green_pwm.set_duty_cycle(packet.g);
		blue_pwm.set_duty_cycle(packet.b);

		std::cout << std::setfill(' ') << std::setprecision(3) << std::fixed
		          << "Output: r (low) = " << std::setw(8) << packet.r
		          << ", g (med) = " << std::setw(8) << packet.g
		          << ", b (high) = " << std::setw(8) << packet.b << '\r'
		          << std::flush;
	});

	// Initialize PWM.
	blue_pwm.start();
	red_pwm.start();
	green_pwm.start();

	// Begin listening
	udp_in.listen(config.server_port);

	// Lock thread to wait for signal
	main_thread_lock.lock();

	// Cleanup
	blue_pwm.stop();
	red_pwm.stop();
	green_pwm.stop();

	// Neat.
	std::cout << "Exiting..." << std::endl;
}
