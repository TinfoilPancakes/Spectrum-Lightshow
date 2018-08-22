/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Socket.hpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/08/03 00:42:51 by prp              2E 54 65 63 68          */
/*   Updated: 2018/08/20 21:41:59 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "SocketAddress.hpp"

namespace Lightshow {
using t_sockaddr	= struct sockaddr;
using t_sockaddr_in = struct sockaddr_in;

class Socket {

	int		 socket_fd   = -1;
	uint16_t port_number = 0xe419;

	std::thread		 listener_thread;
	std::atomic_bool thread_continue;

	std::function<void(SocketAddress, size_t, uint8_t*)> on_recieve = nullptr;

	template <size_t buffer_size> static void listen_method(Socket* socket) {

		uint8_t buffer[buffer_size];

		t_sockaddr_in reciever;

		socklen_t reciever_size = sizeof(reciever);

		while (socket->thread_continue) {

			bzero(&reciever, sizeof(t_sockaddr_in));
			bzero(buffer, buffer_size);
			std::cout << "awaiting read...\n";
			ssize_t bytes_read = recvfrom(socket->socket_fd,
										  buffer,
										  buffer_size,
										  0,
										  (t_sockaddr*)&reciever,
										  &reciever_size);

			SocketAddress address(reciever.sin_family,
								  reciever.sin_port,
								  reciever.sin_addr.s_addr);

			if (socket->on_recieve != nullptr)
				socket->on_recieve(address, bytes_read, buffer);
			else
				std::cout << "No handler\n";
		}
	}

public:
	Socket() {
		this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

		if (this->socket_fd < 0) {
			int last_err = errno;
			std::cerr << "Error opening socket: " << strerror(last_err)
					  << std::endl;
		}

		this->thread_continue = false;
	}

	Socket(uint16_t port_number) : Socket() { this->port_number = port_number; }

	~Socket() { this->stop(); };

	template <size_t buffer_size> void listen() {

		t_sockaddr_in address;
		bzero(&address, sizeof(t_sockaddr_in));

		address.sin_family		= AF_INET;
		address.sin_port		= htons(this->port_number);
		address.sin_addr.s_addr = INADDR_ANY;

		int b_result =
			bind(this->socket_fd, (t_sockaddr*)&address, sizeof(address));

		if (b_result < 0) {
			int last_err = errno;
			std::cerr << "Error binding socket to address: "
					  << "Message: " << strerror(last_err) << std::endl;
			return;
		}

		std::cout << "DBG: Starting Socket Thread Listener..." << std::endl;
		this->thread_continue = true;
		this->listener_thread =
			std::thread(Socket::listen_method<buffer_size>, this);
	}

	void stop() {
		if (!this->listener_thread.joinable())
			return;
		std::cout << "DBG: Stopping socket thread..." << std::endl;
		if (this->thread_continue)
			this->thread_continue = false;
		this->listener_thread.join();
	}

	bool send_to(SocketAddress  address,
				 const uint8_t* msg_buffer,
				 size_t			msg_length) {

		auto as_struct = address.get_struct();
		auto sent	  = sendto(this->socket_fd,
							msg_buffer,
							msg_length,
							0,
							(t_sockaddr*)&as_struct,
							sizeof(as_struct));

		if (sent < 0) {
			int last_error = errno;
			std::cerr << "Error in call to sendto(...): "
					  << strerror(last_error) << std::endl;
			return false;
		}
		return true;
	}

	void set_on_recieve(
		const std::function<void(SocketAddress, size_t, uint8_t*)>& handler) {
		this->on_recieve = handler;
	}
};
} // namespace Lightshow

#endif
