#include <avr/io.h>
#include <util/delay.h>

void uart_tx(unsigned char c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}

void __vector_11() __attribute__((signal, used, externally_visible)); // ISR pour Timer1 compare match A
void __vector_11() {
    ADCSRA |= (1 << ADSC); // Start next ADC conversion
}

void __vector_21() __attribute__((signal, used, externally_visible)); // ISR pour ADC conversion complete
void __vector_21() {
    uint8_t nbr = ADCH; // Read the high byte of the ADC result (left adjusted)
    uart_tx("0123456789ABCDEF"[(nbr & 0xF0) >> 4]); // Send high bits (0000 1111)
    uart_tx("0123456789ABCDEF"[nbr & 0x0F]); // Send low bits (1111 0000)
    uart_tx('\n'); // New line for better readability
    uart_tx('\r'); // Carriage return for better readability
}

void uart_init() {
    UCSR0A |= (1 << U2X0); // Enable double speed mode
    
    UBRR0H = 0;
    UBRR0L = 16;
    
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, no parity, 1 stop bit
}

void init_timer() {
    OCR1A = 312; // Set compare value for Timer1
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare match A interrupt
}

int main() {
    DIDR0 |= (1 << ADC0D); // Disable digital input on ADC0 pin
    ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADATE); // Enable ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescaler to 128
    ADCSRB |= (1 << ADTS2) | (1 << ADTS0); // Auto trigger on Timer1 compare match A
    ADMUX |= (1 << ADLAR) | (1 << REFS0); // Left adjust result, AVcc reference, select ADC0 (potentiometre)
    init_timer();
    uart_init();
    SREG |= (1 << 7); // Enable global interrupts
    while(1) {}
}