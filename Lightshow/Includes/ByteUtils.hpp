/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   ByteUtils.hpp                                 <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/23 11:30:26 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/24 15:32:54 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef TF_BYTE_UTILS_HPP
#define TF_BYTE_UTILS_HPP

#include <cstdint>

namespace TF {
namespace ByteUtils {

bool __is_big_endian() {
	union {
		uint32_t val;
		uint8_t  bytes[sizeof(uint32_t)];
	} endian_check = {0x01020304};

	return (endian_check.bytes[0] == 0x01);
}

template <typename T> T bswp(T value) {
	uint8_t  rval[sizeof(T)];
	uint8_t* val_ptr = (uint8_t*)&value;

	for (uint64_t i = 0; i < sizeof(T); ++i) {
		rval[i] = val_ptr[sizeof(T) - 1 - i];
	}

	return *(T*)rval;
}

template <typename T> T to_big_endian(T value) {
	if (__is_big_endian())
		return value;
	bswp(value);
}

template <typename T> T from_big_endian(T value) {
	if (__is_big_endian())
		return value;
	bswp(value);
}

} // namespace ByteUtils
} // namespace TF

#endif
