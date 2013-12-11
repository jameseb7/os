#include <stdint.h>
#include "gdt.h"
#include "asm_functions.h"

extern uint32_t stack;

struct TSS tss = {
  .ss0 = 0x10, /*data segment selector*/
  .IOPB_offset = sizeof(struct TSS)
};

struct GDT_entry gdt[] = {
  {/*0x00 null table entry at start*/
    .limit_low = 0x0000,
    .base_low = 0x0000,
    .base_mid = 0x00,
    .access = 0x10,
    .flags_limit_high = 0x00,
    .base_high = 0x00
  },
  
  {/*0x08 code segment selector - base: 0x00000000, limit: 0xFFFFFFFF*/
    .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_mid = 0x00,
    .access = 0x9A, /*Present | Ring 0 | Executable | Readable*/
    .flags_limit_high = 0xCF,
    .base_high = 0x00
  },
  
  {/*0x10 data segment selector - base: 0x00000000, limit: 0xFFFFFFFF*/
    .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_mid = 0x00,
    .access = 0x92, /*Present | Ring 0 | Writable*/
    .flags_limit_high = 0xCF,
    .base_high = 0x00
  },

  {/*0x18 Task State Segment (TSS) selector*/
    .limit_low = sizeof(struct TSS) & 0xFFFF,
    .access = 0x89, /*Present | Ring 0 | Executable | Accessed*/
    .flags_limit_high = ((sizeof(struct TSS) >> 16) & 0x0F) | 0x40
  }
};

void setup_gdt(){
  tss.esp0 = stack;
 /* TODO: work out how to implement the syscall stack */

  gdt[3].base_low  = (uint16_t) ((uint32_t) &tss) & 0xFFFF;
  gdt[3].base_mid  = (uint8_t)  (((uint32_t) &tss) >> 16) & 0xFF;
  gdt[3].base_high = (uint8_t)  (((uint32_t) &tss) >> 24) & 0xFF;
  

  load_gdt((uint32_t) &gdt, sizeof(gdt));
}

