/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Crypto.cpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/15 10:12:47 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/30 11:48:09 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Crypto.hpp"
#include "ByteUtils.hpp"

#include <bitset>
#include <random>

using namespace TF::Crypto;

uint8_t get_byte_offset(uint64_t seed) {

	uint8_t* seed_ptr = (uint8_t*)&seed;
	uint8_t  hash     = 0;

	for (size_t i = 0; i < sizeof(seed); ++i) {
		hash ^= seed_ptr[i];
	}

	std::bitset<8> hash_bitset(hash);
	uint8_t        offset = 0;

	offset |= hash_bitset[3];
	offset <<= 1;
	offset |= hash_bitset[5];
	offset <<= 1;
	offset |= hash_bitset[7];

	return offset;
}

std::basic_string<uint8_t>
TF::Crypto::encrypt(uint64_t seed, uint8_t* msg, size_t len) {
	using TF::ByteUtils::to_big_endian;

	std::random_device                      rd;
	std::mt19937_64                         mt_generator(rd());
	std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

	std::basic_string<uint8_t> encrypted;

	uint64_t current_key = seed;

	for (size_t i = 0; i < len; ++i) {
		uint8_t  offset  = get_byte_offset(current_key);
		uint64_t padding = dist(mt_generator);

		uint64_t extended_msg = msg[i];
		uint64_t mask         = 0xff;

		extended_msg <<= offset * 8;
		mask <<= offset * 8;
		mask = ~mask;
		padding &= mask;
		padding |= extended_msg;

		uint64_t e_char = padding ^ current_key;

		e_char = to_big_endian(e_char);

		encrypted.append((uint8_t*)&e_char, sizeof(e_char));

		current_key = (current_key * msg[i]) ^ padding;
	}

	return encrypted;
}

std::basic_string<uint8_t>
TF::Crypto::decrypt(uint64_t seed, uint8_t* msg, size_t len) {
	using TF::ByteUtils::from_big_endian;

	std::basic_string<uint8_t> decrypted;

	uint64_t current_key = seed;

	for (size_t i = 0; i < len; i += sizeof(seed)) {
		auto offset = get_byte_offset(current_key);

		uint64_t e_char = *((uint64_t*)(&msg[i]));

		e_char = from_big_endian(e_char);

		uint64_t padded = e_char ^ current_key;

		uint8_t original = (padded >> (offset * 8)) & 0xFF;
		decrypted.push_back(original);

		current_key = (current_key * original) ^ padded;
	}
	return decrypted;
}