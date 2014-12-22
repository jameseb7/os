#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>

void processes_init(void);

void run_next_process(void);
void add_process(void (*start_process)(void));

void check_process_stack(void);

#endif /* PROCESSES_H */
