#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <kernel/arch/portio.h>
#include <kernel/printf.h>

#define PORT_COM1 0x3F8
#define PORT_COM2 0x2F8
#define PORT_COM3 0x3E8
#define PORT_COM4 0x2E8

static uint16_t *video_memory = (uint16_t *)0xC03FF000;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void move_cursor() {
     uint16_t cursorLocation = cursor_y * 80 + cursor_x;
     outb(0x3D4, 14);
     outb(0x3D5, cursorLocation >> 8);
     outb(0x3D4, 15);
     outb(0x3D5, cursorLocation);
}

static void scroll() {
    uint8_t  attributeByte = (0 << 4) | (15 & 0x0F);
    uint16_t blank         = 0x20     | (attributeByte << 8);

    if(cursor_y >= 25) {
       int i;
       for (i = 0*80; i < 24*80; i++) {
           video_memory[i] = video_memory[i+80];
       }
       for (i = 24*80; i < 25*80; i++) {
            video_memory[i] = blank;
       }
       cursor_y = 24;
    }
}

void console_put(char c) {
   	uint8_t  backColour = 0;
     uint8_t  foreColour = 15;
     uint8_t  attributeByte = (backColour << 4) | (foreColour & 0x0F);
     uint16_t attribute = attributeByte << 8;
     uint16_t *location;

     if (c == 0x08 && cursor_x) {  // backspace
        cursor_x--;
        move_cursor();
        console_put(0x20);
        cursor_x--;
     } else if (c == 0x09) {       // tab
        cursor_x = (cursor_x+8) & ~(8-1);
     } else if (c == '\r') {       // carriage return
        cursor_x = 0;
     } else if (c == '\n') {       // newline
        cursor_x = 0;
        cursor_y++;
     } else if(c >= ' ') {         // all other printables
        location = video_memory + (cursor_y*80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
     } 

     // Check if we need to insert a new line because we have reached the end
     // of the screen.
     if (cursor_x >= 80) {
         cursor_x = 0;
         cursor_y ++;
     }
        scroll();
        move_cursor();
}

static int is_ser_transmit_empty(uint16_t port) {
    return inb(port + 5) & 0x20;
}

static void console_write(char *c) {
     int i=0;
     char o;
     while (c[i]) {
        o=c[i++];
     	console_put(o);
        while(is_ser_transmit_empty(PORT_COM1) == 0);
        outb(PORT_COM1,o);
     }
}

bool is_init=false;

static void init_early_kprintf() {
     uint16_t pos=0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    cursor_y = pos / 25;
    cursor_x = pos % 80;
    is_init = true;
    console_write("\n\ndebug output ready\n");
}

int kprintf(const char *fmt, ...)
{
	if(!is_init) init_early_kprintf();
	char buf[1024];
        va_list args;
        int i;


        va_start(args, fmt);
        i=vsnprintf(buf,1024,fmt,args);
        va_end(args);
	console_write(buf);
	return i;
}


