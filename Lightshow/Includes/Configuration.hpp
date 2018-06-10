/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Configuration.hpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/11 03:09:06 by prp              2E 54 65 63 68          */
/*   Updated: 2018/03/30 19:59:15 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "INIReader.h"

#include <iostream>
#include <string>

namespace Lightshow {

constexpr auto k_section = "lightshow_pi";

constexpr auto k_sample_freq = "i_sample_freq";
constexpr auto k_framerate   = "i_framerate";

constexpr auto k_smoothing = "b_smoothing";

constexpr auto k_floor = "f_floor";

constexpr auto k_low_cutoff  = "f_low_cutoff";
constexpr auto k_mid_cutoff  = "f_mid_cutoff";
constexpr auto k_high_cutoff = "f_high_cutoff";

constexpr auto k_low_mult  = "f_low_mult";
constexpr auto k_mid_mult  = "f_mid_mult";
constexpr auto k_high_mult = "f_high_mult";

class Configuration {

	int sample_freq = 44100;
	int framerate   = 60;

	bool smoothing = false;

	float floor		  = 0.0f;
	float low_cutoff  = 25.0f;
	float mid_cutoff  = 50.0f;
	float high_cutoff = 75.0f;

	float low_mult  = 1.0f;
	float mid_mult  = 1.0f;
	float high_mult = 1.0f;

public:
	Configuration() {}

	Configuration(const std::string& filepath) {
		INIReader reader(filepath);
		if (reader.ParseError()) {
			std::cerr << "Configuration Error [init(filepath)]: Could not "
						 "parse config file.\n"
					  << "Using default values instead." << std::endl;
			return;
		}
		this->sample_freq =
			reader.GetInteger(k_section, k_sample_freq, this->sample_freq);
		this->framerate =
			reader.GetInteger(k_section, k_framerate, this->framerate);
		this->smoothing =
			reader.GetBoolean(k_section, k_smoothing, this->smoothing);

		this->floor = reader.GetReal(k_section, k_floor, this->floor);

		this->low_cutoff =
			reader.GetReal(k_section, k_low_cutoff, this->low_cutoff);
		this->mid_cutoff =
			reader.GetReal(k_section, k_mid_cutoff, this->mid_cutoff);
		this->high_cutoff =
			reader.GetReal(k_section, k_high_cutoff, this->high_cutoff);

		this->low_mult = reader.GetReal(k_section, k_low_mult, this->low_mult);
		this->mid_mult = reader.GetReal(k_section, k_mid_mult, this->mid_mult);
		this->high_mult =
			reader.GetReal(k_section, k_high_mult, this->high_mult);
	}

	inline int get_sample_freq() const { return this->sample_freq; }
	inline int get_framerate() const { return this->framerate; }

	inline bool get_smoothing() const { return this->smoothing; }

	inline float get_low_cutoff() const { return this->low_cutoff; }
	inline float get_mid_cutoff() const { return this->mid_cutoff; }
	inline float get_high_cutoff() const { return this->high_cutoff; }

	inline float get_floor() const { return this->floor; }
	inline float get_low_mult() const { return this->low_mult; }
	inline float get_mid_mult() const { return this->mid_mult; }
	inline float get_high_mult() const { return this->high_mult; }
};
}
#endif