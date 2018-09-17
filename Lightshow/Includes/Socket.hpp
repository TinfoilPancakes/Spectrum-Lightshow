/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Socket.hpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/08/03 00:42:51 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/17 04:36:43 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <atomic>
#include <functional>
#include <thread>

#include <netinet/in.h>
#include <netinet/ip.h>

#include "SocketAddress.hpp"

namespace TF {
namespace Network {

class Socket {

	using t_sockaddr    = struct sockaddr;
	using t_sockaddr_in = struct sockaddr_in;
	using t_timeval     = struct timeval;

	using t_recv_handler = std::function<void(SocketAddress, size_t, uint8_t*)>;

	int      socket_fd   = -1;
	int      signal_fd   = -1;
	uint16_t port_number = 0xe419;

	std::thread      listener_thread;
	std::atomic_bool thread_continue;

	t_recv_handler on_recieve = nullptr;

	static void listen_method(Socket* socket, size_t buffer_size);

public:
	Socket();

	Socket(uint16_t port_number);

	~Socket();

	void listen(size_t buffer_size);

	void stop();

	bool send_to(SocketAddress  address,
	             const uint8_t* msg_buffer,
	             size_t         msg_length);

	void set_on_recieve(const t_recv_handler& handler);
};
} // namespace Network
} // namespace TF

#endif
