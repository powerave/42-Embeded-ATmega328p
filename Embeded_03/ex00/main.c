#include <avr/io.h>
#include <util/delay.h>

void clear_color() {
    PORTD &= ~(1 << PD6); // Clear OCR0A
    PORTD &= ~(1 << PD5); // Clear OCR0B 
    PORTD &= ~(1 << PD3); // Clear OCR1A  
}

uint8_t set_colors(uint8_t cycle) {
    
    switch(cycle) {
        case 0: // Red
            clear_color();
            PORTD |= (1 << PD5); // Set PD5 for red
            cycle = 1; // Move to next color
            break;
        case 1: // Green
            clear_color();
            PORTD |= (1 << PD6); // Set PD6 for green
            cycle = 2; // Move to next color
            break;
        case 2: // Blue
            clear_color();
            PORTD |= (1 << PD3); // Set PD3 for blue
            cycle = 0; // Move to next color
            break;
    }
    return cycle;
}

int main() {
    uint8_t cycle = 0;
    DDRD |= (1 << PD6) | (1 << PD5) | (1 << PD3); // Set PD6, PD5, PD3 as output for RGB
    PORTD |= (1 << PD6) | (1 << PD5) | (1 << PD3); // Set PD6, PD5, PD3 as output for RGB
    while(1) {
        cycle = set_colors(cycle);
        _delay_ms(1000);
    }
}
