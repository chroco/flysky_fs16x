/*
 * Copyright (c) 2020-2021 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
//#include <zephyr/ztest.h>

#include "flysky_fs16x.h"
#include "blinky.h"

int main()
{
	printf("flysky\n");
	
	FlySky flysky = FlySky();

//*
	while(1)
	{
		flysky.sampleFlysky();
		flysky.printPulse();
		//printf(".");

		k_msleep(50);
	}
//*/
}

//ZTEST_SUITE(pwm_loopback, NULL, pwm_loopback_setup, NULL, pwm_loopback_after, NULL);
