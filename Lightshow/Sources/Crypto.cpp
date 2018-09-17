/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Crypto.cpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/15 10:12:47 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/16 13:17:49 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Crypto.hpp"

#include <random>

using namespace TF::Crypto;

struct s_bitset {
	unsigned b0 : 1;
	unsigned b1 : 1;
	unsigned b2 : 1;
	unsigned b3 : 1;

	unsigned b4 : 1;
	unsigned b5 : 1;
	unsigned b6 : 1;
	unsigned b7 : 1;
};

using t_bitset = struct s_bitset;

union u_byte_access {
	uint8_t  val;
	t_bitset bits;
};

using t_byte_access = union u_byte_access;

uint8_t get_byte_offset(uint64_t seed) {
	uint8_t* seed_ptr = (uint8_t*)&seed;
	uint8_t* end_ptr  = seed_ptr + sizeof(seed);
	uint8_t  hash     = 0;

	while (seed_ptr < end_ptr) {
		hash ^= *seed_ptr;
		++seed_ptr;
	}

	t_byte_access extractor;
	extractor.val = hash;

	uint8_t offset = 0;
	offset |= extractor.bits.b3;
	offset <<= 1;
	offset |= extractor.bits.b4;
	offset <<= 1;
	offset |= extractor.bits.b5;
	return offset;
}

std::basic_string<uint8_t>
TF::Crypto::encrypt(uint64_t seed, uint8_t* msg, size_t len) {
	std::random_device                      rd;
	std::mt19937_64                         mt_generator(rd());
	std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

	std::basic_string<uint8_t> encrypted;

	uint64_t current_key = seed;

	for (size_t i = 0; i < len; ++i) {
		uint8_t  offset  = get_byte_offset(current_key);
		uint64_t padding = dist(mt_generator);

		((uint8_t*)&padding)[offset] = msg[i];

		uint64_t e_char = padding ^ current_key;
		encrypted.append((uint8_t*)&e_char, sizeof(e_char));

		current_key = (current_key * msg[i]) ^ padding;
	}

	return encrypted;
}

std::basic_string<uint8_t>
TF::Crypto::decrypt(uint64_t seed, uint8_t* msg, size_t len) {
	std::basic_string<uint8_t> decrypted;

	uint64_t current_key = seed;

	for (size_t i = 0; i < len; i += sizeof(seed)) {
		auto offset = get_byte_offset(current_key);

		uint64_t e_char = (*(uint64_t*)&msg[i]);
		uint64_t padded = e_char ^ current_key;

		uint8_t original = ((uint8_t*)&padded)[offset];
		decrypted.push_back(original);

		current_key = (current_key * original) ^ padded;
	}
	return decrypted;
}