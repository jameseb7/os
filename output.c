#include <stdint.h>
#include "kernel.h"

static volatile char * videoram = (char *) 0xB8000;
static unsigned int current_row = 0;
static unsigned int current_col = 0;

static char parse_nybble(uint8_t);

unsigned int write_screen(const char * str,
			  char foreclr, char backclr,
			  unsigned int x, unsigned int y){
   unsigned int i; /*string index*/

   for(i = 0; str[i] != '\0'; i++){
      videoram[2*(80*y + x + i)] = str[i];
      foreclr = (char) foreclr & ((char) 0x0F);
      backclr = (char) (backclr << 4);
      videoram[2*(80*y + x + i) + 1] = backclr | foreclr;
   }
   return i;
}

void clear_screen(){
   unsigned int i, j;
   for(i = 0; i < 80; i++){
      for(j = 0; j < 25; j++){
         write_screen(" ", 0x07, 0x00, i, j);
      }
   }
   current_row = 0;
   current_col = 0;
}

void kprint(const char * str){
   current_col += write_screen(str, 0x07, 0x00, current_col, current_row);
   if(current_col >= 80){
      current_col = 0;
      current_row++;
   }
}
   
void kprintln(const char * str){
   write_screen(str, 0x07, 0x00, current_col, current_row);
   current_row++;
   current_col = 0;
   if(current_row >= 25) current_row = 0;
}

static char parse_nybble(uint8_t nybble){
  switch(nybble & 0x0F){
  case 0x0:
    return '0';
    break; 
  case 0x1:
    return '1';
    break; 
  case 0x2:
    return '2';
    break; 
  case 0x3:
    return '3';
    break; 
  case 0x4:
    return '4';
    break; 
  case 0x5:
    return '5';
    break; 
  case 0x6:
    return '6';
    break; 
  case 0x7:
    return '7';
    break; 
  case 0x8:
    return '8';
    break; 
  case 0x9:
    return '9';
    break; 
  case 0xA:
    return 'A';
    break; 
  case 0xB:
    return 'B';
    break; 
  case 0xC:
    return 'C';
    break; 
  case 0xD:
    return 'D';
    break; 
  case 0xE:
    return 'E';
    break; 
  case 0xF:
    return 'F';
    break;
  default:
	return '0';
  }
}

char * uint32_to_hex_string(uint32_t input){
   char * output = (char *) 0x00007E00;
   int i;
   
   output[0] = '0';
   output[1] = 'x';

   for(i = 0; i < 8; i++){
     output[9-i] = parse_nybble((uint8_t) (input >> i*4) & 0x0F);
   }

   output[10] = '\0';

   return output;
}

char * uint64_to_hex_string(uint64_t input){
  char * output = (char *) 0x00007E00;
   int i;
   
   output[0] = '0';
   output[1] = 'x';

   for(i = 0; i < 16; i++){
     output[17-i] = parse_nybble((uint8_t) (input >> i*4) & 0x0F);
   }

   output[18] = '\0';

   return output;
}
