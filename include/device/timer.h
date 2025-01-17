#ifndef __TIMER_DEVICE_H__
#define __TIMER_DEVICE_H__

#define TIMER_HZ 60
#define IRQ_TIMER 0x80000007

void add_timer_exec(void (*subhandler)(void));
void stop_timers();
void resume_timers();

#endif
