/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   test_pulse.cpp                                <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 04:38:05 by prp              2E 54 65 63 68          */
/*   Updated: 2018/03/01 11:53:47 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "FFTransformer.hpp"
#include "PulseAudioSource.hpp"

#include <cmath>
#include <csignal>
#include <cstring>
#include <iomanip>
#include <iostream>

constexpr size_t sample_size = 44100 / 60;

bool to_continue = true;

void handle_interrupt(int signal) {
	if (signal == SIGINT)
		to_continue = false;
}

int main(void) {

	std::signal(SIGINT, &handle_interrupt);

	auto spec = Lightshow::PulseAudioSource::default_spec;

	{
		Lightshow::PCMStereoSample buffer[sample_size];
		std::memset(buffer, 0, sizeof(buffer));

		auto source = Lightshow::PulseAudioSource(
			"Blargo",
			"gerp gork",
			&spec,
			Lightshow::PulseAudioSource::get_default_source_name());

		auto transormer = FFTransformer(1, sample_size);
		std::cout << "Started Correctly..." << std::endl;
		long f_count		  = 0;
		uint index_cuttofs[2] = {15, 74};

		double left_values[3]{0};

		while (to_continue) {
			if (!source.read_into(buffer, sample_size))
				std::cerr << "Error: PulseAudioSource read_into failed."
						  << std::endl;
			else {
				++f_count;
				transormer.fill_input_buffer(
					0, buffer, [](void* in, size_t id) -> double {
						Lightshow::PCMStereoSample* input =
							(Lightshow::PCMStereoSample*)in;
						return (double)(input[id].left + input[id].right);
					});
				transormer.calculate_dft();

				index_cuttofs[0] =
					transormer.output_sample_count * 15.0 / 736.0;
				index_cuttofs[1] =
					transormer.output_sample_count * 74.0 / 736.0;

				auto i = 0u;

				auto fftw_left = transormer.get_output(0);
				left_values[0] = 0;
				for (; i < index_cuttofs[0]; ++i) {
					auto real	  = fftw_left[i][0];
					auto imaginary = fftw_left[i][1];
					auto magnitude =
						std::sqrt(real * real + imaginary * imaginary);
					left_values[0] += magnitude;
				}

				left_values[1] = 0;
				for (; i < index_cuttofs[1]; ++i) {
					auto real	  = fftw_left[i][0];
					auto imaginary = fftw_left[i][1];
					auto magnitude =
						std::sqrt(real * real + imaginary * imaginary);
					left_values[1] += magnitude;
				}

				left_values[2] = 0;
				for (; i < transormer.output_sample_count; ++i) {
					auto real	  = fftw_left[i][0];
					auto imaginary = fftw_left[i][1];
					auto magnitude =
						std::sqrt(real * real + imaginary * imaginary);
					left_values[2] += magnitude;
				}

				float r = (left_values[0] / 1000000) / 50;
				float g = (left_values[1] / 1000000) / 50;
				float b = (left_values[2] / 1000000) / 70;
				// normalize_volumes(r, g, b);
				std::cout << "Size: " << transormer.output_sample_count << ' ';
				std::cout << std::fixed << std::setw(15) << std::setfill(' ')
						  << std::setprecision(4) << r << ' ' << std::setw(15)
						  << std::setfill(' ') << std::setprecision(4) << g
						  << ' ' << std::setw(15) << std::setfill(' ')
						  << std::setprecision(4) << b << '\r' << std::flush;
			}
		}
	}
	std::cout << "\nExiting..." << std::endl;
	return 0;
}
