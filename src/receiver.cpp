#include "receiver.h"


// Reciever

struct gpio_callback Receiver::receiver_data = {0};

uint64_t Receiver::pulse_time_us = 0;

//*
Receiver::Receiver(const gpio_dt_spec receiver) :
	pwm_in{
		DEVICE_DT_GET(PWM_LOOPBACK_IN_CTLR),
		PWM_LOOPBACK_IN_CHANNEL,
		PWM_LOOPBACK_IN_FLAGS
	},
	receiver(receiver)
{
	k_mutex_init(&time_mutex);

	int ret = 0;

	while (!gpio_is_ready_dt(&receiver)) {
		printk("Error: device %s is not ready\n", receiver.port->name);
		k_msleep(1000);
	}

	ret = gpio_pin_configure_dt(&receiver, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", 
						ret, receiver.port->name, receiver.pin
		);
	}

	//ret = gpio_pin_interrupt_configure_dt(&receiver, GPIO_INT_EDGE_BOTH);
	ret = gpio_pin_interrupt_configure_dt(&receiver, GPIO_INT_EDGE_RISING);
	//ret = gpio_pin_interrupt_configure_dt(preceiver, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt %s pin %d\n", 
						ret, receiver.port->name, receiver.pin);
	}
	
	gpio_init_callback(&receiver_data, receiver_cb, BIT(receiver.pin));
	gpio_add_callback(receiver.port, &receiver_data);
}
//*/

//*
Receiver::Receiver() :
	pwm_in{
		DEVICE_DT_GET(PWM_LOOPBACK_IN_CTLR),
		PWM_LOOPBACK_IN_CHANNEL,
		PWM_LOOPBACK_IN_FLAGS
	},
	receiver(GPIO_DT_SPEC_GET_OR(RECEIVER_NODE, gpios, {0}))
{
	k_mutex_init(&time_mutex);

	int ret = 0;

	while (!gpio_is_ready_dt(&receiver)) {
		printk("Error: device %s is not ready\n", receiver.port->name);
		k_msleep(1000);
	}

	ret = gpio_pin_configure_dt(&receiver, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", 
						ret, receiver.port->name, receiver.pin
		);
	}

	//ret = gpio_pin_interrupt_configure_dt(&receiver, GPIO_INT_EDGE_BOTH);
	ret = gpio_pin_interrupt_configure_dt(&receiver, GPIO_INT_EDGE_RISING);
	//ret = gpio_pin_interrupt_configure_dt(preceiver, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt %s pin %d\n", 
						ret, receiver.port->name, receiver.pin);
	}
	
	gpio_init_callback(&receiver_data, receiver_cb, BIT(receiver.pin));
	gpio_add_callback(receiver.port, &receiver_data);
}
//*/

Receiver::~Receiver()
{

}

int Receiver::getPulse(void)
{
	pulseCapture(
		TEST_PWM_PERIOD_USEC, 
		TEST_PWM_PULSE_USEC,
		TEST_PWM_UNIT_USEC, 
		PWM_CAPTURE_TYPE_PULSE | PWM_POLARITY_NORMAL
	);

	return 0;
}

void Receiver::printPulse(void)
{
	uint64_t time_us = getPulseTime();

	printf("(%llu) ", time_us);
}

//*
void Receiver::pulseCapture(
		uint32_t period, 
		uint32_t pulse, 
		enum test_pwm_unit unit, 
		pwm_flags_t flags
	)
{
	uint64_t period_capture = 0;
	uint64_t pulse_capture = 0;
	int err = 0;

	err = pwm_capture_usec(pwm_in.dev, pwm_in.pwm, flags, &period_capture,
						 &pulse_capture, K_USEC(period * 10));

	pwm_disable_capture(pwm_in.dev, pwm_in.pwm);

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
	//printf("(%llu) ", pulse_capture);
	setPulseTime(pulse_capture);
	//pulse_time_us = pulse_capture; 
}
//*/

uint64_t Receiver::getPulseTime(void)
{
	k_mutex_lock(&time_mutex, K_FOREVER);
	uint64_t time_us = pulse_time_us;
	k_mutex_unlock(&time_mutex);

	return time_us;
}

void Receiver::setPulseTime(uint64_t time_us)
{
	k_mutex_lock(&time_mutex, K_FOREVER);
	pulse_time_us = time_us;
	k_mutex_unlock(&time_mutex);
}

void Receiver::receiver_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
//	printf(".");

}

