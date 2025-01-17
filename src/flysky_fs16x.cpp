#include "flysky_fs16x.h"
#include <string.h>

// Reciever

static FlySky *pFlySky = NULL;

//*
Receiver::Receiver(
		const gpio_dt_spec receiver,
		void (*receiver_isr)(const device *, gpio_callback *, uint32_t)
	) :
		receiver(receiver),
		receiver_isr(receiver_isr),
		receiver_data({0}),
		pulse_time_us(0),
		start_time(0),
		stop_time(0),
		total_cycles(0),
		total_ns(0)
{
	k_mutex_init(&time_mutex);
	
	int ret = -1;

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
//*/

Receiver::~Receiver()
{

}

/*
void Receiver::setFlySky(FlySky *pFlySky)
{
	this->pFlySky = pFlySky;	
}
//*/

void Receiver::printPulse(void)
{
	uint64_t time_us = getPulseTime();

	printf("\n(%llu)\n", time_us);
}

const gpio_dt_spec *Receiver::getReceiver(void)
{

	return &receiver;
}

uint64_t Receiver::getPulseTime(void)
{
	k_mutex_lock(&time_mutex, K_FOREVER);
	uint64_t time_us = pulse_time_us;
	k_mutex_unlock(&time_mutex);

	return time_us;
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
				stop_time = timing_counter_get();
				total_cycles = timing_cycles_get(&start_time, &stop_time);
				total_ns = timing_cycles_to_ns(total_cycles);
				setPulseTimeIsr(total_ns/1000);
			break;
		case 1:
				start_time = timing_counter_get();
			
			break;
		default:

			break;
	}

	return 0;
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

void FlySky::throttle_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pFlySky->getThrottle()->handleIsr();
}

void FlySky::roll_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pFlySky->getRoll()->handleIsr();
}

void FlySky::pitch_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pFlySky->getPitch()->handleIsr();
}

void FlySky::yaw_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	pFlySky->getYaw()->handleIsr();
}

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
	)
{
	//receiver.setFlySky(this); // 0_o

	pFlySky = this;
}

FlySky::~FlySky()
{

}

int FlySky::sampleFlysky(void)
{

	return 0;
}

K_MUTEX_DEFINE(my_mutex);
void FlySky::printPulse(void)
{
	k_mutex_lock(&my_mutex, K_FOREVER);
	uint64_t throttle_pulse_time_us = throttle.getPulseTime();
	uint64_t roll_pulse_time_us = roll.getPulseTime();
	uint64_t pitch_pulse_time_us = pitch.getPulseTime();
	uint64_t yaw_pulse_time_us = yaw.getPulseTime();
	k_mutex_unlock(&my_mutex);
	
	printf("\n(%llu)(%llu)(%llu)(%llu)", 
		throttle_pulse_time_us, 
		roll_pulse_time_us,
		pitch_pulse_time_us,
		yaw_pulse_time_us
	);
}

