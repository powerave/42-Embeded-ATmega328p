#include <avr/io.h>

volatile uint8_t brightness = 0;
volatile uint8_t direction = 1; // 1 for increasing, 0 for decreasing

void __vector_14() __attribute__ ((signal, used));
void __vector_14() {
    if (direction) {
        if (brightness < 255) {
            brightness++;
        } else {
            direction = 0;
        }
    } else {
        if (brightness > 0) {
            brightness--;
        } else {
            direction = 1;
        }
    }

    OCR1A = brightness; // Duty cycle for OC1A (PB1)
}

int main(void) {
    DDRB |= (1 << DDB1); // PB1/OC1A as output

    // Timer1: 8-bit Fast PWM on OC1A (PB1), non-inverting.
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS11); // Prescaler 8
    OCR1A = 0;

    // Timer0: CTC interrupt to update brightness periodically.
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
    OCR0A = 30; // 1,016sec par cycle
    TIMSK0 = (1 << OCIE0A); // Activation Tiomer0 compare match OCR0A

    SREG |= (1 << 7); // Enable global interrupts

    while (1) {}
}