/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   SoftPWMControl.hpp                            <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2016/05/21 13:12:39 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/09 08:38:25 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOFTPWMCONTROL_HPP
#define SOFTPWMCONTROL_HPP

#include <atomic>
#include <chrono>
#include <thread>

#include "GPIOInterface.hpp"

namespace Lightshow {

class SoftPWMControl {
	using Nanos = std::chrono::nanoseconds;

protected:
	std::thread      pin_control_thread;
	std::atomic_bool to_continue;

	GPIOInterface& pin;
	// Defaults to 8ms period time.
	std::atomic<Nanos> period;
	// Defaults to fully off. Values range from 0.0 - 1.0
	std::atomic<float> duty_cycle;

	static void execute(SoftPWMControl* controller);

	void launch_thread();

	void join_thread();

public:
	SoftPWMControl(GPIOInterface& pin);

	SoftPWMControl(GPIOInterface& pin, const float duty_cycle);

	SoftPWMControl(GPIOInterface& pin,
	               const float    duty_cycle,
	               const Nanos    period);

	~SoftPWMControl();

	inline void start() { this->launch_thread(); }

	inline void stop() { this->join_thread(); }

	inline void set_duty_cycle(const float& duty_cycle) {
		this->duty_cycle = duty_cycle;
	}

	inline void set_period_length(const Nanos& period) {
		this->period = period;
	}

	inline float get_duty_cycle() { return this->duty_cycle; }
};
} // namespace Lightshow
#endif // SOFTPWMCONTROL_HPP
