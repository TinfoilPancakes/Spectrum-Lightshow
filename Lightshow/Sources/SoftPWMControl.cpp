/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   SoftPWMControl.cpp                            <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/05 07:41:31 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/17 10:41:59 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "SoftPWMControl.hpp"
#include "DebugTools.hpp"

using namespace Lightshow;
using namespace TF::Debug;

void SoftPWMControl::execute(SoftPWMControl* controller) {

	while (controller->to_continue) {

		float duty_cycle = controller->duty_cycle.load();
		Nanos period     = controller->period.load();

		if (controller->duty_cycle != 0)
			controller->pin.set_pin_value(GPIO_ON);

		std::this_thread::sleep_for(period * duty_cycle);

		controller->pin.set_pin_value(GPIO_OFF);

		std::this_thread::sleep_for(period - (period * duty_cycle));
	}
}

void SoftPWMControl::launch_thread() {
	if (this->to_continue)
		print_warning_line(
		    "WRN -> [SoftPWMControl::launch_thread]: Thread already started.");
	this->to_continue        = true;
	this->pin_control_thread = std::thread(SoftPWMControl::execute, this);
}

void SoftPWMControl::join_thread() {
	this->to_continue = false;
	this->pin_control_thread.join();
}

SoftPWMControl::SoftPWMControl(GPIOInterface& pin) : pin(pin) {
	this->period      = std::chrono::milliseconds(8);
	this->to_continue = false;
	this->duty_cycle  = 0;
}

SoftPWMControl::SoftPWMControl(GPIOInterface& pin, const float duty_cycle)
    : SoftPWMControl(pin) {
	this->duty_cycle = duty_cycle;
}

SoftPWMControl::SoftPWMControl(GPIOInterface& pin,
                               const float    duty_cycle,
                               const Nanos    period)
    : SoftPWMControl(pin, duty_cycle) {
	this->period = period;
}

SoftPWMControl::~SoftPWMControl() {
	if (this->to_continue)
		this->join_thread();
}
