/*
 * Copyright (c) 2020-2021 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "flysky_fs16x_rc.h"
#include <string.h>
//#include <zephyr/kernel.h>
//#include <zephyr/drivers/gpio.h>
//#include <zephyr/drivers/pwm.h>
//#include <zephyr/ztest.h>


#define TEST_PWM_PERIOD_NSEC 100000000
#define TEST_PWM_PULSE_NSEC   15000000
#define TEST_PWM_PERIOD_USEC    100000
#define TEST_PWM_PULSE_USEC      75000

#define RECEIVER_NODE	DT_ALIAS(receiver0) //

static struct gpio_callback receiver_data = {0};
static uint64_t time_us = 0; 
static const struct gpio_dt_spec receiver = GPIO_DT_SPEC_GET_OR(RECEIVER_NODE, gpios, {0});

K_MUTEX_DEFINE(time_mutex);

enum test_pwm_unit {
	TEST_PWM_UNIT_NSEC,
	TEST_PWM_UNIT_USEC,
};

void get_test_pwms(struct test_pwm *out, struct test_pwm *in)
{
	/* PWM generator device */
	out->dev = DEVICE_DT_GET(PWM_LOOPBACK_OUT_CTLR);
	out->pwm = PWM_LOOPBACK_OUT_CHANNEL;
	out->flags = PWM_LOOPBACK_OUT_FLAGS;
//	zassert_true(device_is_ready(out->dev), "pwm loopback output device is not ready");

	/* PWM capture device */
	in->dev = DEVICE_DT_GET(PWM_LOOPBACK_IN_CTLR);
	in->pwm = PWM_LOOPBACK_IN_CHANNEL;
	in->flags = PWM_LOOPBACK_IN_FLAGS;
//	zassert_true(device_is_ready(in->dev), "pwm loopback input device is not ready");
}

static void capture(uint32_t period, uint32_t pulse, enum test_pwm_unit unit, pwm_flags_t flags)
{
	struct test_pwm in;
	struct test_pwm out;
	uint64_t period_capture = 0;
	uint64_t pulse_capture = 0;
	int err = 0;

	get_test_pwms(&out, &in);

	err = pwm_set(out.dev, out.pwm, 
					PWM_USEC(period), PWM_USEC(pulse), 
					out.flags ^= (flags & PWM_POLARITY_MASK));
	
	err = pwm_capture_usec(in.dev, in.pwm, flags, &period_capture,
						 &pulse_capture, K_USEC(period * 10));

	pwm_disable_capture(in.dev, in.pwm);

	if (err == -ENOTSUP) {
//		TC_PRINT("capture type not supported\n");
//		ztest_test_skip();
	}

//	zassert_equal(err, 0, "failed to capture pwm (err %d)", err);

	if (flags & PWM_CAPTURE_TYPE_PERIOD) {
//		zassert_within(period_capture, period, period / 100,
//			       "period capture off by more than 1%");
	}

	if (flags & PWM_CAPTURE_TYPE_PULSE) {
//		zassert_within(pulse_capture, pulse, pulse / 100,
//			       "pulse capture off by more than 1%");
	}
	printf("(%llu) ", pulse_capture);
	//time_us = pulse_capture; 
}
//*/

void printPulse(void)
{
	k_mutex_lock(&time_mutex, K_FOREVER);
	printf("(%llu) ", time_us);
	k_mutex_unlock(&time_mutex);
}

//*
int sample_flysky(void)
{
	capture(TEST_PWM_PERIOD_USEC, TEST_PWM_PULSE_USEC,
		     TEST_PWM_UNIT_USEC, PWM_CAPTURE_TYPE_PULSE | PWM_POLARITY_NORMAL);
	return 0;
}
//*/

static struct test_context {
	uint32_t last_edge_time;
	uint32_t high_time;
	uint32_t low_time;
	bool sampling_done;
	uint8_t skip_cnt;
} ctx;

static void setup_edge_detect(void)
{
	ctx.last_edge_time = 0;
	ctx.high_time = 0;
	ctx.low_time = 0;
	ctx.sampling_done = false;
	ctx.skip_cnt = 0;
}

//*
static void receiver_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
//	printf(".");
///*
	uint32_t start_time = k_cycle_get_32();
	
	while(gpio_pin_get_dt(&receiver) > 0)
	{

	}

	uint32_t end_time = k_cycle_get_32();
	time_us = end_time - start_time;
//*/
}
//*/

