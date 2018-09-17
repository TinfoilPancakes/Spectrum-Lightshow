/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Config.hpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/11 03:09:06 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/16 19:46:52 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "INIReader.h"

#include <mutex>
#include <string>

namespace Lightshow {

class Config {
public:
	bool is_local   = true;
	bool is_server  = false;
	bool random_key = false;

	std::string server_addr = "#";

	uint64_t initial_key = 0x676f6f6769726c;
	int      server_port = 2533;

	int sample_freq = 44100;
	int framerate   = 60;

	float floor       = 0.0f;
	float low_cutoff  = 25.0f;
	float mid_cutoff  = 50.0f;
	float high_cutoff = 75.0f;

	float low_mult  = 1.0f;
	float mid_mult  = 1.0f;
	float high_mult = 1.0f;

	Config() {}

	Config(const std::string& filepath);

	bool load_config_file(const std::string& file);

	bool save_config_file(const std::string& file);
};
} // namespace Lightshow
#endif
