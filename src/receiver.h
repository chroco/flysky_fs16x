#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/timing/timing.h>
//#include <zephyr/ztest.h>

#define PWM_LOOPBACK_OUT_IDX 0
#define PWM_LOOPBACK_IN_IDX  1

#define PWM_LOOPBACK_NODE DT_INST(0, test_pwm_loopback)
#define RECEIVER_NODE	DT_ALIAS(receiver0)

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
//*
#define TEST_PWM_PERIOD_USEC      2000
#define TEST_PWM_PULSE_USEC       1000
//*/
/*
#define TEST_PWM_PERIOD_USEC    100000
#define TEST_PWM_PULSE_USEC      75000
//*/

#define RECEIVER_STACK_SIZE 500
#define RECEIVER_PRIORITY 5

#define LED0_NODE DT_ALIAS(led0)

struct pwm_t {
	const device *dev;
	uint32_t pwm;
	pwm_flags_t flags;
};

//class FlySky;

class Receiver
{
	public:
		Receiver(const gpio_dt_spec);
		~Receiver();

		//void setFlySky(FlySky *);

		void printPulse(void);
		static void receiver_entry(void *, void *, void *);
		int startReceiverThread(void);
	private:
		//FlySky *pFlySky;
		k_mutex time_mutex;
		static uint64_t pulse_time_us;
		uint64_t getPulseTime(void);
		void setPulseTime(uint64_t);
		
		const gpio_dt_spec receiver;
		static gpio_callback receiver_data;
		static void receiver_cb(const device *, gpio_callback *cb, uint32_t);

		static int work(void);
};

#endif
