#ifndef _FLYSKY_FS16X_H_
#define _FLYSKY_FS16X_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
//#include <zephyr/ztest.h>

#define PWM_LOOPBACK_OUT_IDX 0
#define PWM_LOOPBACK_IN_IDX  1

#define PWM_LOOPBACK_NODE DT_INST(0, test_pwm_loopback)

#define PWM_LOOPBACK_OUT_CTLR \
	DT_PWMS_CTLR_BY_IDX(PWM_LOOPBACK_NODE, PWM_LOOPBACK_OUT_IDX)
#define PWM_LOOPBACK_OUT_CHANNEL \
	DT_PWMS_CHANNEL_BY_IDX(PWM_LOOPBACK_NODE, PWM_LOOPBACK_OUT_IDX)
#define PWM_LOOPBACK_OUT_FLAGS \
	DT_PWMS_FLAGS_BY_IDX(PWM_LOOPBACK_NODE, PWM_LOOPBACK_OUT_IDX)

#define PWM_LOOPBACK_IN_CTLR \
	DT_PWMS_CTLR_BY_IDX(PWM_LOOPBACK_NODE, PWM_LOOPBACK_IN_IDX)
#define PWM_LOOPBACK_IN_CHANNEL \
	DT_PWMS_CHANNEL_BY_IDX(PWM_LOOPBACK_NODE, PWM_LOOPBACK_IN_IDX)
#define PWM_LOOPBACK_IN_FLAGS \
	DT_PWMS_FLAGS_BY_IDX(PWM_LOOPBACK_NODE, PWM_LOOPBACK_IN_IDX)

#define TEST_PWM_PERIOD_NSEC 100000000
#define TEST_PWM_PULSE_NSEC   15000000
//#define TEST_PWM_PERIOD_USEC      2500
//#define TEST_PWM_PULSE_USEC       1250
#define TEST_PWM_PERIOD_USEC    100000
#define TEST_PWM_PULSE_USEC      75000

enum test_pwm_unit {
	TEST_PWM_UNIT_NSEC,
	TEST_PWM_UNIT_USEC,
};

struct test_pwm {
	const struct device *dev;
	uint32_t pwm;
	pwm_flags_t flags;
};

struct pwm_t {
	const struct device *dev;
	uint32_t pwm;
	pwm_flags_t flags;
};

class Receiver
{
	public:
		Receiver();
		Receiver(const gpio_dt_spec);
		//Receiver(int);
		~Receiver();

		int getPulse(void);
		void printPulse(void);
	private:
		struct k_mutex time_mutex;
		pwm_t pwm_in;
		pwm_t pwm_out;
		static uint64_t pulse_time_us;
		uint64_t getPulseTime(void);
		void setPulseTime(uint64_t);
		
		const struct gpio_dt_spec receiver;
		//static const struct gpio_dt_spec receiver;
		static struct gpio_callback receiver_data;
		static void receiver_cb(const device *, gpio_callback *cb, uint32_t);
		
		void pulseCapture(uint32_t, uint32_t, enum test_pwm_unit, pwm_flags_t);
};

class FlySky {
	public:
		FlySky();
		~FlySky();

		int sampleFlysky(void);
		void printPulse(void);
	private:
		Receiver receiver;
		//pwm_t pwm_in;
		pwm_t pwm_out;
		uint32_t period;// = TEST_PWM_PERIOD_USEC;
		uint32_t pulse;// = TEST_PWM_PULSE_USEC;
	
		int startPwm(void);
		//static void receiver_cb(const device *, gpio_callback *cb, uint32_t);
		//void receiverInit(void);
		//int setupReceiver(const gpio_dt_spec *, gpio_callback *, gpio_callback_handler_t);
};

#endif
