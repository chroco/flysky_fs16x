# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(flysky_fs16x_rc)

#FILE(GLOB app_sources src/*.c)

#target_sources(app PRIVATE ${app_sources})

target_sources(app 
	PRIVATE 
	#src/test_pwm_loopback.c
		src/blinky.c
		src/receiver.cpp
		src/flysky_fs16x.cpp
		src/main.cpp
)
