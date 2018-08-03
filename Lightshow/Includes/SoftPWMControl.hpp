/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   SoftPWMControl.hpp                            <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2016/05/21 13:12:39 by prp              2E 54 65 63 68          */
/*   Updated: 2018/08/02 19:52:11 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOFTPWMCONTROL_HPP
#define SOFTPWMCONTROL_HPP

#include <atomic>
#include <chrono>
#include <cmath>
#include <mutex>
#include <thread>

#include "GPIOInterface.hpp"

class SoftPWMControl {
	using Nanos = std::chrono::nanoseconds;

protected:
	std::thread		 pin_control_thread;
	std::atomic_bool to_continue;

	GPIOInterface& pin;
	// Defaults to 8ms period time.
	std::atomic<Nanos> period;
	// Defaults to fully off. Values range from 0.0 - 1.0
	std::atomic<float> duty_cycle;

	static void execute(SoftPWMControl* controller) {
		while (controller->to_continue) {
			float duty_cycle = controller->duty_cycle.load();
			Nanos period	 = controller->period.load();

			if (controller->duty_cycle != 0)
				controller->pin.set_pin_value(GPIO_ON);

			std::this_thread::sleep_for(period * duty_cycle);

			controller->pin.set_pin_value(GPIO_OFF);

			std::this_thread::sleep_for(period - (period * duty_cycle));
		}
	}

	void launch_thread() {
		if (this->to_continue)
			std::cerr << "Warning: Thread Already Started..." << std::endl;
		this->to_continue		 = true;
		this->pin_control_thread = std::thread(SoftPWMControl::execute, this);
	}

	void join_thread() {
		this->to_continue = false;
		this->pin_control_thread.join();
	}

public:
	SoftPWMControl(GPIOInterface& pin) : pin(pin) {
		this->period	  = std::chrono::milliseconds(8);
		this->to_continue = false;
		this->duty_cycle  = 0;
	}

	SoftPWMControl(GPIOInterface& pin, const float duty_cycle)
		: SoftPWMControl(pin) {
		this->duty_cycle = duty_cycle;
	}

	SoftPWMControl(GPIOInterface&				  pin,
				   const float					  duty_cycle,
				   const std::chrono::nanoseconds period)
		: SoftPWMControl(pin, duty_cycle) {
		this->period = period;
	}

	~SoftPWMControl() {
		if (this->to_continue)
			this->join_thread();
	}

	void start() { this->launch_thread(); }

	void stop() { this->join_thread(); }

	void set_duty_cycle(const float& duty_cycle) {
		this->duty_cycle = duty_cycle;
	}

	void set_period_length(const std::chrono::nanoseconds& period) {
		this->period = period;
	}

	float get_duty_cycle() { return this->duty_cycle; }
};

#endif // SOFTPWMCONTROL_HPP
