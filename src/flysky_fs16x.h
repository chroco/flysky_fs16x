#ifndef _FLYSKY_FS16X_H_
#define _FLYSKY_FS16X_H_

#include "receiver.h"

class FlySky {
	public:
		FlySky();
		~FlySky();

		int sampleFlysky(void);
		void printPulse(void);
	private:
		Receiver receiver;
		pwm_t pwm_out;
		uint32_t period;// = TEST_PWM_PERIOD_USEC;
		uint32_t pulse;// = TEST_PWM_PULSE_USEC;
	
		int startPwm(void);
};

#endif
