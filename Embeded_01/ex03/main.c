#include <avr/io.h>
#include <util/delay.h>

int main() {
    uint8_t cycle = 10; // duty cycle de 10%
    uint8_t Button = 1;
    uint8_t Button2 = 1;

    DDRB |= (1 << PB1); // LED en sortie

    DDRD &= ~(1 << PD2); // on force le pin PD2 a 0
    PORTD |= (1 << PD2); //Activation de la pull-up
    DDRD &= ~(1 << PD4); // on force le pin PD4 a 0
    PORTD |= (1 << PD4); //Activation de la pull-up

    TCCR1A |= (1 << WGM11 | 1 << COM1A1); // Fast PWM mode)
    TCCR1B |= (1 << WGM13 | 1 << WGM12) | (1 << CS12); // Fast PWM mode et prescaler de 256

    ICR1 = F_CPU / 256UL - 1; // compare match pour une frequence de 1Hz
    OCR1A = (ICR1 * cycle) / 100; // duty cycle de 10%

    while (1) {
        uint8_t now = (PIND & (1 << PD2) ? 1 : 0);
        uint8_t now2 = (PIND & (1 << PD4) ? 1 : 0);

        if (Button == 1 && now == 0) {
            _delay_ms(20); // Debounce delay
            if (!(PIND & (1 <<PD2))) { // Check if button is still pressed
                cycle += 10;
                if (cycle > 100)
                    cycle = 10; // Reset to 10% if it goes above 100%
                OCR1A = ((uint32_t)ICR1 * cycle) / 100; // Update duty cycle
            }
        }
        if (Button2 == 1 && now2 == 0) {
            _delay_ms(20); // Debounce delay
            if (!(PIND & (1 << PD4))) { // Check if button is still pressed
                cycle -= 10;
                if (cycle < 10)
                    cycle = 100; // Reset to 100% if it goes below 10%
                OCR1A = ((uint32_t)ICR1 * cycle) / 100; // Update duty cycle
            }
        }
        Button = now;
        Button2 = now2;
    }
}