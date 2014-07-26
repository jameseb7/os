#include <stdint.h>
#include "kutil.h"

void divide_by_zero_handler(void);
void double_fault_handler(void);
void invalid_tss_handler(void);
void segment_not_present_handler(uint16_t);
void general_protection_fault_handler(uint32_t);
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
  kprintln_uint32((uint32_t) selector);
  halt();
}

void general_protection_fault_handler(uint32_t error_code){
  kprint("ERROR: General protection fault");
  kprintln_uint32(error_code);
  halt();
}

void page_fault_handler(){
  uint32_t error_code, address;

  __asm__("movl %%cr2, %0" : "=r"(address));
  __asm__("pop %0" : "=r"(error_code));

  kprintln("ERROR: Page fault");
  kprint("error code: ");
  kprintln_uint32(error_code);
  kprint("address: ");
  kprintln_uint32(address);

  halt();
}
