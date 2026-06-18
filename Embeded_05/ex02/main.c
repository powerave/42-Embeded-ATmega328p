#include <avr/io.h>
#include <util/delay.h>

volatile uint16_t adc0 = 0;
volatile uint16_t adc1 = 0;
volatile uint16_t adc2 = 0;
volatile uint8_t current_adc = 0;

void uart_tx(unsigned char c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}

void uart_tx_uint10(uint16_t val) {
    char buffer[4];

    buffer[0] = (val / 1000) % 10 + '0';
    buffer[1] = (val / 100) % 10 + '0';
    buffer[2] = (val / 10) % 10 + '0';
    buffer[3] = (val % 10) + '0';

    uint8_t i = 0;
    while (i < 3 && buffer[i] == '0') {
        buffer[i] = ' '; // Replace leading zeros with spaces
        i++;
    }
    for (uint8_t j = 0; j < 4; j++) {
        uart_tx(buffer[j]); // Print space for leading zeros
    }
}

void __vector_11() __attribute__((signal, used, externally_visible)); // ISR pour Timer1 compare match A
void __vector_11() {
    ADCSRA |= (1 << ADSC); // Start next ADC conversion

    uart_tx_uint10(adc0); // Send low bits
    uart_tx(',');
    uart_tx(' ');
    uart_tx_uint10(adc1); // Send low bits
    uart_tx(',');
    uart_tx(' ');
    uart_tx_uint10(adc2); // Send low bits
    uart_tx('\n'); // New line for better readability
    uart_tx('\r'); // Carriage return for better readability
}

void __vector_21() __attribute__((signal, used, externally_visible)); // ISR pour ADC conversion complete
void __vector_21() {
    uint16_t raw_adc = ADCL | (ADCH << 8); // Read the full 10-bit ADC result (left adjusted)
    if (current_adc == 0) {
        adc0 = raw_adc; // Store ADC0 result
        current_adc = 1; // Switch to next ADC channel
        ADMUX = (1 << REFS0) | (1 << MUX0); // Select ADC1 (light sensor)
    } else if (current_adc == 1) {
        adc1 = raw_adc; // Store ADC1 result
        current_adc = 2; // Switch to next ADC channel
        ADMUX = (1 << REFS0) | (1 << MUX1) | (0 << MUX0); // Select ADC2 (temperature sensor)
    } else if (current_adc == 2) {
        adc2 = raw_adc; // Store ADC2 result
        current_adc = 0; // Switch back to first ADC channel
        ADMUX = (1 << REFS0); // Select ADC0 (potentiometer
    }
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
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D); // Disable digital input on ADC0, ADC1 and ADC2 pins
    ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADATE); // Enable ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescaler to 128
    ADCSRB |= (1 << ADTS2) | (1 << ADTS0); // Auto trigger on Timer1 compare match A
    ADMUX |= (1 << REFS0); // Left adjust result, AVcc reference, select ADC0 (potentiometre)
    init_timer();
    uart_init();
    SREG |= (1 << 7); // Enable global interrupts
    while(1) {}
}