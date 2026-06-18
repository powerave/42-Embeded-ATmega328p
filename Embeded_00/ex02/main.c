#include <avr/io.h>
#include <util/delay.h>

int main () {
    DDRD &= ~(1 << PD2); // on met le pin PD2 a 1
    PORTD |= (1 << PD2); //Activation de la pull-up
    while (1) {
        if (PIND & (1 << PD2)) {
            DDRB &= ~(1 << PB0); // D1 est l'equivalent de PB0 
            PORTB &= ~(1 << PB0); // Eteint la LED connectee a D1
        } else {
            DDRB |= (1 << PB0); // D1 est l'equivalent de PB0 
            PORTB |= (1 << PB0); // Allume la LED connectee a D1
        }
    }
}