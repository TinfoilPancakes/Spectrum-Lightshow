/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   GPIOInterface.hpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2016/05/19 13:12:39 by prp              2E 54 65 63 68          */
/*   Updated: 2018/08/02 19:48:39 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef GPIOINTERFACE_HPP
#define GPIOINTERFACE_HPP

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#define GPIO_SYSTEM_DIR "/sys/class/gpio/"

#define GPIO_DIR_OUT 1
#define GPIO_DIR_IN 0
#define GPIO_ON 1
#define GPIO_OFF 0

class GPIOInterface {
protected:
	std::string   pin_id;
	std::fstream* pin_io_stream = nullptr;

public:
	GPIOInterface(const std::string& pin_id, int8_t direction) {
		this->pin_id = pin_id;
		this->enable();
		std::this_thread::sleep_for(std::chrono::milliseconds(
			50)); // Delay for sysfs to finish setting up GPIO pin...
		this->set_direction(direction);
		this->init_io_stream();
	}

	GPIOInterface(const std::string& pin_id)
		: GPIOInterface(pin_id, GPIO_DIR_IN) {
		// Just add default value for GPIO direction to in.
	}

	~GPIOInterface() {
		this->destroy_io_stream();
		this->disable();
	}

	const std::string& get_pin_id() { return this->pin_id; }

	bool enable() {
		std::string   path = GPIO_SYSTEM_DIR "export";
		std::ofstream export_stream(path.c_str());
		std::cout << "Enabling GPIO pin #" << this->pin_id << "..."
				  << std::endl;
		if (!export_stream.is_open()) {
			std::cout << "ERROR: Failed to enable GPIO pin #" << this->pin_id
					  << std::endl;
			return false;
		}
		export_stream << this->pin_id;
		export_stream.close();
		std::cout << "Done." << std::endl;
		return true;
	}

	bool disable() {
		std::string   path = GPIO_SYSTEM_DIR "unexport";
		std::ofstream unexport_stream(path.c_str());
		std::cout << "Disabling GPIO pin #" << this->pin_id << "..."
				  << std::endl;
		if (!unexport_stream.is_open()) {
			std::cout << "ERROR: Failed to disable GPIO pin #" << this->pin_id
					  << std::endl;
			return false;
		}
		unexport_stream << this->pin_id;
		unexport_stream.close();
		std::cout << "Done." << std::endl;
		return true;
	}

	bool destroy_io_stream() {
		std::cout << "Closing I/O streams for GPIO pin #" << this->pin_id
				  << "..." << std::endl;
		if (this->pin_io_stream) {
			this->pin_io_stream->close();
			delete this->pin_io_stream;
			this->pin_io_stream = nullptr;
		}
		std::cout << "Done." << std::endl;
		return true;
	}

	bool init_io_stream() {
		std::string path = GPIO_SYSTEM_DIR "gpio" + this->pin_id + "/value";
		std::cout << "Initializing I/O streams for GPIO pin #" << this->pin_id
				  << "..." << std::endl;
		this->pin_io_stream =
			new std::fstream(path.c_str(), std::ios::out | std::ios::in);
		if (!this->pin_io_stream || !this->pin_io_stream->is_open()) {
			std::cout << "ERROR: Failed to initialize I/O stream for GPIO pin #"
					  << this->pin_id << std::endl;
			return false;
		}
		std::cout << "Done." << std::endl;
		return true;
	}

	bool set_direction(int8_t direction) {
		std::string path = GPIO_SYSTEM_DIR "gpio" + this->pin_id + "/direction";
		std::ofstream direction_stream(path.c_str());

		std::cout << "Setting GPIO pin #" << this->pin_id << " direction..."
				  << std::endl;
		if (!direction_stream.is_open()) {
			std::cout << "ERROR: Failed to set direction of GPIO pin #"
					  << this->pin_id << std::endl;
			return false;
		}
		if (direction == GPIO_DIR_IN) {
			direction_stream << "in";
		} else {
			direction_stream << "out";
		}
		direction_stream.close();
		std::cout << "Done." << std::endl;
		return true;
	}

	int8_t get_pin_value() {
		if (!this->pin_io_stream) {
			return -1;
		}
		int8_t pinValue;
		*(this->pin_io_stream) >> pinValue;
		this->pin_io_stream->flush();
		return pinValue;
	}

	bool set_pin_value(int8_t value) {
		if (!this->pin_io_stream) {
			return false;
		}
		*(this->pin_io_stream) << value;
		this->pin_io_stream->flush();
		return true;
	}
};

#endif // GPIOINTERFACE_HPP
