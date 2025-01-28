#include "flysky_fs16x.h"
#include <string.h>

// Reciever

Receiver::Receiver(
		const gpio_dt_spec receiver,
		void (*receiver_isr)(const device *, gpio_callback *, uint32_t)
	) :
		receiver(receiver),
		receiver_data({0}),
		pulse_time_us(0),
		start_time(0),
		stop_time(0)
{
	k_mutex_init(&receiver_mutex);
	
	int ret = 0;

	ret = gpio_is_ready_dt(&receiver);
	__ASSERT(ret != 0, "Error: device %s is not ready\n", receiver.port->name);

	ret = gpio_pin_configure_dt(&receiver, GPIO_INPUT);
	__ASSERT(ret == 0, "Error %d: failed to configure %s pin %d\n", 
				ret, receiver.port->name, receiver.pin);

	ret = gpio_pin_interrupt_configure_dt(&receiver, GPIO_INT_EDGE_BOTH);
	__ASSERT(ret == 0, "Error %d: failed to configure interrupt %s pin %d\n", 
					ret, receiver.port->name, receiver.pin);
	
	gpio_init_callback(&receiver_data, receiver_isr, BIT(receiver.pin));
	gpio_add_callback(receiver.port, &receiver_data);
	
	timing_init();
	timing_start();
}

Receiver::~Receiver()
{

}

void Receiver::printPulse(void)
{
	uint64_t time_us = getMutexPulseTime();

	printf("\n(%llu)\n", time_us);
}

const gpio_dt_spec *Receiver::getReceiver(void)
{

	return &receiver;
}

uint64_t Receiver::getMutexPulseTime(void)
{
	k_mutex_lock(&receiver_mutex, K_FOREVER);
	uint64_t time_us = pulse_time_us;
	k_mutex_unlock(&receiver_mutex);

	return time_us;
}

uint64_t Receiver::getPulseTime(void)
{
	return pulse_time_us;
}

void Receiver::setPulseTimeIsr(uint64_t time_us)
{
	pulse_time_us = time_us;
}

int Receiver::handleIsr(void)
{
	int pin_state = gpio_pin_get_dt(getReceiver());
	
	switch(pin_state)
	{
		case 0:
			{
				stop_time = timing_counter_get();
				uint64_t total_cycles = timing_cycles_get(&start_time, &stop_time);
				uint64_t total_ns = timing_cycles_to_ns(total_cycles);
				setPulseTimeIsr(total_ns/1000);
			}
			break;
		case 1:
				start_time = timing_counter_get();
			
			break;
		default:

			break;
	}

	return 0;
}

// FlySky

FlySky *FlySky::pThis = NULL;
flysky_data_t FlySky::flysky_data = {0};

FlySky::FlySky() :
	throttle(
		Receiver(
			GPIO_DT_SPEC_GET_OR(THROTTLE_NODE, gpios, {0}),
			&FlySky::throttle_isr
		)
	),
	roll(
		Receiver(
			GPIO_DT_SPEC_GET_OR(ROLL_NODE, gpios, {0}),
			&FlySky::roll_isr
		)
	),
	pitch(
		Receiver(
			GPIO_DT_SPEC_GET_OR(PITCH_NODE, gpios, {0}),
			&FlySky::pitch_isr
		)
	),
	yaw(
		Receiver(
			GPIO_DT_SPEC_GET_OR(YAW_NODE, gpios, {0}),
			&FlySky::yaw_isr
		)
	),
	vra(
		Receiver(
			GPIO_DT_SPEC_GET_OR(VRA_NODE, gpios, {0}),
			&FlySky::vra_isr
		)
	),
	vrb(
		Receiver(
			GPIO_DT_SPEC_GET_OR(VRB_NODE, gpios, {0}),
			&FlySky::vrb_isr
		)
	) /*,
	r6(
		Receiver(
			GPIO_DT_SPEC_GET_OR(R6_NODE, gpios, {0}),
			&FlySky::r6_isr
		)
	),
	r7(
		Receiver(
			GPIO_DT_SPEC_GET_OR(R7_NODE, gpios, {0}),
			&FlySky::r7_isr
		)
	),
	r8(
		Receiver(
			GPIO_DT_SPEC_GET_OR(R8_NODE, gpios, {0}),
			&FlySky::r8_isr
		)
	),
	r9(
		Receiver(
			GPIO_DT_SPEC_GET_OR(R9_NODE, gpios, {0}),
			&FlySky::r9_isr
		)
	)
//*/
{
	pThis = this; 

	k_mutex_init(&flysky_mutex);
}

