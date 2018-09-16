/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Lightshow.cpp                                 <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/11 13:29:03 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/16 10:59:58 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Lightshow.hpp"
#include "DebugTools.hpp"

#include <cstring>
#include <random>

using namespace Lightshow;
using namespace TF::Debug;

NetworkPacket Lightshow::create_packet(float r, float g, float b) {
	std::random_device                      rd;
	std::mt19937_64                         mt_generator(rd());
	std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX - 1);

	NetworkPacket packet;
	packet.r    = r;
	packet.g    = g;
	packet.b    = b;
	packet.seed = dist(mt_generator);

	return packet;
}

std::basic_string<uint8_t> Lightshow::build_remote_msg(NetworkPacket packet) {
	std::basic_string<uint8_t> raw_msg;
	raw_msg.append((uint8_t*)"WOOF", 4);
	raw_msg.append((uint8_t*)&packet.r, sizeof(packet.r));
	raw_msg.append((uint8_t*)&packet.g, sizeof(packet.g));
	raw_msg.append((uint8_t*)&packet.b, sizeof(packet.b));
	raw_msg.append((uint8_t*)&packet.seed, sizeof(packet.seed));
	uint8_t crc = 0;
	for (auto byte : raw_msg) {
		crc ^= byte;
	}
	raw_msg.push_back(crc);
	return raw_msg;
}

NetworkPacket Lightshow::extract_remote_msg(uint8_t* msg, size_t len) {
	NetworkPacket extracted;
	extracted.seed = UINT64_MAX;
	if (std::strncmp((char*)msg, "WOOF", 4)) {
		print_warning_line("WRN -> [extract_remote_msg]: Invalid packet.");
		return extracted;
	}

	uint8_t crc = 0;
	for (size_t i = 0; i < len - 1; ++i) {
		crc ^= msg[i];
	}

	if (crc != msg[len - 1]) {
		if (std::strncmp((char*)msg, "WOOF", 4)) {
			print_warning_line(
			    "WRN -> [extract_remote_msg]: Invalid Packet CRC");
			return extracted;
		}
	}

	float* msg_content = (float*)&msg[4];

	extracted.r = msg_content[0];
	extracted.g = msg_content[1];
	extracted.b = msg_content[2];

	uint64_t* msg_seed = (uint64_t*)&msg_content[3];

	extracted.seed = *msg_seed;

	return extracted;
}

double Lightshow::sum_samples(fftw_complex* samples,
                              size_t        start_idx,
                              size_t        end_idx) {
	double sum = 0;
	while (start_idx < end_idx) {
		double real      = samples[start_idx][0];
		double imaginary = samples[start_idx][0];
		sum += std::sqrt(real * real + imaginary * imaginary);
		++start_idx;
	}
	return sum;
}
