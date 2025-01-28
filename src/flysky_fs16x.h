#ifndef _FLYSKY_FS16X_H_
#define _FLYSKY_FS16X_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/timing/timing.h>

#define ROLL_NODE					DT_ALIAS(receiver0)
#define PITCH_NODE				DT_ALIAS(receiver1) 
#define THROTTLE_NODE			DT_ALIAS(receiver2)
#define YAW_NODE					DT_ALIAS(receiver3)  
#define VRA_NODE					DT_ALIAS(receiver4)
#define VRB_NODE					DT_ALIAS(receiver5)
/*
#define R6_NODE						DT_ALIAS(receiver6)
#define R7_NODE						DT_ALIAS(receiver7)
#define R8_NODE						DT_ALIAS(receiver8)
#define R9_NODE						DT_ALIAS(receiver9)
//*/
#define LED0_NODE					DT_ALIAS(led0)

typedef void receiver_isr_t(const struct device *, struct gpio_callback *, uint32_t);

class Receiver
{
	public:
		Receiver(
			const gpio_dt_spec, 
			void (*)(const device *, gpio_callback *, uint32_t)
		);
		~Receiver();

		void printPulse(void);
		
		const gpio_dt_spec *getReceiver(void);
		uint64_t getMutexPulseTime(void);
		uint64_t getPulseTime(void);
		
		void setPulseTimeIsr(uint64_t);
		int handleIsr(void);
	private:
		const gpio_dt_spec receiver;
		gpio_callback receiver_data;
		uint64_t pulse_time_us;
		timing_t start_time;
		timing_t stop_time;

		k_mutex receiver_mutex;
};

struct flysky_data_t
{
	uint64_t throttle_pulse_time_us;
	uint64_t roll_pulse_time_us;
	uint64_t pitch_pulse_time_us;
	uint64_t yaw_pulse_time_us;
	uint64_t vra_pulse_time_us;
	uint64_t vrb_pulse_time_us;
/*
	uint64_t r6_pulse_time_us;
	uint64_t r7_pulse_time_us;
	uint64_t r8_pulse_time_us;
	uint64_t r9_pulse_time_us;
//*/
};

class FlySky {
	public:
		FlySky();
		~FlySky();

		void printPulses(void);
		void printPulses(flysky_data_t *);
		void capturePulses(flysky_data_t *);
		
		Receiver *getThrottle(void);
		Receiver *getRoll(void);
		Receiver *getPitch(void);
		Receiver *getYaw(void);
		Receiver *getVrA(void);
		Receiver *getVrB(void);
/*
		Receiver *getR6(void);
		Receiver *getR7(void);
		Receiver *getR8(void);
		Receiver *getR9(void);
//*/
	private:
		Receiver throttle;
		Receiver roll;
		Receiver pitch;
		Receiver yaw;
		Receiver vra;
		Receiver vrb;
/*
		Receiver r6;
		Receiver r7;
		Receiver r8;
		Receiver r9;
//*/

		static FlySky *pThis;
		static flysky_data_t flysky_data;
		static receiver_isr_t throttle_isr;
		static receiver_isr_t roll_isr;
		static receiver_isr_t pitch_isr;
		static receiver_isr_t yaw_isr;
		static receiver_isr_t vra_isr;
		static receiver_isr_t vrb_isr;
/*
		static receiver_isr_t r6_isr;
		static receiver_isr_t r7_isr;
		static receiver_isr_t r8_isr;
		static receiver_isr_t r9_isr;
//*/
		
		k_mutex flysky_mutex;
};

#endif
