#include "kutil.h"
#include "processes.h"

void timer_handler(void);

uint32_t counter = 0;
void timer_handler(){
	cli("timer_handler()");
	/* clock interrupt fires 100 times a second */

	counter++;
	if(counter % 3 == 0){
		run_next_process();
	}

	outb(0x20, 0x20);
	outb(0xA0, 0x20);
	sti("timer_handler()");
}
