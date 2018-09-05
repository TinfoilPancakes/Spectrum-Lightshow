/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Configuration.cpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/04 09:39:06 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/05 12:28:50 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"

#include "DebugTools.hpp"

using namespace Lightshow;
using namespace TF::Debug;

Configuration::Configuration(const std::string& filepath) {
	INIReader reader(filepath);

	if (reader.ParseError()) {
		print_error_line("ERR -> [Configuration::Configuration]: Could not "
						 "parse configuration file. Using "
						 "default values instead.");
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

	this->low_mult  = reader.GetReal(k_section, k_low_mult, this->low_mult);
	this->mid_mult  = reader.GetReal(k_section, k_mid_mult, this->mid_mult);
	this->high_mult = reader.GetReal(k_section, k_high_mult, this->high_mult);
}
