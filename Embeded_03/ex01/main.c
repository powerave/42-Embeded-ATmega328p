#include <avr/io.h>
#include <util/delay.h>

void clear_color() {
    OCR0A = 0; // Clear red
    OCR0B = 0; // Clear green
    OCR2B = 0; // Clear blue  
}

uint8_t set_colors(uint8_t cycle) {
    
    switch(cycle) {
        case 0: // Red
            clear_color();
            OCR0A = 0; // Set red to maximum
            OCR0B = 255;   // Set green to minimum
            OCR2B = 0;   // Set blue to minimum
            cycle = 1; // Move to next color
            break;
        case 1: // Green
            clear_color();
            OCR0A = 255;   // Set red to minimum
            OCR0B = 0; // Set green to maximum
            OCR2B = 0;   // Set blue to minimum
            cycle = 2; // Move to next color
            break;
        case 2: // Blue
            clear_color();
            OCR0A = 0;   // Set red to minimum
            OCR0B = 0;   // Set green to minimum
            OCR2B = 255; // Set blue to maximum
            cycle = 3; // Move to next color
            break;
        case 3: // Yellow
            clear_color();
            OCR0A = 255; // Set red to maximum
            OCR0B = 255; // Set green to maximum
            OCR2B = 0;   // Set blue to minimum
            cycle = 4; // Move to next color
            break;
        case 4: // Cyan
            clear_color();
            OCR0A = 255;   // Set red to minimum
            OCR0B = 0; // Set green to maximum
            OCR2B = 255; // Set blue to maximum
            cycle = 5; // Move to next color
            break;
        case 5: // Magenta
            clear_color();
            OCR0A = 0; // Set red to maximum
            OCR0B = 255;   // Set green to minimum
            OCR2B = 255; // Set blue to maximum
            cycle = 6; // Move to next color
            break;
        case 6: // White
            clear_color();
            OCR0A = 255; // Set red to maximum
            OCR0B = 255; // Set green to maximum
            OCR2B = 255; // Set blue to maximum
            cycle = 0; // Move to next color
            break;
    }
    return cycle;
}

int main() {
    uint8_t cycle = 0;

    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01); // Fast PWM mode, non-inverting
    TCCR0B = (1 << CS00); // No prescaling
    TCCR2A = (1 << COM2B1) | (1 << WGM20) | (1 << WGM21); // Fast PWM mode, non-inverting
    TCCR2B = (1 << CS20); // No prescaling

    DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3); // Set PD6, PD5, PD3 as output for RGB
    while(1) {
        cycle = set_colors(cycle);
        _delay_ms(1000);
    }
}
