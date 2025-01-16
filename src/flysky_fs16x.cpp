#include "flysky_fs16x.h"
#include <string.h>

// Reciever

static Receiver *pReceiver = NULL;

struct gpio_callback Receiver::receiver_data = {0};

uint64_t Receiver::pulse_time_us = 0;
static uint64_t pulse_time_us = 0;

Receiver::Receiver(
		const gpio_dt_spec receiver,
		gpio_callback *preceiver_data, 
		void (*preceiver_isr)(const device *, gpio_callback *, uint32_t)
	) :
		receiver(receiver),
		preceiver_data(preceiver_data),
		preceiver_isr(preceiver_isr)
{
	k_mutex_init(&time_mutex);

	pReceiver = this;

	int ret = 0;

	ret = gpio_is_ready_dt(&receiver);
	__ASSERT(ret != 0, "Error: device %s is not ready\n", receiver.port->name);

	ret = gpio_pin_configure_dt(&receiver, GPIO_INPUT);
	__ASSERT(ret == 0, "Error %d: failed to configure %s pin %d\n", 
					ret, receiver.port->name, receiver.pin);

	ret = gpio_pin_interrupt_configure_dt(&receiver, GPIO_INT_EDGE_BOTH);
	__ASSERT(ret == 0, "Error %d: failed to configure interrupt %s pin %d\n", 
					ret, receiver.port->name, receiver.pin);
	
	gpio_init_callback(preceiver_data, preceiver_isr, BIT(receiver.pin));
	gpio_add_callback(receiver.port, preceiver_data);
	
	timing_init();
	timing_start();
}


Receiver::Receiver(const gpio_dt_spec receiver) :
	receiver(receiver)
{
	k_mutex_init(&time_mutex);

	pReceiver = this;

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

void Receiver::setPulseTime(uint64_t time_us)
{
	k_mutex_lock(&time_mutex, K_FOREVER);
	pulse_time_us = time_us;
	k_mutex_unlock(&time_mutex);
}

timing_t cb_start_time, cb_stop_time;
uint64_t cb_total_cycles;
uint64_t cb_total_ns;
uint64_t cb_count = 0;

void Receiver::receiver_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	//printf(".");
	int pin_state = gpio_pin_get_dt(pReceiver->getReceiver());
	
	switch(pin_state)
	{
		case 0:
				cb_stop_time = timing_counter_get();
				cb_total_cycles = timing_cycles_get(&cb_start_time, &cb_stop_time);
				cb_total_ns = timing_cycles_to_ns(cb_total_cycles);
				pReceiver->pulse_time_us = cb_total_ns/1000;
			break;
		case 1:
				cb_start_time = timing_counter_get();
			
			break;
		default:

			break;
	}
}

static void receiver_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	//printf(".");
	int pin_state = gpio_pin_get_dt(pReceiver->getReceiver());
	
	switch(pin_state)
	{
		case 0:
				cb_stop_time = timing_counter_get();
				cb_total_cycles = timing_cycles_get(&cb_start_time, &cb_stop_time);
				cb_total_ns = timing_cycles_to_ns(cb_total_cycles);
				//pReceiver->pulse_time_us = cb_total_ns/1000;
				pulse_time_us = cb_total_ns/1000;
			break;
		case 1:
				cb_start_time = timing_counter_get();
			
			break;
		default:

			break;
	}
}

static gpio_callback receiver_data = {0};

FlySky::FlySky() :
/*
	receiver(
		Receiver(GPIO_DT_SPEC_GET_OR(RECEIVER_NODE, gpios, {0}))
	)
//*/
//*
	receiver(
		Receiver(
			GPIO_DT_SPEC_GET_OR(RECEIVER_NODE, gpios, {0}),
			&receiver_data,
			&receiver_isr
		)
	)
//*/
{
	//receiver.setFlySky(this); // 0_o
	
	//receiver.startReceiverThread();
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
	//receiver.printPulse();
	k_mutex_lock(&my_mutex, K_FOREVER);
	printf("\n(%llu)\n",pulse_time_us);
	k_mutex_unlock(&my_mutex);
}

/*
int Receiver::work(void)
{
  int ret = 0;
	
	timing_t start_time, end_time;
	uint64_t total_cycles;
	uint64_t total_ns;

	start_time = timing_counter_get();
  while (1) 
  {
		//pReceiver->getPulse();
    k_msleep(100);
		
		end_time = timing_counter_get();
		total_cycles = timing_cycles_get(&start_time, &end_time);
		start_time = end_time;
		total_ns = timing_cycles_to_ns(total_cycles);
   	//printf("\n[%llu]\n", total_ns/1000);
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
//*/
