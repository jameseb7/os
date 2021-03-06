#include <stdint.h>
#include "kutil.h"

void divide_by_zero_handler(void);
void debug_handler(void);
void double_fault_handler(void);
void invalid_tss_handler(void);
void segment_not_present_handler(uint16_t);
void general_protection_fault_handler(uint32_t);
void page_fault_handler(uint32_t, uint32_t, uint32_t);

void divide_by_zero_handler(){
  error("ERROR: Division by zero");
}

void debug_handler(){
}

void double_fault_handler(){
  error("ERROR: Double fault");
}

void invalid_tss_handler(){
  error("ERROR: Invalid TSS");
}

void segment_not_present_handler(uint16_t selector){
  kprint("ERROR: Segment not present:");
  kprintln_uint32((uint32_t) selector);
  cli("segment_not_present_handler()");
  halt();
}

void general_protection_fault_handler(uint32_t error_code){
  kprint("ERROR: General protection fault");
  kprintln_uint32(error_code);
  cli("general_protection_fault_handler()");
  halt();
}

void page_fault_handler(uint32_t error_code, uint32_t address, uint32_t instruction_address){
  kprintln("ERROR: Page fault");
  kprint("error code: ");
  kprintln_uint32(error_code);
  kprint("address: ");
  kprintln_uint32(address);
  kprint("instruction address: ");
  kprintln_uint32(instruction_address);

  cli("page_fault_handler()");
  halt();
}
