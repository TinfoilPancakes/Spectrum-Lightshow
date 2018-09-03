/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Socket.cpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/01 09:24:53 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/02 15:00:29 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

using namespace Lightshow;

Socket::Socket() {
	this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (this->socket_fd < 0) {
		int last_err = errno;
		std::cerr << "Error opening socket: " << strerror(last_err)
				  << std::endl;
	}

	this->thread_continue = false;
}

Socket::Socket(uint16_t port_number) : Socket() {
	this->port_number = port_number;
}

Socket::~Socket() {
	this->stop();
	close(this->socket_fd);
};

void Socket::listen_method(Socket* socket, size_t buffer_size) {

	std::vector<uint8_t> buffer_obj;
	buffer_obj.resize(buffer_size);

	uint8_t* buffer = buffer_obj.data();

	t_sockaddr_in reciever;

	socklen_t reciever_size = sizeof(reciever);

	// File Descriptor for poll
	// Bitmask for events to listen for
	// Number of events returned (set by poll(...))
	struct pollfd poll_args[2] =
		{{socket->socket_fd, POLLIN | POLLHUP | POLLERR | POLLNVAL, 0},
		 {socket->signal_fd, POLLIN | POLLHUP | POLLERR | POLLNVAL, 0}};

	while (socket->thread_continue) {

		bzero(&reciever, sizeof(t_sockaddr_in));
		bzero(buffer, buffer_size);

		// -1 for last param sets to wait indefinitely.
		int event_count = poll(poll_args, 2, -1);

		if (event_count < 0) {
			std::cout
				<< "poll(...) returned error: skipping loop iteration...\n";
			continue;
		}

		if (poll_args[0].revents & POLLIN) {
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
				std::cout << "DBG: No handler\n";
		}
	}
	close(socket->signal_fd);
}

void Socket::listen(size_t buffer_size) {

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

	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGUSR1);
	sigprocmask(SIG_BLOCK, &signal_mask, nullptr);

	this->signal_fd = signalfd(-1, &signal_mask, 0);

	if (this->signal_fd <= 0) {
		std::cerr << "Error creating signalfd\n";
		return;
	}

	std::cout << "DBG: Starting Socket Thread Listener..." << std::endl;
	this->thread_continue = true;
	this->listener_thread =
		std::thread(Socket::listen_method, this, buffer_size);
}

void Socket::stop() {
	if (!this->listener_thread.joinable())
		return;

	this->thread_continue = false;
	std::cout << "DBG: Stopping socket thread..." << std::endl;
	kill(getpid(), SIGUSR1);
	this->listener_thread.join();
	std::cout << "DBG: Done, exiting stop method.\n";
}

bool Socket::send_to(SocketAddress  address,
					 const uint8_t* msg_buffer,
					 size_t			msg_length) {

	auto addr_struct = address.get_struct();
	auto sent		 = sendto(this->socket_fd,
						  msg_buffer,
						  msg_length,
						  0,
						  (t_sockaddr*)&addr_struct,
						  sizeof(addr_struct));

	if (sent < 0) {
		int last_error = errno;
		std::cerr << "Error in call to sendto(...): " << strerror(last_error)
				  << std::endl;
		return false;
	}
	return true;
}

void Socket::set_on_recieve(const t_recv_handler& handler) {
	this->on_recieve = handler;
}