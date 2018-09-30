/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Config.cpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/04 09:39:06 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/30 12:45:18 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "DebugTools.hpp"

using namespace Lightshow;

namespace {
constexpr auto k_section = "lightshow_pi";

constexpr auto k_is_local  = "b_is_local";
constexpr auto k_is_server = "b_is_server";

constexpr auto k_initial_key = "i_initial_key";
constexpr auto k_server_addr = "s_server_addr";
constexpr auto k_server_port = "i_server_port";

constexpr auto k_sample_freq = "i_sample_freq";
constexpr auto k_framerate   = "i_framerate";

constexpr auto k_smoothing = "b_smoothing";

constexpr auto k_floor       = "f_floor";
constexpr auto k_low_cutoff  = "f_low_cutoff";
constexpr auto k_mid_cutoff  = "f_mid_cutoff";
constexpr auto k_high_cutoff = "f_high_cutoff";

constexpr auto k_low_mult  = "f_low_mult";
constexpr auto k_mid_mult  = "f_mid_mult";
constexpr auto k_high_mult = "f_high_mult";
} // namespace

Config::Config(const std::string& filepath) {
	this->load_config_file(filepath);
}

bool Config::load_config_file(const std::string& file) {
	using TF::Debug::print_error_line;

	INIReader reader(file);

	if (reader.ParseError()) {
		print_error_line("ERR -> [Config::Config]: Could not "
		                 "parse configuration file. Using "
		                 "default values instead.");
		return false;
	}

	is_local  = reader.GetBoolean(k_section, k_is_local, is_local);
	is_server = reader.GetBoolean(k_section, k_is_server, is_server);

	auto temp_key = reader.Get(k_section, k_initial_key, "0x00676f6f6769726c");
	initial_key   = std::stoull(temp_key.c_str(), nullptr, 0);

	server_port = reader.GetInteger(k_section, k_server_port, server_port);

	server_addr = reader.Get(k_section, k_server_addr, server_addr);

	sample_freq = reader.GetInteger(k_section, k_sample_freq, sample_freq);
	framerate   = reader.GetInteger(k_section, k_framerate, framerate);

	floor       = reader.GetReal(k_section, k_floor, floor);
	low_cutoff  = reader.GetReal(k_section, k_low_cutoff, low_cutoff);
	mid_cutoff  = reader.GetReal(k_section, k_mid_cutoff, mid_cutoff);
	high_cutoff = reader.GetReal(k_section, k_high_cutoff, high_cutoff);

	low_mult  = reader.GetReal(k_section, k_low_mult, low_mult);
	mid_mult  = reader.GetReal(k_section, k_mid_mult, mid_mult);
	high_mult = reader.GetReal(k_section, k_high_mult, high_mult);

	return true;
}
