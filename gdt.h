#ifndef GDT_H
#define GDT_H

#include <stdint.h>

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

void setup_gdt(void);

#endif
