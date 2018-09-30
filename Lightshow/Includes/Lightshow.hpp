/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Lightshow.hpp                                 <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/11 13:03:56 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/30 12:11:18 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIGHTSHOW_HPP
#define LIGHTSHOW_HPP

#include <fftw3.h>

#include "Config.hpp"
#include "Socket.hpp"

namespace Lightshow {

struct s_network_packet {
	uint64_t seed = 0;

	float r = 0;
	float g = 0;
	float b = 0;
};

using NetworkPacket = struct s_network_packet;

void run_local(Config& config);

void run_tx(Config& config);

void run_rx(Config& config);

} // namespace Lightshow

#endif
