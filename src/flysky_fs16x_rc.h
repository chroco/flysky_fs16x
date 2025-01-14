#ifndef _FLYSKY_FS16X_RC_H_
#define _FLYSKY_FS16X_RC_H_

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

struct test_pwm {
	const struct device *dev;
	uint32_t pwm;
	pwm_flags_t flags;
};

struct test_pwm_callback_data {
	uint32_t *buffer;
	size_t buffer_len;
	size_t count;
	int status;
	struct k_sem sem;
	bool pulse_capture;
};

void get_test_pwms(struct test_pwm *out, struct test_pwm *in);
int sample_flysky(void);
void receiverInit(void);
void printPulse(void);
void gpioInit(void);
#endif
