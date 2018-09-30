/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   Socket.cpp                                    <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/01 09:24:53 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/30 13:05:08 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "DebugTools.hpp"

#include <vector>

#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <poll.h>
#include <signal.h>
#include <unistd.h>

using namespace TF::Network;

Socket::Socket() {
	using TF::Debug::print_error_line;

	this->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (this->socket_fd < 0) {
		int last_err = errno;
		print_error_line("ERR -> [Socket::Socket]: Failed opening socket.\n",
		                 "> errno #",
		                 last_err,
		                 ": ",
		                 strerror(last_err));
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
	using TF::Debug::print_warning_line;

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
			int last_err = errno;
			print_warning_line("WRN -> [Socket::listen_method]: poll(...) "
			                   "returned non-fatal error.\n",
			                   "> errno #",
			                   last_err,
			                   ": ",
			                   strerror(last_err));
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
				print_warning_line("WRN -> [Socket::listen_method]: on_recieve "
				                   "handler not set.");
		}
	}
	close(socket->signal_fd);
}

void Socket::listen(size_t buffer_size) {
	using TF::Debug::print_debug_line;
	using TF::Debug::print_error_line;

	t_sockaddr_in address;
	bzero(&address, sizeof(t_sockaddr_in));

	address.sin_family      = AF_INET;
	address.sin_port        = htons(this->port_number);
	address.sin_addr.s_addr = INADDR_ANY;

	int b_result =
	    bind(this->socket_fd, (t_sockaddr*)&address, sizeof(address));

	if (b_result < 0) {
		int last_err = errno;
		print_error_line("ERR -> [Socket::listen]: Call to bind(...) failed.\n",
		                 "> errno #",
		                 last_err,
		                 ": ",
		                 strerror(last_err));
		return;
	}

	// Setup file descriptor to allow poll to catch signals.
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGUSR1);
	// Block default handling of signal for whole process.
	sigprocmask(SIG_BLOCK, &signal_mask, nullptr);

	this->signal_fd = signalfd(-1, &signal_mask, 0);

	if (this->signal_fd <= 0) {
		int last_err = errno;
		print_error_line(
		    "ERR -> [Socket::listen]: Creation of signalfd(...) failed.\n",
		    "> errno #",
		    last_err,
		    ": ",
		    strerror(last_err));
		return;
	}

	print_debug_line("DBG -> [Socket::listen]: Starting listener thread.");
	this->thread_continue = true;
	this->listener_thread =
	    std::thread(Socket::listen_method, this, buffer_size);
}

void Socket::stop() {
	using TF::Debug::print_debug_line;

	if (!this->listener_thread.joinable())
		return;

	this->thread_continue = false;
	print_debug_line("DBG -> [Socket::stop]: Signalling listener thread.");
	// Signal process with kill(...) instead of raise(...) - On Linux
	// raise(...)seems to only signal thread, whereas kill(..) will signal
	// the entire process and all threads in it.
	kill(getpid(), SIGUSR1);
	// Note: similar behavior might be observed with std::raise from <csignal>
	// header, but I have not yet tested it.
	this->listener_thread.join();

	print_debug_line("DBG -> [Socket::stop]: Done, exiting method.");
}

bool Socket::send_to(SocketAddress  address,
                     const uint8_t* msg_buffer,
                     size_t         msg_length) {

	using TF::Debug::print_error_line;

	auto addr_struct = address.get_struct();
	auto sent        = sendto(this->socket_fd,
                       msg_buffer,
                       msg_length,
                       0,
                       (t_sockaddr*)&addr_struct,
                       sizeof(addr_struct));

	if (sent < 0) {
		int last_error = errno;
		print_error_line("ERR -> [send_to]: Failed to send.\n",
		                 "> errno #",
		                 last_error,
		                 ": ",
		                 strerror(last_error));
		return false;
	}
	return true;
}

void Socket::set_on_recieve(const t_recv_handler& handler) {
	this->on_recieve = handler;
}