/*
static void receiver_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
//	printf(".");
//
//capture(TEST_PWM_PERIOD_USEC, TEST_PWM_PULSE_USEC,
//	     TEST_PWM_UNIT_USEC, PWM_CAPTURE_TYPE_PULSE | PWM_POLARITY_NORMAL);
//
	int pin_state;
	uint32_t current_time = k_cycle_get_32();

	//if (ctx.sampling_done || ++ctx.skip_cnt < CONFIG_SKIP_EDGE_NUM) {
	if (ctx.sampling_done || ++ctx.skip_cnt < 2) {
		return;
	}

	if (!ctx.last_edge_time) {
		// init last_edge_time for first delta 
		ctx.last_edge_time = current_time;
		return;
	}

	uint32_t elapsed_time = current_time - ctx.last_edge_time;

	int pin = __builtin_ffs(pins) - 1;

	if (pin >= 0) {
		pin_state = gpio_pin_get(dev, pin);
	} else {
		return;
	}

	if (pin_state) {
		ctx.low_time = elapsed_time;
	} else {
		ctx.high_time = elapsed_time;
		time_us = ctx.high_time; 
	}

	// sampling is done when both high and low times were stored 
	if (ctx.high_time && ctx.low_time) {
		ctx.sampling_done = true;
	}

	ctx.last_edge_time = current_time;
	
}
//*/

static void config_gpio(const struct gpio_dt_spec *gpio_dt)
{
	/* Configure GPIO pin for edge detection */
	gpio_pin_configure_dt(gpio_dt, GPIO_INPUT);

	receiver_data.pin_mask = BIT(gpio_dt->pin);

	gpio_init_callback(&receiver_data, receiver_cb, receiver_data.pin_mask);
	gpio_add_callback(gpio_dt->port, &receiver_data);
	gpio_pin_interrupt_configure(gpio_dt->port, gpio_dt->pin, GPIO_INT_EDGE_RISING);
//	gpio_pin_interrupt_configure(gpio_dt->port, gpio_dt->pin, GPIO_INT_EDGE_BOTH);
}

void gpioInit(void)
{
	config_gpio(&receiver);
	setup_edge_detect();

	/* wait for sampling */
	k_sleep(K_MSEC(2000));
}

//*
int setupReceiver(
		const struct gpio_dt_spec *preceiver, 
		struct gpio_callback *preceiver_data, 
		gpio_callback_handler_t receiver_cb
	)
{
	int ret;
	
	while (!gpio_is_ready_dt(preceiver)) {
		printk("Error: device %s is not ready\n", preceiver->port->name);
		k_msleep(1000);
	}

	ret = gpio_pin_configure_dt(preceiver, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", 
						ret, preceiver->port->name, preceiver->pin
		);
	}

	ret = gpio_pin_interrupt_configure_dt(preceiver, GPIO_INT_EDGE_BOTH);
	//ret = gpio_pin_interrupt_configure_dt(preceiver, GPIO_INT_EDGE_RISING);
	//ret = gpio_pin_interrupt_configure_dt(preceiver, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt %s pin %d\n", 
						ret, preceiver->port->name, preceiver->pin);
	}
	
	gpio_init_callback(preceiver_data, receiver_cb, BIT(preceiver->pin));
	gpio_add_callback(preceiver->port, preceiver_data);

	return 0;
}
//*/

//*
void receiverInit()
{
	setupReceiver(&receiver, &receiver_data, receiver_cb);
}
//*/

/*
static void test_capture(uint32_t period, uint32_t pulse, enum test_pwm_unit unit,
		  pwm_flags_t flags)
{
	struct test_pwm in;
	struct test_pwm out;
	uint64_t period_capture = 0;
	uint64_t pulse_capture = 0;
	int err = 0;

	get_test_pwms(&out, &in);

	switch (unit) {
	case TEST_PWM_UNIT_NSEC:
//	TC_PRINT("Testing PWM capture @ %u/%u nsec\n",
//		 pulse, period);
		err = pwm_set(out.dev, out.pwm, period, pulse, out.flags ^=
			      (flags & PWM_POLARITY_MASK));
		break;

	case TEST_PWM_UNIT_USEC:
//	TC_PRINT("Testing PWM capture @ %u/%u usec\n",
//		 pulse, period);
		err = pwm_set(out.dev, out.pwm, PWM_USEC(period),
			      PWM_USEC(pulse), out.flags ^=
			      (flags & PWM_POLARITY_MASK));
		break;

	default:
//	TC_PRINT("Unsupported test unit");
//	ztest_test_fail();
	}

//	zassert_equal(err, 0, "failed to set pwm output (err %d)", err);

	switch (unit) {
	case TEST_PWM_UNIT_NSEC:
		err = pwm_capture_nsec(in.dev, in.pwm, flags, &period_capture,
				       &pulse_capture, K_NSEC(period * 10));
		break;

	case TEST_PWM_UNIT_USEC:
		err = pwm_capture_usec(in.dev, in.pwm, flags, &period_capture,
				       &pulse_capture, K_USEC(period * 10));
		break;

	default:
//	TC_PRINT("Unsupported test unit");
//	ztest_test_fail();
	}

	pwm_disable_capture(in.dev, in.pwm);

	if (err == -ENOTSUP) {
//		TC_PRINT("capture type not supported\n");
//		ztest_test_skip();
	}

//	zassert_equal(err, 0, "failed to capture pwm (err %d)", err);

	if (flags & PWM_CAPTURE_TYPE_PERIOD) {
//		zassert_within(period_capture, period, period / 100,
//			       "period capture off by more than 1%");
	}

	if (flags & PWM_CAPTURE_TYPE_PULSE) {
//		zassert_within(pulse_capture, pulse, pulse / 100,
//			       "pulse capture off by more than 1%");
	}
	printf("(%llu) ", pulse_capture);
}
//*/

