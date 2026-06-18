#include <avr/io.h>
#include <util/delay.h>

int main () {
    DDRD &= ~(1 << PD2); // on force le pin PD2 a 0
    PORTD |= (1 << PD2); //Activation de la pull-up

    DDRB |= (1 << PB0); // D1 est l'equivalent de PB0 
    PORTB &= ~(1 << PB0); // Eteint la LED connectee a D1
    
    uint8_t Button = 1;
    while (1) {
        uint8_t now = (PIND & (1 << PD2) ? 1 : 0);

        if (Button == 1 && now == 0) {
            _delay_ms(20); // Debounce delay
            if (!(PIND & (1 << PD2))) {
                PORTB ^= (1 << PB0); // Toggle the state of the button pin
            }
        }
        Button = now;
    }
}