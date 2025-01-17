#ifndef _FLYSKY_FS16X_H_
#define _FLYSKY_FS16X_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/timing/timing.h>

#define THROTTLE_NODE			DT_ALIAS(receiver0)
#define ROLL_NODE					DT_ALIAS(receiver1)
#define PITCH_NODE				DT_ALIAS(receiver2)
#define YAW_NODE					DT_ALIAS(receiver3)
#define LED0_NODE					DT_ALIAS(led0)

#define FLYSKY_STACK_SIZE		500
#define FLYSKY_PRIORITY			5

struct pwm_t {
	const device *dev;
	uint32_t pwm;
	pwm_flags_t flags;
};

//class FlySky;

class Receiver
{
	public:
		Receiver(
			const gpio_dt_spec, 
			void (*)(const device *, gpio_callback *, uint32_t)
		);
		~Receiver();

		//void setFlySky(FlySky *);

		void printPulse(void);
		const gpio_dt_spec *getReceiver(void);
		void setPulseTimeIsr(uint64_t);
		uint64_t getPulseTime(void);
		int handleIsr(void);
	private:
		const gpio_dt_spec receiver;
		void (*receiver_isr)(const device *, gpio_callback *, uint32_t);
		gpio_callback receiver_data;
		uint64_t pulse_time_us;

		timing_t start_time;
		timing_t stop_time;
		uint64_t total_cycles;
		uint64_t total_ns;


		k_mutex time_mutex;
		//FlySky *pFlySky;
};

class FlySky {
	public:
		FlySky();
		~FlySky();

		int sampleFlysky(void);
		void printPulse(void);
		
		static void flysky_entry(void *, void *, void *);
		int startFlySkyThread(void);
		Receiver *getThrottle(void);
		Receiver *getRoll(void);
		Receiver *getPitch(void);
		Receiver *getYaw(void);
	private:
		static void throttle_isr(const struct device *, struct gpio_callback *, uint32_t);
		static void roll_isr(const struct device *, struct gpio_callback *, uint32_t);
		static void pitch_isr(const struct device *, struct gpio_callback *, uint32_t);
		static void yaw_isr(const struct device *, struct gpio_callback *, uint32_t);
		Receiver throttle;
		Receiver roll;
		Receiver pitch;
		Receiver yaw;
};

#endif
