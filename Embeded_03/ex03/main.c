#include <avr/io.h>
#include <util/delay.h>

#define BAUD 115200UL

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 16;

    UCSR0A = (1 << U2X0);


    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // 8 data bits, no parity, 1 stop bit
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait until buffer is empty
    UDR0 = c; // Send character
}

void uart_tx_string(const char* str) {
    while (*str) {
        uart_tx(*str++);
    }
}

char uart_rx() {
    while (!(UCSR0A & (1 << RXC0))); // Wait until data is received
    return UDR0; // Read received character
}

uint8_t hexa_to_int(char c) {
    if (c >= '0' && c <= '9') {
            return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;       
}

uint8_t is_hex(char c) {
    return ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'));
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    if (r == 0)
        DDRD &= ~(1 << PD5);
    else {
        DDRD |= (1 << PD5);
        OCR0A = r;   // Set red intensity
    }
    if (g == 0)
        DDRD &= ~(1 << PD6);
    else {
        DDRD |= (1 << PD6);
        OCR0B = g;   // Set green intensity
    }
    if (b == 0)
        DDRD &= ~(1 << PD3);
    else {
        DDRD |= (1 << PD3);
        OCR2B = b;   // Set blue intensity
    }
}

uint8_t hexa_pair_to_int(char high, char low) {
    return (hexa_to_int(high) << 4) | hexa_to_int(low);
}



void __vector_18() __attribute__((signal, used));
void __vector_18() {
    static uint8_t cycle = 0;
    static char r1, r2, g1, g2, b1, b2;
    char c = UDR0;
    
    if (cycle == 0) {
        if (c == '#') {
            uart_tx(c); // Echo received character back to sender
            cycle = 1;
        }
        return;
    }

    if (!is_hex(c)) {
        cycle = 0;
        uart_tx_string("\r\nInvalid input. Please enter a valid hex color code starting with '#'.\r\n");
        return;
    }

    if (cycle == 1) {
        r1 = c;
        cycle++;
        uart_tx(c);
    }
    else if (cycle == 2) {
        r2 = c;
        cycle++;
        uart_tx(c);
    }
    else if (cycle == 3) {
        g1 = c;
        cycle++;
        uart_tx(c);
    }
    else if (cycle == 4) {
        g2 = c;
        cycle++;
        uart_tx(c);
    }
    else if (cycle == 5) {
        b1 = c;
        cycle++;
        uart_tx(c);
    }
    else if (cycle == 6) {
        b2 = c;
        cycle = 0;
        uart_tx(c);
        set_rgb(hexa_pair_to_int(r1, r2), hexa_pair_to_int(g1, g2), hexa_pair_to_int(b1, b2));
        uart_tx_string("\r\n");
    }
}

static inline void local_sei() {
    __asm__ volatile ("sei" ::: "memory");
}

void init_rgb() {
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01); // Fast PWM mode, non-inverting
    TCCR0B = (1 << CS00); // No prescaling
    TCCR2A = (1 << COM2B1) | (1 << WGM20) | (1 << WGM21); // Fast PWM mode, non-inverting
    TCCR2B = (1 << CS20); // No prescaling

    DDRD &= ~((1 << PD6) | (1 << PD5) | (1 << PD3)); // Set PD6, PD5, PD3 as output for RGB
}

int main() {
    uart_init();
    init_rgb();
    local_sei();
    while(1) {}
}

