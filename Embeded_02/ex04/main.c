#include <avr/io.h>
#include <util/delay.h>

#define USERNAME "username"
#define PASSWORD "password"

#define USERNAME_MSG_LEN 8
#define PWD_MSG_LEN 16

#define USR_INPUT 0b00000001
#define PWD_INPUT 0b00000010
#define UP_VALID  0b00000100

#define NL "\r\n"
#define LOGIN_MSG "Enter your login: " NL
#define USR_MSG "\tusername: "
#define PWD_MSG "\tpassword: "

#define SUCCESS_MSG "Login successful!" NL
#define FAILURE_MSG "Bad combination Username/Password!" NL

volatile uint8_t state = USR_INPUT;
volatile uint8_t usr_idx = 0;
volatile uint8_t pwd_idx = 0;
volatile char username[USERNAME_MSG_LEN + 1] = {0};
volatile char password[PWD_MSG_LEN + 1] = {0}; 

void uart_init() {
    UCSR0A = (1 << U2X0); // On double la vitesse de transmission
    uint16_t ubrr = F_CPU / (8UL * 115200) - 1; // Calcul de la valeur UBRR pour 115200 bauds à 16MHz
    // UBRR decoupe les 12 bits en 4 + 8 et les reassemble ensuite (comme pour 12,80e il prends 12 et 80 en binaire et les reassemble pour faire 12,80e)
    UBRR0H = ubrr >> 8; // bits forts
    UBRR0L = ubrr; // bits faibles

    UCSR0B = (1 << RXEN0) | (1 << TXEN0 | 1 << RXCIE0); // On active la reception et la transmission + interruption de reception
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Format des trames : 8 bits de données, 1 bit de stop, pas de parité
}
void uart_tx(uint8_t c) {
    while (!(UCSR0A & (1 << UDRE0))); // On attend que le buffer de transmission soit vide
    UDR0 = c; // On envoie le caractère
}
void uart_tx_string(const char* str) {
    while (*str) {
        uart_tx(*str++);
    }
}

uint8_t uart_rx() {
    while (!(UCSR0A & (1 << RXC0))); // On attend que le buffer de réception soit plein
    return UDR0; // On retourne le caractère reçu
}

static char	local_strcmp(volatile const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

void uart_login() {
    uint8_t i = 0;
    while (i <= USERNAME_MSG_LEN)
        username[i++] = '\0';
    i = 0;
    while (i <= PWD_MSG_LEN)
        password[i++] = '\0';
    usr_idx = 0;
    pwd_idx = 0;
    uart_tx_string(LOGIN_MSG);
    uart_tx_string(USR_MSG);
    state = USR_INPUT;
}

void handle_backspace() {
    if (usr_idx > 0) {
        usr_idx--;
        uart_tx_string("\b \b"); // Efface le caractère à l'écran
    } else if (pwd_idx > 0) {
        pwd_idx--;
        uart_tx_string("\b \b"); // Efface le caractère à l'écran
    }
}

void handle_enter() {
    if (state & USR_INPUT) {
            uart_tx_string(NL);
            state = PWD_INPUT;
            uart_tx_string(PWD_MSG);
        }else if (state & PWD_INPUT) {
            if ((local_strcmp(password, PASSWORD) == 0) && (local_strcmp(username, USERNAME) == 0)) {
            uart_tx_string(NL);
            uart_tx_string(SUCCESS_MSG);
            state = UP_VALID;
        } else {
            uart_tx_string(NL);
            uart_tx_string(FAILURE_MSG);
            uart_login(); // Recommence le processus de login
        }
    }
}

void fill_username(uint8_t c) {
    if (c == 0x08 || c == 0x7F) {
        handle_backspace();
    } else if (c == '\r' || c == '\n') {
        handle_enter();
    } else if (usr_idx < USERNAME_MSG_LEN) {
        username[usr_idx++] = c;
        uart_tx(c); // Affiche le caractère à l'écran
    }
}

void fill_password(uint8_t c) {
    if (c == 0x08 || c == 0x7F) {
        handle_backspace();
    } else if (c == '\r' || c == '\n') {
        handle_enter();
    } else if (pwd_idx < PWD_MSG_LEN) {
        password[pwd_idx++] = c;
        uart_tx('*'); // Affiche un '*' à l'écran pour masquer le mot de passe
    }
}

static inline void local_sei() {
    __asm__ volatile ("sei" ::: "memory");
}

void __vector_18(void) __attribute__((signal, used, externally_visible)); // ISR pour reception UART
void __vector_18(void) {
    uint8_t c = uart_rx();
    if (c == 0x03) { // Ctrl+C pour réinitialiser le login
        uart_tx_string(NL);
        uart_login();
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2)); // Eteint les LEDs
        return;
    }
    if (state & USR_INPUT) {
        fill_username(c);
    } else if (state & PWD_INPUT) {
        fill_password(c);
    }
}

int main() {
    uint8_t i;
    
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2); // Configure les pins PB0, PB1 et PB2comme sorties (TX et RX)
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2)); // Assure que les LEDs sont éteintes au départ
    uart_init();
    uart_login();
    local_sei(); // Active les interruptions
    
    i = 0;
    while(1){
        if (state & UP_VALID) {
            // Clignote les LEDs pour indiquer que le login est validé
            PORTB ^= (1 << PB0) | (1 << PB1) | (1 << PB2); // Toggle les LEDs
            _delay_ms(500); // Attends 500ms
        }
    }
}