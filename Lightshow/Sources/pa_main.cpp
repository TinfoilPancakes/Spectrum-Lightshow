/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   pa_main.cpp                                   <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:50:59 by prp              2E 54 65 63 68          */
/*   Updated: 2018/03/01 11:52:48 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstring>
#include <iomanip>

#include "PulseAudioSource.hpp"

constexpr size_t buffer_size = 44100 / 60;

bool exit_signal = false;

void interrupt_signal(int signal_code) {
	if (signal_code == SIGINT)
		exit_signal = true;
}

int main(int argc, char const* argv[]) {
	(void)argc;
	// Setup Signal Handlers.
	std::signal(SIGINT, interrupt_signal);
	// Test the scoping and destructor.
	{
		// Default Sound Spec Instance.
		auto spec = Lightshow::PulseAudioSource::default_spec;
		// Buffer Initialization
		Lightshow::PCMStereoSample input_buffer[buffer_size];
		// Create Source...
		Lightshow::PulseAudioSource source(
			argv[0],
			"recording",
			&spec,
			Lightshow::PulseAudioSource::get_default_source_name());
		// Read loop...
		while (!exit_signal) {
			std::memset(input_buffer, 0, sizeof(input_buffer));
			if (source.read_into(input_buffer, sizeof(input_buffer))) {
				// Sum volume...
				long sum_left  = 0;
				long sum_right = 0;
				for (size_t i = 0; i < buffer_size; ++i) {
					sum_left += input_buffer[i].left;
					sum_right += input_buffer[i].right;
				}
				// Print...
				std::cout << '\r' << "L: " << std::setw(8) << std::setfill(' ')
						  << sum_left << " | "
						  << "R: " << std::setw(8) << std::setfill(' ')
						  << sum_right << std::flush;
			} else {
				std::cout << "Aboring Read..." << std::flush;
			}
		}
	}
	return 0;
}
