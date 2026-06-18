#include <avr/io.h>
#include <util/twi.h>

void uart_init() {
    UCSR0A |= (1 << U2X0); // Enable double speed mode
    
    UBRR0H = 0;
    UBRR0L = 16;
    
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, no parity, 1 stop bit
}

void uart_tx(unsigned char c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}

void print_status() {
    uint8_t status = TW_STATUS; // (TWSR & 0xF8) to get the status code without the prescaler bits
    uart_tx('0'); // Send '0' character over UART
    uart_tx('x'); // Send 'x' character over UART
    uart_tx("0123456789ABCDEF"[(status & 0xF0) >> 4]); // Send the upper 4 bits of the status as a hexadecimal character
    uart_tx("0123456789ABCDEF"[status & 0x0F]); // Send the lower 4 bits of the status as a hexadecimal character
    uart_tx('\n'); // Newline for better readability
    uart_tx('\r'); // Carriage return for better readability
}

void i2c_init() {
    // Set SCL frequency to 100kHz
    TWSR = 0; // Prescaler value of 1
    TWBR = 72; // SCL frequency = F_CPU / (16 + 2 * TWBR * prescaler)
}

void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    print_status(); // Print the status code after sending the START condition
}

void i2c_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1<<TWSTO)); 
}

int main() {
    uart_init();
    i2c_init();
    i2c_start();
    i2c_stop();
    while(1){}
    return 0;
}
