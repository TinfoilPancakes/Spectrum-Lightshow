/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   main.cpp                                      <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:50:59 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/17 04:44:44 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "DebugTools.hpp"
#include "Lightshow.hpp"

using namespace Lightshow;

int main(int argc, char const* argv[]) {
	// Parse Configs.
	Config config;

	if (argc > 1)
		config = Config(argv[1]);

	if (config.is_local)
		run_local(config);
	else {
		if (config.is_server)
			run_rx(config);
		else
			run_tx(config);
	}

	return 0;
}
