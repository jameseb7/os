#include <stdint.h>
#include "memory.h"
#include "asm_functions.h"

extern uint32_t kernel_stack;

struct GDT_entry{
   uint16_t limit_low;        /*Limit 0:15*/
   uint16_t base_low;         /*Base 0:15*/
   uint8_t  base_mid;         /*Base 16:23*/
   uint8_t  access;           /*Access Byte*/
   uint8_t  flags_limit_high; /*Flags | Limit 16:19*/
   uint8_t  base_high;        /*Base 24:31*/
};

struct TSS{
   uint16_t link;
   uint16_t link_pad;

   uint32_t esp0;
   uint16_t ss0;
   uint16_t ss0_pad;

   uint32_t esp1;
   uint16_t ss1;
   uint16_t ss1_pad;

   uint32_t esp2;
   uint16_t ss2;
   uint16_t ss2_pad;

   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;

   uint16_t es;
   uint16_t es_pad;
   uint16_t cs;
   uint16_t cs_pad;
   uint16_t ss;
   uint16_t ss_pad;
   uint16_t ds;
   uint16_t ds_pad;
   uint16_t fs;
   uint16_t fs_pad;
   uint16_t gs;
   uint16_t gs_pad;
   uint16_t ldtr;
   uint16_t ldtr_pad;

   uint16_t IOPB_offset_pad;
   uint16_t IOPB_offset;
};

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
  tss.esp0 = kernel_stack;

  gdt[3].base_low  = (uint16_t) ((uint32_t) &tss) & 0xFFFF;
  gdt[3].base_mid  = (uint8_t)  (((uint32_t) &tss) >> 16) & 0xFF;
  gdt[3].base_high = (uint8_t)  (((uint32_t) &tss) >> 24) & 0xFF;
  

  load_gdt((uint32_t) &gdt, sizeof(gdt));
}

