#include <avr/io.h>

int main() {
    DDRB |= (1 << PB1); // LED en sortie
    PORTB &= ~(1 << PB1); // LED eteinte

    uint32_t loopsCount = 0; // mode normal
    while (1) {
        loopsCount++;
        uint32_t bit_timer = (loopsCount >> 19) & 1; // *19bits* pour une frequence de 1Hz environ (1049ms)
        PORTB ^= (bit_timer << PB1); // bascule de l'etat de la LED
    }
}