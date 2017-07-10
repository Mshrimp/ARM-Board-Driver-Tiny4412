


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
	printf("EventTimer: Do something!\n");
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
	init_timer(&EventTimer);

	EventTimer.expires = jiffies + TimerPeriod;
	EventTimer.function = timer_loop;
	add_timer(&EventTimer);

}

void PeriodEvent_Register(void (*handler)(void), EventPeriodTime)
{
	PeriodEvent.handler = handler;
	PeriodEvent.period = EventPeriodTime;
	PeriodEvent.count = 0;
}

void PeriodEvent_Cancel(void (*handler)(void))
{
	PeriodEvent.handler = NULL;
	PeriodEvent.period = 0;
	PeriodEvent.count = 0;
}


