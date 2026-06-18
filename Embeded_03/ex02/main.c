#include <avr/io.h>
#include <util/delay.h>

void init_rgb() {
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01); // Fast PWM mode, non-inverting
    TCCR0B = (1 << CS00); // No prescaling
    TCCR2A = (1 << COM2B1) | (1 << WGM20) | (1 << WGM21); // Fast PWM mode, non-inverting
    TCCR2B = (1 << CS20); // No prescaling

    DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3); // Set PD6, PD5, PD3 as output for RGB
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    OCR0A = g;   // Set green intensity
    OCR0B = r;   // Set red intensity
    OCR2B = b;   // Set blue intensity
}


void wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        set_rgb(255 - pos * 3, 0, pos * 3);
    } else if (pos < 170) {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    } else {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

int main() {
    init_rgb();
    uint8_t pos = 0;
    while(1) {
        wheel(pos);
        pos++;
        _delay_ms(20);
    }
}