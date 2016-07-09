#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>

void processes_init(void);

void run_next_process(void);
void add_process(void (*start_process)(void));
void suspend_current_process(void);
void resume_process(uint16_t process_id);
uint16_t get_current_process_id(void);
void kill_process(uint16_t process_id);

void check_process_stack(void);

#endif /* PROCESSES_H */
