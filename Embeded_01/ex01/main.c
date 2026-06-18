#include <avr/io.h>

int main() {
    DDRB |= (1 << PB1); // LED en sportie

    // Configuration du timer1 pour une frequence de 1Hz environ (1049ms)
    TCCR1A = (1 << COM1A0); // Toggle lors du compare match sur OC1A (PB1)
    TCCR1B = (1 << WGM12) | (1 << CS12); // CTC mode (reinit a 0 du timer a chgaque compare match) et prescaler de 256

    OCR1A = (F_CPU / (2UL * 256UL)) - 1; // compare match pour une frequence de 1Hz

    while (1) {}
}

