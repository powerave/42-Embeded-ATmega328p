#include <avr/io.h>

int main() {
    DDRB |= (1 << PB1); // LED en sortie

    TCCR1A |= (1 << WGM11 | 1 << COM1A1); // Fast PWM mode)
    TCCR1B |= (1 << WGM13 | 1 << WGM12) | (1 << CS12); // Fast PWM mode et prescaler de 256

    ICR1 = F_CPU / 256UL - 1; // compare match pour une frequence de 1Hz
    OCR1A = (ICR1 / 1) / 100; // duty cycle de 10%

    while (1) {}
}
