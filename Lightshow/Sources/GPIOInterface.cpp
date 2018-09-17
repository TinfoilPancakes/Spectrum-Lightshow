/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   GPIOInterface.cpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/05 10:01:30 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/17 08:36:34 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "GPIOInterface.hpp"
#include "DebugTools.hpp"

#include <chrono>
#include <thread>

using namespace Lightshow;
using namespace TF::Debug;

GPIOInterface::GPIOInterface(const std::string& pin_id, int8_t direction) {
	this->pin_id = pin_id;

	this->enable();
	// Delay for sysfs to finish setting up GPIO pin...
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	this->set_direction(direction);

	this->init_io_stream();
}

GPIOInterface::GPIOInterface(const std::string& pin_id)
    : GPIOInterface(pin_id, GPIO_DIR_IN) {
	// Just add default value for GPIO direction to in.
}

GPIOInterface::~GPIOInterface() {
	this->destroy_io_stream();
	this->disable();
}

bool GPIOInterface::enable() {
	std::string   path = GPIO_SYSTEM_DIR "export";
	std::ofstream export_stream(path.c_str());

	print_debug_line("DBG -> [GPIOInterface::enable]: Enabling pin #",
	                 this->pin_id);

	if (!export_stream.is_open()) {
		print_error_line(
		    "ERR -> [GPIOInterface::enable]: Failed to enable pin #",
		    this->pin_id);
		return false;
	}

	export_stream << this->pin_id;
	export_stream.flush();
	export_stream.close();

	return true;
}

bool GPIOInterface::disable() {
	std::string   path = GPIO_SYSTEM_DIR "unexport";
	std::ofstream unexport_stream(path.c_str());

	print_debug_line("DBG -> [GPIOInterface::disable]: Disabling pin #",
	                 this->pin_id);

	if (!unexport_stream.is_open()) {
		print_error_line(
		    "ERR -> [GPIOInterface::disable]: Failed to disable pin #",
		    this->pin_id);
		return false;
	}

	unexport_stream << this->pin_id;
	unexport_stream.flush();
	unexport_stream.close();

	return true;
}

bool GPIOInterface::destroy_io_stream() {

	print_debug_line("DBG -> [GPIOInterface::destroy_io_stream]: ",
	                 "Closing I/O streams for pin #",
	                 this->pin_id);

	if (this->pin_io_stream) {
		this->pin_io_stream->close();
		delete this->pin_io_stream;
		this->pin_io_stream = nullptr;
	}

	return true;
}

bool GPIOInterface::init_io_stream() {
	std::string path = GPIO_SYSTEM_DIR "gpio" + this->pin_id + "/value";

	print_debug_line("DBG -> [GPIOInterface::init_io_stream]: ",
	                 "Initializing I/O streams for pin #",
	                 this->pin_id);

	this->pin_io_stream =
	    new std::fstream(path.c_str(), std::ios::out | std::ios::in);

	if (!this->pin_io_stream || !this->pin_io_stream->is_open()) {
		print_error_line("ERR -> [GPIOInterface::init_io_stream]: ",
		                 "Failed to initialize I/O streams for pin #",
		                 this->pin_id);
		return false;
	}
	return true;
}

bool GPIOInterface::set_direction(int8_t direction) {
	std::string   path = GPIO_SYSTEM_DIR "gpio" + this->pin_id + "/direction";
	std::ofstream direction_stream(path.c_str());

	print_debug_line("DBG -> [GPIOInterface::set_direction]: ",
	                 "Setting direction for pin #",
	                 this->pin_id);

	if (!direction_stream.is_open()) {
		print_error_line("ERR -> [GPIOInterface::set_direction]: ",
		                 "Failed to set direction for pin #",
		                 this->pin_id);
		return false;
	}

	if (direction == GPIO_DIR_IN) {
		direction_stream << "in";
	} else {
		direction_stream << "out";
	}
	direction_stream.flush();
	direction_stream.close();

	return true;
}

int8_t GPIOInterface::get_pin_value() {
	if (!this->pin_io_stream) {
		return -1;
	}

	int8_t pinValue;

	*(this->pin_io_stream) >> pinValue;
	this->pin_io_stream->flush();

	return pinValue;
}

bool GPIOInterface::set_pin_value(int8_t value) {
	if (!this->pin_io_stream) {
		return false;
	}

	*(this->pin_io_stream) << value;
	this->pin_io_stream->flush();

	return true;
}
