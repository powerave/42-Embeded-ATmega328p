#include <avr/io.h>
#include <util/delay.h>

volatile uint16_t current = 0;
volatile uint16_t last_timer1 = 0;
volatile uint16_t last_timer2 = 0;
volatile uint8_t i = 0;

void __vector_11() __attribute__ ((signal, used)); // Timer1 compare match A interrupt for timing
void __vector_11() {
    current++;
}

void __vector_5() __attribute__ ((signal, used)); // PCINT2 for SW2
void __vector_5() {
    uint8_t button2 = PIND & (1 << PD4); // SW2
    if (button2 == 0 && (current - last_timer1 > 20)) { // 1s at 10ms intervals
        i = (i - 1) % 16;
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Clear all LEDs
        if (i & (1 << 0))
            PORTB |= (1 << PB0); // LED1
        if (i & (1 << 1))
            PORTB |= (1 << PB1); // LED2
        if (i & (1 << 2))
            PORTB |= (1 << PB2); // LED3
        if (i & (1 << 3))
            PORTB |= (1 << PB4); // LED4
        last_timer1 = current;
    }
    PCIFR |= (1 << PCIF2); // Clear PCINT2 flag
}

void __vector_1() __attribute__ ((signal, used)); // INT0 for SW1
void __vector_1() {
    uint8_t button1 = PIND & (1 << PD2); // SW1
    if (button1 == 0 && (current - last_timer2 >= 20)) { // 1s at 10ms intervals
        i = (i + 1) % 16;
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Clear all LEDs
        if (i & (1 << 0))
            PORTB |= (1 << PB0); // LED1
        if (i & (1 << 1))
            PORTB |= (1 << PB1); // LED2
        if (i & (1 << 2))
            PORTB |= (1 << PB2); // LED3
        if (i & (1 << 3))
            PORTB |= (1 << PB4); // LED4
        last_timer2 = current;
    }
}

void init_board() {
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4); // PB0, PB1, PB2, PB4 as outputs

    DDRD &= ~((1 << DDD2) | (1 << DDD4)); // PD2 as input (SW1), PD4 as input (SW2)
    PORTD |= (1 << PD2) | (1 << PD4); // Enable pull-up resistors for PD2 and PD4
    
    EICRA |= (1 << ISC01); // Trigger INT0 on falling edge (SW1)
    EIMSK |= (1 << INT0); // Enable external interrupt INT0
    PCICR |= (1 << PCIE2); // Enable pin change interrupt for PCINT[23:16](PD0 - PD7) (SW2)
    PCMSK2 |= (1 << PD4); // Enable pin change interrupt for PD4 (SW2)

    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
    OCR1A = 155; // current++

    TIMSK1 = (1 << OCIE1A); // Enable Timer1 compare match A interrupt

    SREG |= (1 << 7); // Enable global interrupts
}

int main() {
    init_board();
    while(1) {}
}
