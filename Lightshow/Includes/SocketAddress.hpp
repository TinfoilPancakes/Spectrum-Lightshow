/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   SocketAddress.hpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/08/11 08:55:13 by prp              2E 54 65 63 68          */
/*   Updated: 2018/08/20 21:09:19 by prp              50 2E 52 2E 50          */
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

namespace Lightshow {
using t_sockaddr_in = struct sockaddr_in;
class SocketAddress {

	t_sockaddr_in address_struct;

public:
	SocketAddress() { bzero(&this->address_struct, sizeof(t_sockaddr_in)); }

	SocketAddress(uint16_t sin_family, uint16_t sin_port, uint32_t addr) {
		this->address_struct.sin_family		 = sin_family;
		this->address_struct.sin_port		 = htons(sin_port);
		this->address_struct.sin_addr.s_addr = htonl(addr);
	}

	auto get_family() { return this->address_struct.sin_family; }

	auto get_port() { return ntohs(this->address_struct.sin_port); }

	auto get_address() { return ntohl(this->address_struct.sin_addr.s_addr); }

	void set_family(uint16_t sin_family) {
		this->address_struct.sin_family = sin_family;
	}

	void set_port(uint16_t sin_port) {
		this->address_struct.sin_port = htons(sin_port);
	}

	void set_address(uint32_t address) {
		this->address_struct.sin_addr.s_addr = htonl(address);
	}

	t_sockaddr_in get_struct() { return this->address_struct; }

	t_sockaddr_in* get_struct_ptr() { return &this->address_struct; }
};
} // namespace Lightshow

#endif
