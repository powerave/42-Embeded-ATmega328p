#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#define AHT20_ADDR_W 0x70 // Adresse I2C de l'AHT20 pour l'écriture
#define AHT20_ADDR_R 0x71 // Adresse I2C de l'AHT

volatile uint8_t state = 0;

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

// void print_status() {
//     uint8_t status = TW_STATUS; // (TWSR & 0xF8) to get the status code without the prescaler bits
//     uart_tx('0'); // Send '0' character over UART
//     uart_tx('x'); // Send 'x' character over UART
//     uart_tx("0123456789ABCDEF"[(status & 0xF0) >> 4]); // Send the upper 4 bits of the status as a hexadecimal character
//     uart_tx("0123456789ABCDEF"[status & 0x0F]); // Send the lower 4 bits of the status as a hexadecimal character
//     uart_tx('\n'); // Newline for better readability
//     uart_tx('\r'); // Carriage return for better readability
// }

void i2c_init() {
    // Set SCL frequency to 100kHz
    TWSR = 0; // Prescaler value of 1
    TWBR = 72; // SCL frequency = F_CPU / (16 + 2 * TWBR * prescaler)
}

void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
  //  print_status(); // Print the status code after sending the START condition
}

void i2c_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1<<TWSTO)); 
}

void i2c_write(unsigned char data) //send data to slave
{
	TWDR = data; // Load data into TWDR register, data to send
	TWCR = (1<<TWINT)|(1<<TWEN); // TWINT cleared by a ONE. clear it starts TWI operations,
	//TWEN enable TWI, hardware will start transmission of data
	while (!(TWCR & (1<<TWINT))); //once communication is done, TWINT is set again by hardware, we can continue
}

// void i2c_read() {
//     TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); //enable TWI and clear TWINT to start reception, also set TWEA to send ACK after reception
//     while (!(TWCR & (1<<TWINT))); //wait until reception is complete
//     unsigned char data = TWDR; //read the received data from TWDR
//     uart_tx(data); //send the received data over UART for debugging
// }

void print_hex_value(char c) {
    uart_tx("0123456789ABCDEF"[(c & 0xF0) >> 4]); // Send the upper 4 bits of the character as a hexadecimal character
    uart_tx("0123456789ABCDEF"[c & 0x0F]); // Send the lower 4 bits of the character as a hexadecimal character
    uart_tx(' '); // Space for better readability
}

void aht20_init() {
    _delay_ms(40); // Préconisation : 40ms après power-on
    i2c_start();
    i2c_write(AHT20_ADDR_W);
    i2c_write(0xBE); // Commande initialisation
    i2c_write(0x08); // Calibration enable
    i2c_write(0x00); // Reserved (obligatoion d'avoir 3 octets de données)
    i2c_stop();
    _delay_ms(10); // Préconisation : 10ms après init
}

// Ajout d'un paramètre ack (1 pour continuer, 0 pour arrêter la lecture)
unsigned char i2c_read(uint8_t ack) {
    if (ack) 
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    else 
        TWCR = (1 << TWINT) | (1 << TWEN); // NACK pour dernier octet
        
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

int main() {
    uart_init();
    i2c_init();
    aht20_init();

    while(1){
            i2c_start();
            i2c_write(AHT20_ADDR_W);
            i2c_write(0xAC); // Commande mesure
            i2c_write(0x33); // Trigger measurement
            i2c_write(0x00); // Reserved (obligatoion d'avoir 3 octets de données)
            i2c_stop();

            _delay_ms(80); // Préconisation : 80ms après trigger

            // Lecture des données de mesure (Status + 6 octets de données)
            i2c_start();
            i2c_write(AHT20_ADDR_R);

            for (int i = 0; i < 6; i++) { // 6 octets avec ACK
                print_hex_value(i2c_read(1)); // Affiche la valeur hexadécimale de chaque octet reçu
            }
            print_hex_value(i2c_read(0)); // 1 octet NACK pour la dernière lecture

            _delay_ms(2000); // Attente de 2 secondes avant la prochaine mesure
            uart_tx('\n'); // Nouvelle ligne pour séparer les mesures
            uart_tx('\r'); // Retour chariot pour séparer les mesures
            i2c_stop();
        }
    return 0;
}