/*
static void continuous_capture_callback(const struct device *dev,
					uint32_t pwm,
					uint32_t period_cycles,
					uint32_t pulse_cycles,
					int status,
					void *user_data)
{
	struct test_pwm_callback_data *data = user_data;

	if (data->count > data->buffer_len) {
		// Safe guard in case capture is not disabled 
		return;
	}

	if (status != 0) {
		// Error occurred 
		data->status = status;
		k_sem_give(&data->sem);
	}

	if (data->pulse_capture) {
		data->buffer[data->count++] = pulse_cycles;
	} else {
		data->buffer[data->count++] = period_cycles;
	}

	if (data->count > data->buffer_len) {
		data->status = 0;
		k_sem_give(&data->sem);
	}
}
//*/

//static void test_capture(uint32_t period, uint32_t pulse, enum test_pwm_unit unit,
//		  pwm_flags_t flags)

/*
int sample_flysky(void)
{
	struct test_pwm in;
	struct test_pwm out;
	uint64_t period_capture = 0;
	uint64_t pulse_capture = 0;
	int err = 0;
	uint32_t period = TEST_PWM_PERIOD_NSEC;
	uint32_t pulse = TEST_PWM_PULSE_NSEC;

	pwm_flags_t flags = PWM_CAPTURE_TYPE_PULSE | PWM_POLARITY_NORMAL;

	get_test_pwms(&out, &in);

	err = pwm_set(out.dev, out.pwm, PWM_USEC(period),
			      PWM_USEC(pulse), out.flags ^=
			      (flags & PWM_POLARITY_MASK));

	err = pwm_capture_usec(in.dev, in.pwm, flags, &period_capture,
					 &pulse_capture, K_USEC(period * 10));
	
	pwm_disable_capture(in.dev, in.pwm);

	printf("(%llu) ", pulse_capture);
	
	return 0;
}
//*/

/*
int sample_flysky(void)
{
	struct test_pwm in;
	struct test_pwm out;
	uint32_t buffer[10];
	struct test_pwm_callback_data data = {
		.buffer = buffer,
		.buffer_len = ARRAY_SIZE(buffer),
		.count = 0,
		.pulse_capture = true,
	};
	uint64_t usec = 0;
	int err;
	int i;

	get_test_pwms(&out, &in);

	memset(buffer, 0, sizeof(buffer));
	k_sem_init(&data.sem, 0, 1);

	err = pwm_set(out.dev, out.pwm, PWM_USEC(TEST_PWM_PERIOD_USEC),
		      PWM_USEC(TEST_PWM_PULSE_USEC), out.flags);
//	zassert_equal(err, 0, "failed to set pwm output (err %d)", err);

	err = pwm_configure_capture(in.dev, in.pwm,
				    in.flags |
				    PWM_CAPTURE_MODE_CONTINUOUS |
				    PWM_CAPTURE_TYPE_PULSE,
				    continuous_capture_callback, &data);
//	while(1)
//	{
		printf("\n");
		err = pwm_enable_capture(in.dev, in.pwm);
//		zassert_equal(err, 0, "failed to enable pwm capture (err %d)", err);

		err = k_sem_take(&data.sem, K_USEC(TEST_PWM_PERIOD_USEC * data.buffer_len * 10));
//		zassert_equal(err, 0, "pwm capture timed out (err %d)", err);
//		zassert_equal(data.status, 0, "pwm capture failed (err %d)", err);

		err = pwm_disable_capture(in.dev, in.pwm);
//		zassert_equal(err, 0, "failed to disable pwm capture (err %d)", err);

		for (i = 0; i < data.buffer_len; i++) {
			err = pwm_cycles_to_usec(in.dev, in.pwm, buffer[i], &usec);
//			zassert_equal(err, 0, "failed to calculate usec (err %d)", err);
			printf("%llu ", usec);
		}
//	}
	
	printf("\n");
}
//*/


