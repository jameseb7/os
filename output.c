void kprint_clr(char * str, unsigned char foreclr, unsigned char backclr){
   int i; /*string index*/
   static int j = 0; /*videoram index*/
   unsigned char * videoram = (unsigned char *) 0xB8000;
   
   for(i = 0; str[i] != '\0'; i++){
      videoram[j] = str[i];
      j++;
      videoram[j] = (backclr << 4) | (foreclr & 0x0F);
      j++;
   }
}

void write_screen(char * str,
                  unsigned char foreclr, unsigned char backclr,
                  unsigned int x, unsigned int y){
   int i; /*string index*/
   volatile unsigned char * videoram = (unsigned char *) 0xB8000;

   for(i = 0; str[i] != '\0'; i++){
      videoram[2*(80*y + x + i)] = str[i];
      videoram[2*(80*y + x + i) + 1] = (backclr << 4) | (foreclr & 0x0F);
   }
}

void kprint(char * str){
   static unsigned int row = 0;
   write_screen(str, 0x07, 0x00, 0, row);
   row++;
   if(row >= 25) row = 0;
}

