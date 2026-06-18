#include <avr/io.h>
#include <util/delay.h>

void uart_tx(unsigned char c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}
void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r; // Red on OC0B (PD5)
    OCR0A = g; // Green on OC0A (PD6)
    OCR2B = b; // Blue on OC2B (PD3)
}

void wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85)
        set_rgb(255 - pos * 3, 0, pos * 3);
    else if (pos < 170)
    {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    }
    else
    {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

void __vector_11() __attribute__((signal, used, externally_visible)); // ISR pour Timer1 compare match A
void __vector_11() {
    ADCSRA |= (1 << ADSC); // Start next ADC conversion
}

void __vector_21() __attribute__((signal, used, externally_visible)); // ISR pour ADC conversion complete
void __vector_21() {
    uint8_t nbr = ADCH; // Read the high byte of the ADC result (left adjusted)
    // uart_tx("0123456789ABCDEF"[(nbr & 0xF0) >> 4]); // Send high bits
    // uart_tx("0123456789ABCDEF"[nbr & 0x0F]); // Send low bits
    uart_tx('\n'); // New line for better readability
    uart_tx('\r'); // Carriage return for better readability

    wheel(nbr);

	if (nbr == 255)
		PORTB = (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB4);
	else if (nbr >= 192)
		PORTB = (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2);
	else if (nbr >= 128)
		PORTB = (1 << PORTB0) | (1 << PORTB1);
	else if (nbr >= 64)
		PORTB = (1 << PORTB0);
	else
		PORTB = 0;
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




void init_leds() {
    DDRD |= (1 << DDD6) | (1 << DDD5) | (1 << DDD3); // Set PD6, PD5, PD3 as output for RGB LEDs
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4); // Set PB0, PB1, PB2, PB4 as output for RGB LEDs
    PORTD &= ~((1 << PD6) | (1 << PD5) | (1 << PD3)); // Turn off RGB LEDs
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // Turn off RGB LEDs

    // Configure Timer0 for Red (PD5) and Green (PD6) using Fast PWM mode
    TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);
    TCCR0B |= (1 << CS00); // No prescaling
    // Configure Timer2 for Blue (PD3) using Fast PWM mode
    TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2B1);
    TCCR2B |= (1 << CS20); // No prescaling
}

int main() {
    init_leds();
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