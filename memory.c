#include <stdint.h>

#define PDE_PRESENT       1 << 0
#define PDE_WRITEABLE     1 << 1
#define PDE_USER          1 << 2
#define PDE_WRITE_THROUGH 1 << 3
#define PDE_CACHE_DISABLE 1 << 4
#define PDE_ACCESSED      1 << 5
#define PDE_LARGE_PAGES   1 << 7

#define PTE_PRESENT       1 << 0
#define PTE_WRITEABLE     1 << 1
#define PTE_USER          1 << 2
#define PTE_WRITE_THROUGH 1 << 3
#define PTE_CACHE_DISABLE 1 << 4
#define PTE_ACCESSED      1 << 5
#define PTE_DIRTY         1 << 6
#define PTE_GLOBAL        1 << 8

uint32_t * page_directory = 0x00001000;
uint32_t * page_table;

void make_page_directory(){
   int i;

   page_directory[0] = 0x00002000 | PDE_PRESENT | PDE_WRITEABLE;
   for(i = 1; i < 1023; i++) page_directory[i] = 0x00000000;
   page_directory[1023] = ((uint32_t) page_directory) | PDE_PRESENT | PDE_WRITEABLE;

   page_table = 0x00002000;

   /*set up identity paging for the first two megabyte*/
   for(i = 0; i < 512; i++) page_table[i] = (((uint32_t) i) << 12) | PTE_PRESENT | PTE_WRITEABLE;
}

 







