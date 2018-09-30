/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   main.cpp                                      <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:50:59 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/30 12:11:07 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "DebugTools.hpp"
#include "Lightshow.hpp"

int main(int argc, char const* argv[]) {
	// Parse Configs.
	Lightshow::Config config;

	if (argc > 1)
		config = Lightshow::Config(argv[1]);

	if (config.is_local)
		Lightshow::run_local(config);
	else {
		if (config.is_server)
			Lightshow::run_rx(config);
		else
			Lightshow::run_tx(config);
	}

	return 0;
}
