/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Crypto.hpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/15 09:58:24 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/16 02:27:17 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef TF_CRYPTO_HPP
#define TF_CRYPTO_HPP

#include <cstdint>
#include <string>

namespace TF {
namespace Crypto {

std::basic_string<uint8_t> encrypt(uint64_t seed, uint8_t* msg, size_t len);

std::basic_string<uint8_t> decrypt(uint64_t seed, uint8_t* msg, size_t len);

} // namespace Crypto
} // namespace TF

#endif
