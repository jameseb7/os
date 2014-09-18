#include "kutil.h"
#include "processes.h"

void timer_handler(void);

uint32_t counter = 0;
void timer_handler(){
	cli();

	counter++;
	if(counter % 3 == 0){
		run_next_process();
	}

	outb(0x20, 0x20);
	sti();
}
