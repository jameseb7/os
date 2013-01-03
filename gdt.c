#include <stdint.h>
#include "kernel.h"

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

struct GDT_entry * gdt = (struct GDT_entry *) 0x00000500;
uint16_t gdt_size;
struct TSS * tss = (struct TSS *) 0x00000600;

extern uint32_t stack;

void make_gdt(){
   /*0x00 null table entry at start*/
   gdt[0].limit_low = 0x0000;
   gdt[0].base_low = 0x0000;
   gdt[0].base_mid = 0x00;
   gdt[0].access = 0x10;
   gdt[0].flags_limit_high = 0x00;
   gdt[0].base_high = 0x00;
   
   /*0x08 code segment selector - base: 0x00000000, limit: 0xFFFFFFFF*/
   gdt[1].limit_low = 0xFFFF;
   gdt[1].base_low = 0x0000;
   gdt[1].base_mid = 0x00;
   gdt[1].access = 0x9A; /*Present | Ring 0 | Executable | Readable*/
   gdt[1].flags_limit_high = 0xCF;
   gdt[1].base_high = 0x00;

   /*0x10 data segment selector - base: 0x00000000, limit: 0xFFFFFFFF*/
   gdt[2].limit_low = 0xFFFF;
   gdt[2].base_low = 0x0000;
   gdt[2].base_mid = 0x00;
   gdt[2].access = 0x92; /*Present | Ring 0 | Writable*/
   gdt[2].flags_limit_high = 0xCF;
   gdt[2].base_high = 0x00;

   /*0x18 Task State Segment (TSS) selector - base: 0x00000600*/
   gdt[3].limit_low = sizeof(struct TSS) & 0xFFFF;
   gdt[3].base_low = 0x0600;
   gdt[3].base_mid = 0x00;
   gdt[3].access = 0x89; /*Present | Ring 0 | Executable | Accessed*/
   gdt[3].flags_limit_high = 0x40;
   gdt[3].base_high = 0x00;

   gdt_size = 4 * sizeof(struct GDT_entry);

   tss->ss0 = 0x10; /*data segment selector*/
   tss->esp0 = stack; 
   tss->IOPB_offset = sizeof(struct TSS);
}