FlySky::~FlySky()
{

}

void FlySky::capturePulses(flysky_data_t *pflysky_data)
{
	k_mutex_lock(&flysky_mutex, K_FOREVER);
	pflysky_data->throttle_pulse_time_us = throttle.getPulseTime();
	pflysky_data->roll_pulse_time_us = roll.getPulseTime();
	pflysky_data->pitch_pulse_time_us = pitch.getPulseTime();
	pflysky_data->yaw_pulse_time_us = yaw.getPulseTime();
	pflysky_data->vra_pulse_time_us = vra.getPulseTime();
	pflysky_data->vrb_pulse_time_us = vrb.getPulseTime();
/*
	pflysky_data->r6_pulse_time_us = r6.getPulseTime();
	pflysky_data->r7_pulse_time_us = r7.getPulseTime();
	pflysky_data->r8_pulse_time_us = r8.getPulseTime();
	pflysky_data->r9_pulse_time_us = r9.getPulseTime();
//*/
	k_mutex_unlock(&flysky_mutex);
}

void FlySky::printPulses(void)
{
	flysky_data_t temp_flysky_data = {0};
	capturePulses(&temp_flysky_data);
	printPulses(&temp_flysky_data);
}

void FlySky::printPulses(flysky_data_t *pflysky_data)
{
	printf(
		"\n(T: %4llu)(Y: %4llu)(P: %4llu)(R: %4llu)(A: %4llu)(B: %4llu)", //(0: %4llu)", // (0: %4llu)(0: %4llu)(0: %4llu)", 
		pflysky_data->throttle_pulse_time_us, 
		pflysky_data->yaw_pulse_time_us,
		pflysky_data->pitch_pulse_time_us,
		pflysky_data->roll_pulse_time_us,
		pflysky_data->vra_pulse_time_us,
		pflysky_data->vrb_pulse_time_us/*,
		pflysky_data->r6_pulse_time_us,
		pflysky_data->r7_pulse_time_us,
		pflysky_data->r8_pulse_time_us,
		pflysky_data->r9_pulse_time_us
//*/
	);
}

Receiver *FlySky::getThrottle(void)
{
	return &throttle;
}

Receiver *FlySky::getRoll(void)
{
	return &roll;
}

Receiver *FlySky::getPitch(void)
{
	return &pitch;
}

Receiver *FlySky::getYaw(void)
{
	return &yaw;
}

Receiver *FlySky::getVrA(void)
{
	return &vra;
}

Receiver *FlySky::getVrB(void)
{
	return &vrb;
}

/*
Receiver *FlySky::getR6(void)
{
	return &r6;
}

Receiver *FlySky::getR7(void)
{
	return &r7;
}

Receiver *FlySky::getR8(void)
{
	return &r8;
}

Receiver *FlySky::getR9(void)
{
	return &r9;
}
//*/

void FlySky::throttle_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getThrottle()->handleIsr();
}

void FlySky::roll_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getRoll()->handleIsr();
}

void FlySky::pitch_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getPitch()->handleIsr();
}

void FlySky::yaw_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getYaw()->handleIsr();
}

void FlySky::vra_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getVrA()->handleIsr();
}

void FlySky::vrb_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getVrB()->handleIsr();
}

/*
void FlySky::r6_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getR6()->handleIsr();
}

void FlySky::r7_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getR7()->handleIsr();
}

void FlySky::r8_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getR8()->handleIsr();
}

void FlySky::r9_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pThis->getR9()->handleIsr();
}
//*/
