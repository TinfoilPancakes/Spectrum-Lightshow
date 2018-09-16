/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Lightshow.hpp                                 <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/11 13:03:56 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/16 02:48:41 by prp              50 2E 52 2E 50          */
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

NetworkPacket create_packet(float r, float g, float b);

std::basic_string<uint8_t> build_remote_msg(NetworkPacket packet);

NetworkPacket extract_remote_msg(uint8_t* msg, size_t len);

double sum_samples(fftw_complex* samples, size_t start_idx, size_t end_idx);

} // namespace Lightshow

#endif
