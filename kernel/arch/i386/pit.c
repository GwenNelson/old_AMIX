#include <stdint.h>
#include <stddef.h>

#include <kernel/arch/pit.h>

void init_pit() {
outb(0x43, 0x36);

    uint32_t divisor = 1193182 / 200;
    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);

}
