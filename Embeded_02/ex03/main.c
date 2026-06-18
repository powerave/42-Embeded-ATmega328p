#include <avr/io.h>

void uart_init() {
    UCSR0A = (1 << U2X0); // On double la vitesse de transmission

    // UBRR decoupe les 12 bits en 4 + 8 et les reassemble ensuite (comme pour 12,80e il prends 12 et 80 en binaire et les reassemble pour faire 12,80e)
    UBRR0H = 0; // bits forts
    UBRR0L = 16; // bits faibles (pour 115200 bauds à 16MHz)

    UCSR0B = (1 << RXEN0) | (1 << TXEN0 | (1 << RXCIE0)); // On active la reception et la transmission + interruption de reception

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Format des trames : 8 bits de données, 1 bit de stop, pas de parité
}

void uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}

void __vector_18(void) __attribute__((signal, used, externally_visible)); // ISR pour reception UART
void __vector_18(void) {
    uint8_t c = UDR0; // On lit le caractère reçu
    if (c <= 'Z' && c >= 'A') {
        c += 32;
    } else if (c <= 'z' && c >= 'a') {
        c -= 32;
    }
    uart_tx(c);
}

int main() {
    TCCR1A = 0; // Mode CTC
    TCCR1B = (1 << WGM12) | (1 << CS12);
    OCR1A = 62499; // Pour une interruption toutes les secondes à 16MHz avec un prescaler de 256
    uart_init();
    SREG |= (1 << 7); // Enable global interrupts
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt
    while (1) {}
}