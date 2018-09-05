/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   SocketAddress.hpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/08/11 08:55:13 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/05 09:59:20 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */
#ifndef SOCKET_ADDRESS_HPP
#define SOCKET_ADDRESS_HPP

#include <cstdlib>
#include <cstring>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace TF {
namespace Network {

class SocketAddress {

	using t_sockaddr_in = struct sockaddr_in;

	t_sockaddr_in address_struct;

public:
	SocketAddress() { bzero(&this->address_struct, sizeof(t_sockaddr_in)); }

	SocketAddress(uint16_t sin_family, uint16_t sin_port, uint32_t addr) {
		this->address_struct.sin_family		 = sin_family;
		this->address_struct.sin_port		 = htons(sin_port);
		this->address_struct.sin_addr.s_addr = htonl(addr);
	}

	inline auto get_family() { return this->address_struct.sin_family; }

	inline auto get_port() { return ntohs(this->address_struct.sin_port); }

	inline auto get_address() {
		return ntohl(this->address_struct.sin_addr.s_addr);
	}

	inline void set_family(uint16_t sin_family) {
		this->address_struct.sin_family = sin_family;
	}

	inline void set_port(uint16_t sin_port) {
		this->address_struct.sin_port = htons(sin_port);
	}

	inline void set_address(uint32_t address) {
		this->address_struct.sin_addr.s_addr = htonl(address);
	}

	inline t_sockaddr_in get_struct() { return this->address_struct; }

	inline t_sockaddr_in* get_struct_ptr() { return &this->address_struct; }
};
} // namespace Network
} // namespace TF

#endif
