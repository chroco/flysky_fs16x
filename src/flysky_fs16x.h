#ifndef _FLYSKY_FS16X_H_
#define _FLYSKY_FS16X_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/timing/timing.h>

#define RECEIVER_NODE	DT_ALIAS(receiver0)
#define LED0_NODE DT_ALIAS(led0)

#define RECEIVER_STACK_SIZE 500
#define RECEIVER_PRIORITY 5

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
		Receiver(
			const gpio_dt_spec, 
			gpio_callback *, 
			void (*)(const device *, gpio_callback *, uint32_t)
		);
		~Receiver();

		//void setFlySky(FlySky *);

		void printPulse(void);
		const gpio_dt_spec *getReceiver(void);
		//static void receiver_entry(void *, void *, void *);
		//int startReceiverThread(void);
	private:
		//FlySky *pFlySky;
		k_mutex time_mutex;
		static uint64_t pulse_time_us;
		uint64_t getPulseTime(void);
		void setPulseTime(uint64_t);
		
		const gpio_dt_spec receiver;
		static gpio_callback receiver_data;
		gpio_callback *preceiver_data;
		static void receiver_isr(const device *, gpio_callback *cb, uint32_t);
		void (*preceiver_isr)(const device *, gpio_callback *cb, uint32_t);

		//static int work(void);
};

class FlySky {
	public:
		FlySky();
		~FlySky();

		int sampleFlysky(void);
		void printPulse(void);
	private:
		Receiver receiver;
};

#endif
