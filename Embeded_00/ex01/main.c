#include <avr/io.h>

int main() {
    DDRB |= (1 << PB0); // D1 est l'equivalent de PB0 
    PORTB |= (1 << PB0); // Allume la LED connectee a D1
    while (1) {}
}