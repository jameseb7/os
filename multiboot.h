#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

typedef struct {
   uint32_t mod_start;
   uint32_t mod_end;

   char * string;

   uint32_t reserved;
} boot_module;

typedef struct {
   uint32_t tabsize;
   uint32_t strsize;
   uint32_t addr;
   uint32_t reserved;
} aout_symbol_table;

typedef struct {
   uint32_t num;
   uint32_t size;
   uint32_t addr;
   uint32_t shndx;
} elf_section_header;

typedef struct {
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
} mmap;

typedef struct {
   uint32_t size;

   uint8_t  drive_number;

   uint8_t  drive_mode;

   uint16_t drive_cylinders;
   uint8_t  drive_heads;
   uint8_t  drive_sectors;

   uint8_t drive_ports[];
} drives;

typedef struct {
   uint16_t version;
   uint16_t cseg;
   uint32_t offset;
   uint16_t cseg_16;
   uint16_t dseg;
   uint16_t flags;
   uint16_t cseg_len;
   uint16_t cseg_16_len;
   uint16_t dseg_len;
} apm_table_t;


typedef struct {
   uint32_t flags;

   uint32_t mem_lower;
   uint32_t mem_upper;

   uint32_t boot_device;

   uint32_t command_line;

   uint32_t mods_count;
   uint32_t mods_addr;

   union {
      aout_symbol_table aout;
      elf_section_header elf;
   } syms;

   uint32_t mmap_length;
   uint32_t mmap_addr;

   uint32_t drives_length;
   uint32_t drives_addr;

   uint32_t config_table;

   uint32_t boot_loader_name;

   uint32_t apm_table;

   uint32_t vbe_control_info;
   uint32_t vbe_mode_info;
   uint16_t vbe_mode;
   uint16_t vbe_interface_seg;
   uint16_t vbe_interface_off;
   uint16_t vbe_interface_len;
} multiboot_data;

#endif /*MULTIBOOT_H*/
