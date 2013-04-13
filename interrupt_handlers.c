#include <stdint.h>
#include "kernel.h"

void divide_by_zero_handler(void);
void double_fault_handler(void);
void invalid_tss_handler(void);
void segment_not_present_handler(uint16_t);
void general_protection_fault_handler(void);
void page_fault_handler(void);

void divide_by_zero_handler(){
  kprintln("ERROR: Division by zero");
  halt();
}

void double_fault_handler(){
  kprintln("ERROR: Double fault");
  halt();
}

void invalid_tss_handler(){
  kprintln("ERROR: Invalid TSS");
  halt();
}

void segment_not_present_handler(uint16_t selector){
  kprint("ERROR: Segment not present:");
  kprintln(uint32_to_hex_string((uint32_t) selector));
  halt();
}

void general_protection_fault_handler(){
  kprintln("ERROR: General protection fault");
  halt();
}

void page_fault_handler(){
  kprintln("ERROR: Page fault");
  halt();
}
