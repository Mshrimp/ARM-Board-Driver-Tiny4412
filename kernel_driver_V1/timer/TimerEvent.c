


#define	TimerPeriod		10

typedef struct {
	void (*handler)(void);
	unsigned int period;
	unsigned int count;
} EventInfo;

static EventInfo PeriodEvent;
struct timer_list EventTimer;
int count = 0;

void function_tmp(void);

static void timer_loop(void)
{
	count++;
	mod_timer(&EventTimer, jiffies + TimerPeriod);
	printk("EventTimer: timer loop, Do something!\n");
	function_tmp =  EventTimer.handler;
	if (function_tmp != NULL)
	{
		EventTimer.count++;
		if (EventTimer.count >= EventTimer.period)
		{
			function_tmp();
			EventTimer.count = 0;
		}
	}
}


static void timer_init(void)
{
	printk("EventTimer: Timer init!\n");
	init_timer(&EventTimer);

	EventTimer.expires = jiffies + TimerPeriod;
	EventTimer.function = timer_loop;
	add_timer(&EventTimer);

}

void PeriodEvent_Register(void (*handler)(void), EventPeriodTime)
{
	printk("EventTimer: Timer register!\n");
	PeriodEvent.handler = handler;
	PeriodEvent.period = EventPeriodTime;
	PeriodEvent.count = 0;
}

void PeriodEvent_Cancel(void (*handler)(void))
{
	printk("EventTimer: Timer cancel!\n");
	PeriodEvent.handler = NULL;
	PeriodEvent.period = 0;
	PeriodEvent.count = 0;
}

void timer_destory(void)
{
	del_timer(&EventTimer);
	printk("EventTimer: Timer destory!\n");
}
