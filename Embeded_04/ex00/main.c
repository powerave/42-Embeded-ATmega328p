#include <avr/io.h>
#include <util/delay.h>

volatile uint16_t current = 0;
volatile uint16_t last_timer = 0;

void __vector_11(void) __attribute__ ((signal, used));
void __vector_11(void) {
    current++;
}

void __vector_1(void) __attribute__ ((signal, used));
void __vector_1(void) {
    if ((current - last_timer > 10)) { // 1s at 10ms intervals
        PORTB ^= (1 << PORTB0); // Toggle LED
        last_timer = current;
    }
}

int main() {
    DDRB |= (1 << DDB0); // Sortie
    DDRD &= ~(1 << DDD2); // Entrée (SW1) // Normal mode

    TCCR1A = 0;
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
    OCR1A = 249; // Compare value for 10ms at 16MHz with 1024 prescaler

    EICRA |= (1 << ISC01); // Trigger INT0 on falling edge
    EIMSK |= (1 << INT0); // Enable external interrupt INT0

    TIMSK1 = (1 << OCIE1A); // Enable Timer1 compare match A interrupt
    SREG |= (1 << 7); // Enable global interrupts
    while(1) {}
}