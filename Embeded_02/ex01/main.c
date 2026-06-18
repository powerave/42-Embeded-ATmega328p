#include <avr/io.h>

void uart_init() {
    UCSR0A = (1 << U2X0); // On double la vitesse de transmission

    // UBRR decoupe les 12 bits en 4 + 8 et les reassemble ensuite (comme pour 12,80e il prends 12 et 80 en binaire et les reassemble pour faire 12,80e) 
    UBRR0H = 0; // bits forts 
    UBRR0L = 16; // bits faibles (pour 115200 bauds à 16MHz)

    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // On active la reception et la transmission

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Format des trames : 8 bits de données, 1 bit de stop, pas de parité (8N1 souvent par défaut)
}

void uart_putstr(const char *str) {
    while (*str) {
        while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
        UDR0 = *str; // On envoie le caractère
        str++; // On passe au caractère suivant
    }
}

void __vector_11(void) __attribute__((signal, used, externally_visible)); // ISR pour Timer1 compare match A
void __vector_11(void) {
    uart_putstr("Hello, World!\r\n");
}

int main() {
    TCCR1B = (1 << WGM12) | (1 << CS12); //CTC mode, prescaler de 256
    OCR1A = 62499; // Pour une interruption toutes les secondes à 16MHz avec un prescaler de 256
    uart_init();
    SREG |= (1 << 7); // Enable global interrupts
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt
    while (1) {}
}