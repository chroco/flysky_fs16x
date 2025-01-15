#include "receiver.h"

// Reciever


static Receiver *pReceiver = NULL;

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

	pReceiver = this;

	__ASSERT(device_is_ready(pwm_in.dev) == true, "Device not ready");

/*
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
//*/
}
//*/

Receiver::~Receiver()
{

}

void Receiver::printPulse(void)
{
	uint64_t time_us = getPulseTime();

	printf("\n(%llu)\n", time_us);
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
						 //&pulse_capture, K_USEC(period * 4));
	
	__ASSERT(err == 0, "failed to capture pwm (err %d)", err);

	pwm_disable_capture(pwm_in.dev, pwm_in.pwm);

	//printf("(%llu) ", pulse_capture);
	setPulseTime(pulse_capture);
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

int Receiver::work(void)
{
  int ret = 0;
	
	timing_t start_time, end_time;
	uint64_t total_cycles;
	uint64_t total_ns;

	timing_init();
	timing_start();

	start_time = timing_counter_get();
  while (1) 
  {
		pReceiver->getPulse();
		
		end_time = timing_counter_get();
		total_cycles = timing_cycles_get(&start_time, &end_time);
		start_time = end_time;
		total_ns = timing_cycles_to_ns(total_cycles);
   	printf("[%llu] ", total_ns/1000);
		//k_usleep(1);
    //k_msleep(SLEEP_TIME_MS);
  }

	return ret;
}

void Receiver::receiver_entry(void *unused0, void *unused1, void *unused2)
{
  (void) unused0;
  (void) unused1;
  (void) unused2;

  if(work())
  {
    printk("ERROR: receiver thread exited!");
  }
}

K_THREAD_STACK_DEFINE(receiver_stack_area, RECEIVER_STACK_SIZE);
K_THREAD_DEFINE(
  receiver_tid, RECEIVER_STACK_SIZE,
  Receiver::receiver_entry, NULL, NULL, NULL,
  RECEIVER_PRIORITY, 0, 0
);

int Receiver::startReceiverThread(void)
{
  k_thread_start(receiver_tid);

  return 0;
}
