#include <avr/io.h>
#include <util/delay.h>

int main () {
    int i = 0;
    DDRD &= ~(1 << PD2); // on force le pin PD2 a 0
    PORTD |= (1 << PD2); //Activation de la pull-up
    DDRD &= ~(1 << PD4); // on force le pin PD2 a 0
    PORTD |= (1 << PD4); //Activation de la pull-up

    DDRB |= (1 << PB0); // D1 est l'equivalent de PB0 
    PORTB &= ~(1 << PB0); // Eteint la LED1
    DDRB |= (1 << PB1); // led2
    PORTB &= ~(1 << PB1); // eteint la led2
    DDRB |= (1 << PB2); // led3
    PORTB &= ~(1 << PB2); // Eteint la LED3
    DDRB |= (1 << PB4); // led4
    PORTB &= ~(1 << PB4); // Eteint la LED4

    uint8_t Button = 1;
    uint8_t Button2 = 1;
    while (1) {
        uint8_t now = (PIND & (1 << PD2) ? 1 : 0);
        uint8_t now2 = (PIND & (1 << PD4) ? 1 : 0);

        if (Button2 == 1 && now2 == 0) {
            _delay_ms(20); // Debounce delay
            if (!(PIND & (1 << PD4))) {
                i = (i - 1) % 16; // Decrement i and wrap around using modulo
                PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Eteint toutes les LEDs
                if (i & (1 << 0))
                    PORTB |= (1 << PB0); // Allume LED1 si le bit 0 de i est 1
                if (i & (1 << 1))
                    PORTB |= (1 << PB1); // Allume LED2 si le bit 1 de i est 1
                if (i & (1 << 2))
                    PORTB |= (1 << PB2); // Allume LED3 si le bit 2 de i est 1
                if (i & (1 << 3))
                    PORTB |= (1 << PB4); // Allume LED4 si le bit 3 de i est 1
            }
        }
        if (Button == 1 && now == 0) {
            _delay_ms(20); // Debounce delay
            if (!(PIND & (1 << PD2))) {
                i = (i + 1) % 16;
                PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Eteint toutes les LEDs
                if (i & (1 << 0))
                    PORTB |= (1 << PB0); // Allume LED1 si le bit 0 de i est 1
                if (i & (1 << 1))
                    PORTB |= (1 << PB1); // Allume LED2 si le bit 1 de i est 1
                if (i & (1 << 2))
                    PORTB |= (1 << PB2); // Allume LED3 si le bit 2 de i est 1
                if (i & (1 << 3))
                    PORTB |= (1 << PB4); // Allume LED4 si le bit 3 de i est 1
            }
        }
        Button = now;
        Button2 = now2;
    }
}