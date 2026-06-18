#include <avr/io.h>

void uart_init() {
    UCSR0A = (1 << U2X0); // On double la vitesse de transmission

    // UBRR decoupe les 12 bits en 4(11 - 8) + 8(7 - 0) et les reassemble ensuite (comme pour 12,80e il prends 12 et 80 en binaire et les reassemble pour faire 12,80e) 
    UBRR0H = 0; // bits forts 
    UBRR0L = 16; // bits faibles (pour 115200 bauds à 16MHz)

    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // On active la reception et la transmission

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Format des trames : 8 bits de données, 1 bit de stop, pas de parité (8N1 souvent par défaut)
}

uint8_t uart_rx() {
    while (!(UCSR0A & (1 << RXC0))); // On attend que le buffer de réception soit plein
    return UDR0; // On retourne le caractère reçu
}

void uart_tx(uint8_t c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}

int main() {
    uart_init();
    while (1) {
        uart_tx(uart_rx()); 
    }
}