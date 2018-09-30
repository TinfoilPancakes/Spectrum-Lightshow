/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   ByteUtils.hpp                                 <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/23 11:30:26 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/26 07:57:15 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef TF_BYTE_UTILS_HPP
#define TF_BYTE_UTILS_HPP

#include <cstdint>

namespace TF {
namespace ByteUtils {

inline bool _is_big_endian() {
	union {
		uint32_t val;
		uint8_t  bytes[sizeof(uint32_t)];
	} endian_check = {0x01020304};

	return (endian_check.bytes[0] == 0x01);
}

template <typename T> T bswp(T value) {
	T rval;

	uint8_t* rv_ptr  = (uint8_t*)&rval;
	uint8_t* val_ptr = (uint8_t*)&value;

	for (uint64_t i = 0; i < sizeof(T); ++i) {
		rv_ptr[i] = val_ptr[sizeof(T) - 1 - i];
	}

	return rval;
}

template <typename T> T to_big_endian(T value) {
	if (_is_big_endian())
		return value;
	return bswp(value);
}

template <typename T> T from_big_endian(T value) {
	if (_is_big_endian())
		return value;
	return bswp(value);
}

} // namespace ByteUtils
} // namespace TF

#endif
