#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdlib.h>

#define AHT20_ADDR_W 0x70 // Adresse I2C de l'AHT20 pour l'écriture
#define AHT20_ADDR_R 0x71 // Adresse I2C de l'AHT20 pour la lecture

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

void uart_printstr(char *s) {
    while (*s) uart_tx(*s++);
}

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
	TWCR = (1 << TWINT) | (1 << TWEN); // TWINT cleared by a ONE. clear it starts TWI operations,
	//TWEN enable TWI, hardware will start transmission of data
	while (!(TWCR & (1 << TWINT))); //once communication is done, TWINT is set again by hardware, we can continue
}

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
    float temperature_data[3];
    float humidity_data[3];
    float t_deg, h_rel;
    char buffer[20]; // Buffer pour la conversion en chaîne de caractères
    uint8_t data[7]; // Buffer pour stocker les 7 octets de données reçus de l'AHT20
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

            for (int i = 0; i < 6; i++) // 6 octets avec ACK
                data[i] = i2c_read(1); // Lit chaque octet reçu
            data[6] = i2c_read(0); // 1 octet NACK pour la dernière lecture
            i2c_stop();

            // Calculs (Extraction des 20 bits)
            // Humidité : Octet 1, 2 et la moitié haute de l'octet 3
            uint32_t hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
            
            // Température : Moitié basse de l'octet 3, Octet 4 et 5
            uint32_t temp_raw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
    
            h_rel = ((float)hum_raw * 100.0) / 1048576.0; // Hrel​=Srh*100 / 2^20 pour l'humidité relative
            t_deg = ((float)temp_raw * 200.0) / 1048576.0 - 50.0; // T​=St*200 / 2^20 - 50 pour la température
            if (state < 2) {
                temperature_data[state] = t_deg;
                humidity_data[state] = h_rel;
                state++;
            } else if (state >= 2) {
                temperature_data[state] = t_deg;
                humidity_data[state] = h_rel;
                t_deg = (temperature_data[0] + temperature_data[1] + temperature_data[2]) / 3.0;
                h_rel = (humidity_data[0] + humidity_data[1] + humidity_data[2]) / 3.0;
                
                uart_printstr("Temperature : ");
                dtostrf(t_deg, 6, 1, buffer); // Convertit la temperature avec 3 chiffres après la virgule
                uart_printstr(buffer); // Affiche la température convertie
                uart_printstr("C, Humidity : ");
                dtostrf(h_rel, 6, 2, buffer); // Convertit l'humidité avec 3 chiffres après la virgule
                uart_printstr(buffer); // Affiche l'humidité convertie
                uart_tx('%');
                uart_tx('\n'); // Nouvelle ligne pour séparer les mesures
                uart_tx('\r'); // Retour chariot pour séparer les mesures
                state = 0;
            }
            _delay_ms(2000); // Attente de 2 secondes avant la prochaine mesure
        }
    return 0;
}
