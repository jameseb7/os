#include <stdint.h>

int write_screen(char * str,
                  unsigned char foreclr, unsigned char backclr,
                  unsigned int x, unsigned int y){
   int i; /*string index*/
   volatile unsigned char * videoram = (unsigned char *) 0xB8000;

   for(i = 0; str[i] != '\0'; i++){
      videoram[2*(80*y + x + i)] = str[i];
      videoram[2*(80*y + x + i) + 1] = (backclr << 4) | (foreclr & 0x0F);
   }
   return i;
}

void clear_screen(){
   int i, j;
   for(i = 0; i < 80; i++){
      for(j = 0; j < 25; j++){
         write_screen(" ", 0x07, 0x00, i, j);
      }
   }
}

static unsigned int current_row = 0;
static unsigned int current_col = 0;

void kprint(char * str){
   current_col += write_screen(str, 0x07, 0x00, current_col, current_row);
   if(current_col >= 80){
      current_col = 0;
      current_row++;
   }
}
   
void kprintln(char * str){
   write_screen(str, 0x07, 0x00, current_col, current_row);
   current_row++;
   current_col = 0;
   if(current_row >= 25) current_row = 0;
}

char * intptr_to_hex_string(uint32_t input){
   char * output = 0x00007E00;
   int i;
   
   output[0] = '0';
   output[1] = 'x';

   for(i = 0; i < 8; i++){
      switch((input >> (i*4)) & 0xF){
      case 0x0:
         output[9-i] = '0';
         break; 
      case 0x1:
         output[9-i] = '1';
         break; 
      case 0x2:
         output[9-i] = '2';
         break; 
      case 0x3:
         output[9-i] = '3';
         break; 
      case 0x4:
         output[9-i] = '4';
         break; 
      case 0x5:
         output[9-i] = '5';
         break; 
      case 0x6:
         output[9-i] = '6';
         break; 
      case 0x7:
         output[9-i] = '7';
         break; 
      case 0x8:
         output[9-i] = '8';
         break; 
      case 0x9:
         output[9-i] = '9';
         break; 
      case 0xA:
         output[9-i] = 'A';
         break; 
      case 0xB:
         output[9-i] = 'B';
         break; 
      case 0xC:
         output[9-i] = 'C';
         break; 
      case 0xD:
         output[9-i] = 'D';
         break; 
      case 0xE:
         output[9-i] = 'E';
         break; 
      case 0xF:
         output[9-i] = 'F';
         break;
      default:
         output[9-i] = '0';
      }
   }

   output[10] = '\0';

   return output;
}
