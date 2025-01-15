/*
 * Copyright (c) 2020-2021 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "flysky_fs16x.h"
#include <string.h>

FlySky::FlySky() :
	receiver(
		Receiver(GPIO_DT_SPEC_GET_OR(RECEIVER_NODE, gpios, {0}))
	),
	pwm_out{
		DEVICE_DT_GET(PWM_LOOPBACK_OUT_CTLR),
		PWM_LOOPBACK_OUT_CHANNEL,
		PWM_LOOPBACK_OUT_FLAGS
	}
{
	//receiver.setFlySky(this); // 0_o
	
	//startPwm();
	
	receiver.startReceiverThread();
}

FlySky::~FlySky()
{

}

int FlySky::startPwm(void)
{
	uint32_t period = TEST_PWM_PERIOD_USEC;
	uint32_t pulse = TEST_PWM_PULSE_USEC;
	pwm_flags_t flags = PWM_CAPTURE_TYPE_PULSE | PWM_POLARITY_NORMAL;

	int err = pwm_set(
				pwm_out.dev, pwm_out.pwm, 
				PWM_USEC(period), 
				PWM_USEC(pulse), 
				pwm_out.flags ^= (flags & PWM_POLARITY_MASK)
			);

	return err;
}

int FlySky::sampleFlysky(void)
{

	return 0;
}

void FlySky::printPulse(void)
{
	receiver.printPulse();
}

