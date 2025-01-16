/*
 * Copyright (c) 2020-2021 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "blinky.h"
#include "flysky_fs16x.h"

int main()
{
	printf("flysky\n");
	
	FlySky flysky = FlySky();

	for(uint64_t i = 0;;++i)
	{
		if(i % 20 == 0)
		{
			printf("\n*%llu*", i/20);
		}
		
		flysky.printPulse();

		k_msleep(50);
	}

	return 0;
}
