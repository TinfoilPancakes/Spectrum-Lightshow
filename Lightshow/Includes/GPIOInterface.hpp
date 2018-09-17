/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   GPIOInterface.hpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2016/05/19 13:12:39 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/17 11:28:17 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef GPIOINTERFACE_HPP
#define GPIOINTERFACE_HPP

#include <fstream>
#include <string>

#define GPIO_SYSTEM_DIR "/sys/class/gpio/"

#define GPIO_DIR_OUT 1
#define GPIO_DIR_IN 0
#define GPIO_ON 1
#define GPIO_OFF 0

namespace Lightshow {

class GPIOInterface {
protected:
	std::string   pin_id;
	std::fstream* pin_io_stream = nullptr;

public:
	GPIOInterface(const std::string& pin_id, int8_t direction);

	GPIOInterface(const std::string& pin_id);

	~GPIOInterface();

	inline const std::string& get_pin_id() { return this->pin_id; }

	bool enable();

	bool disable();

	bool destroy_io_stream();

	bool init_io_stream();

	bool set_direction(int8_t direction);

	int get_pin_value();

	bool set_pin_value(int value);
};
} // namespace Lightshow
#endif // GPIOINTERFACE_HPP
