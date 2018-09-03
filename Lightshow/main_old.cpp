/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   main_old.cpp                                  <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2017/09/15 22:43:45 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/02 14:03:09 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "SharedMemoryObject.hpp"
#include "SoftPWMControl.hpp"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <signal.h>

bool g_to_continue = true;

void handle_interrupt(int signal) {
	if (signal == SIGINT) {
		std::cout << "Ending..." << std::endl;
		g_to_continue = false;
	}
}

typedef struct {
	bool   is_stereo		= false;
	size_t fftw_output_size = 0;
} t_external_frame;

int main(int argc, char const* argv[]) {
	(void)argc;
	(void)argv;
	{
		extension::SharedMemoryObject<32768> shared_area;
		shared_area.init("/cli-visualizer-share");
		double fftw_left[4096]{0};
		double fftw_right[4096]{0};

		GPIOInterface blueLights("17", GPIO_DIR_OUT);
		GPIOInterface redLights("22", GPIO_DIR_OUT);
		GPIOInterface greenLights("27", GPIO_DIR_OUT);

		SoftPWMControl bluePWM(&blueLights);
		SoftPWMControl redPWM(&redLights);
		SoftPWMControl greenPWM(&greenLights);

		bluePWM.setDutyCycle(0);
		redPWM.setDutyCycle(0);
		greenPWM.setDutyCycle(0);

		bluePWM.init();
		redPWM.init();
		greenPWM.init();

		// std::thread lightshow_thread(wait_for_quit);

		g_to_continue = true;

		signal(SIGINT, &handle_interrupt);

		t_external_frame frame;
		auto			 data = shared_area.get_data_ptr();

		/*  Frequency to index.
			200hz 3
			300hz 5
			400hz 7
			600hz 10
			700hz 12
			800hz 13
			1200hz 19
			2000hz 34
			2500hz 42

			736 / 15
			736 / 74
		 */

		uint index_cuttofs[2] = {15, 74};

		double left_values[3]{0};
		double right_values[3]{0};

		std::cout << "Starting Lights... data_ptr = " << (void*)data
				  << std::endl;
		while (g_to_continue) {
			auto start = std::chrono::system_clock::now();

			shared_area.lock();

			frame = *(t_external_frame*)data;

			memcpy(fftw_left,
				   data + sizeof(t_external_frame),
				   sizeof(double) * 2 * frame.fftw_output_size);

			if (frame.is_stereo)
				memcpy(fftw_right,
					   data + sizeof(t_external_frame) +
						   sizeof(double) * 2 * frame.fftw_output_size,
					   sizeof(double) * 2 * frame.fftw_output_size);

			shared_area.unlock();

			index_cuttofs[0] = frame.fftw_output_size * 15.0 / 736.0;
			index_cuttofs[1] = frame.fftw_output_size * 74.0 / 736.0;

			auto i = 0u;

			left_values[0] = 0;
			for (; i < index_cuttofs[0]; ++i) {
				auto real	  = fftw_left[i * 2];
				auto imaginary = fftw_left[i * 2 + 1];
				auto magnitude = sqrt(real * real + imaginary * imaginary);
				left_values[0] += magnitude;
			}

			left_values[1] = 0;
			for (; i < index_cuttofs[1]; ++i) {
				auto real	  = fftw_left[i * 2];
				auto imaginary = fftw_left[i * 2 + 1];
				auto magnitude = sqrt(real * real + imaginary * imaginary);
				left_values[1] += magnitude;
			}

			left_values[2] = 0;
			for (; i < frame.fftw_output_size; ++i) {
				auto real	  = fftw_left[i * 2];
				auto imaginary = fftw_left[i * 2 + 1];
				auto magnitude = sqrt(real * real + imaginary * imaginary);
				left_values[2] += magnitude;
			}

			if (frame.is_stereo) {
				i				= 0;
				right_values[0] = 0;
				for (; i < index_cuttofs[0]; ++i) {
					auto real	  = fftw_right[i * 2];
					auto imaginary = fftw_right[i * 2 + 1];
					auto magnitude = sqrt(real * real + imaginary * imaginary);
					right_values[0] += magnitude;
				}

				right_values[1] = 0;
				for (; i < index_cuttofs[1]; ++i) {
					auto real	  = fftw_right[i * 2];
					auto imaginary = fftw_right[i * 2 + 1];
					auto magnitude = sqrt(real * real + imaginary * imaginary);
					right_values[1] += magnitude;
				}

				right_values[2] = 0;
				for (; i < frame.fftw_output_size; ++i) {
					auto real	  = fftw_right[i * 2];
					auto imaginary = fftw_right[i * 2 + 1];
					auto magnitude = sqrt(real * real + imaginary * imaginary);
					right_values[2] += magnitude;
				}
			}
			float r = (left_values[0] / 1000000) / 50;
			float g = (left_values[1] / 1000000) / 50;
			float b = (left_values[2] / 1000000) / 70;
			// normalize_volumes(r, g, b);
			std::cout << "Size: " << frame.fftw_output_size
					  << ", Stereo: " << frame.is_stereo << ' ';
			std::cout << std::setw(15) << r << ' ' << std::setw(15) << g << ' '
					  << std::setw(15) << b << '\r' << std::flush;

			redPWM.setDutyCycle(r);
			greenPWM.setDutyCycle(g);
			bluePWM.setDutyCycle(b);

			auto duration = std::chrono::system_clock::now() - start;
			if (duration < std::chrono::milliseconds(34))
				std::this_thread::sleep_for(std::chrono::milliseconds(34) -
											duration);
		}
		std::cout << "Loop Stopped" << std::endl;
		redPWM.stop();
		greenPWM.stop();
		bluePWM.stop();
	}
	// lightshow_thread.join();

	return 0;
}
