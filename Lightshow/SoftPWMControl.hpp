/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   SoftPWMControl.hpp                            <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2016/05/21 13:12:39 by prp              2E 54 65 63 68          */
/*   Updated: 2017/09/24 13:30:50 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOFTPWMCONTROL_HPP
#define SOFTPWMCONTROL_HPP

#include <chrono>
#include <cmath>
#include <mutex>
#include <thread>

#include "GPIOInterface.hpp"

class SoftPWMControl {
protected:
	std::mutex  lock;
	std::thread pinControlThread;

	GPIOInterface* pin = nullptr;

	std::chrono::duration<unsigned long long, std::nano> period =
		std::chrono::milliseconds(8); // Defaults to 8ms period time.
	bool  toContinue = true;
	float dutyCycle  = 0; // Defaults to fully off. ranges from 0.0 - 1.0

	static void Execute(SoftPWMControl* controller, std::mutex* m) {
		bool shouldContinue = controller->toContinue;
		while (shouldContinue) {
			m->lock();
			controller->pin->setPinValue(short(
				GPIO_PIN_ON *
				(controller->dutyCycle !=
				 0))); // The ceil function is to keep full off at 0% duty.
			m->unlock();
			std::this_thread::sleep_for(controller->period *
										controller->dutyCycle);
			m->lock();
			controller->pin->setPinValue(GPIO_PIN_OFF);
			m->unlock();
			std::this_thread::sleep_for(
				controller->period -
				(controller->period * controller->dutyCycle));
			m->lock();
			shouldContinue = controller->toContinue;
			m->unlock();
		}
	}

	void launchThread() {
		this->pinControlThread =
			std::thread(SoftPWMControl::Execute, this, &this->lock);
	}

	void joinThread() {
		this->lock.lock();
		this->toContinue = false;
		this->lock.unlock();
		this->pinControlThread.join();
	}

public:
	SoftPWMControl(GPIOInterface* pin) { this->pin = pin; }

	SoftPWMControl(GPIOInterface* pin, const float& dutyCycle)
		: SoftPWMControl(pin) {
		this->dutyCycle = dutyCycle;
	}

	SoftPWMControl(
		GPIOInterface* pin,
		const float&   dutyCycle,
		const std::chrono::duration<unsigned long long, std::nano>& period)
		: SoftPWMControl(pin, dutyCycle) {
		this->period = period;
	}

	void init() { this->launchThread(); }

	void stop() { this->joinThread(); }

	void setDutyCycle(const float& dutyCycle) {
		this->lock.lock();
		this->dutyCycle = dutyCycle;
		this->lock.unlock();
	}

	void setPeriodLength(const std::chrono::nanoseconds& period) {
		this->lock.lock();
		this->period = period;
		this->lock.unlock();
	}

	float getDutyCycle() { return this->dutyCycle; }
};

#endif // SOFTPWMCONTROL_HPP
