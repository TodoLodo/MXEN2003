#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

// Define USART constants for ATmega328, see ATmega328P datasheet, pg 145
#define USART_BAUDRATE 9600
#define F_CPU 16000000
#define BAUD_PRESCALE ((((F_CPU / 16) + (USART_BAUDRATE / 2)) / (USART_BAUDRATE)) - 1)

/************************************************************************
Initialise USART 0
See ATmega328P datasheet for register descriptions, pg 159
Input: None
Output: None
************************************************************************/
void serial0_init(void)
{
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Enable bits for transmit and recieve
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // Use 8- bit character sizes
    UBRR0 = BAUD_PRESCALE;                  // Load baud rate prescaler into register
}

/************************************************************************
Print string via USART 0
See ATmega328P datasheet for register descriptions, pg 159
Input:      string_pointer      char array      string to be printed to serial 0
Output:     None
************************************************************************/
void serial0_print_string(char *string_pointer)
{
    while (*string_pointer) // While not null character (end of string)
    {
        while ((UCSR0A & (1 << UDRE0)) == 0)
        {
        }                       // Wait for register empty flag
        UDR0 = *string_pointer; // Send what's at the string pointer to serial data register
        string_pointer++;       // Increment string pointer to go to next letter in string
    }
}

/************************************************************************
Test serial sprinting accross USART 0
    Will initialise USART 0 if not initialised
Input: None
Output: None
************************************************************************/
void test_print(void)
{
    if (UBRR0 != BAUD_PRESCALE) // Check USART prescale set
    {
        // Run initialisation if not set and print test string
        serial0_init();
        serial0_print_string("Serial 0 was not initialised. Now initialised and working");
    }
    else
    {
        // Print test string
        serial0_print_string("Serial 0 is working");
    }
}

// Define global variable
volatile uint32_t microseconds = 0;

/************************************************************************
Initialise microsecond timer using timer 1
    Increments in 1000 microsecond intervals
    Tracks microseconds using counter register
Input: None
Output: None
************************************************************************/
void micros_init(void)
{
    cli();                   // Disable global interrupts
    TCCR1A = 0;              // No pin outputs required
    TCCR1B = (1 << WGM12);   // Set CTC mode
    TCNT1 = 0;               // Reset timer counter
    OCR1A = 15999;           // Set comparison register A for 1000 microsecond intervals
    TIMSK1 |= (1 << OCIE1A); // Set Output Compare Interrupt Enable 1 A
    TCCR1B |= (1 << CS10);   // Set prescaler to 1, starting timer
    sei();                   // Enable global interrupts
}

/************************************************************************
Reset microseconds timer
    Reset counter register and variable
Input: None
Output: None
************************************************************************/
void micros_reset(void)
{
    uint8_t oldSREG = SREG;
    TCNT1 = 0;
    microseconds = 0;
    SREG = oldSREG;
}

/************************************************************************
Returns the current microseconds count
Input: None
Output: uint32_t microseconds
************************************************************************/
uint32_t micros_now(void)
{
    uint32_t m;
    uint8_t oldSREG = SREG;

    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = microseconds;  // Assign microseconds
    m += (TCNT1 >> 4); // Add counter value to microseconds, adjust for clock speed

    SREG = oldSREG;

    return m;
}

/************************************************************************
Timer 1 compare a interrupt service routine
    Increments global microseconds variable in 1000 microsecond intervals
************************************************************************/
ISR(TIMER1_COMPA_vect)
{
    microseconds += 1000; // increment microseconds
}

/***********************************************************************/

#define t1 5
#define t2 50
#define t3 200
#define constant 344.8 * 100 / 1000000 / 2
#define SIG_PIN PB1

/************************************************************************
ping sensor function
    computes and return the range of an object with respect to the sensor
Input: None
Output: uint16_t range
************************************************************************/
uint16_t ping_sensor(void)
{
    PORTB &= ~(1 << SIG_PIN); // setting signal pin state as LOW (disable interal pullup resistor when in INPUT mode)
    DDRB &= ~(1 << SIG_PIN);  // setting  signal pin as INPUT
    micros_reset();           // reset microsecond counter and timer
    while ((micros_now()) < t3)
    {
    }                        // looping time microseconds now after reseting is 200us or more
    DDRB |= (1 << SIG_PIN);  // setting signal pin as OUTPUT
    PORTB |= (1 << SIG_PIN); // seting signal pin output as HIGH
    micros_reset();          // reset microsecond counter and timer
    while ((micros_now()) < t1)
    {
    }                         // looping time microseconds now after reseting is 5us or more
    PORTB &= ~(1 << SIG_PIN); // setting signal pin output as LOW
    DDRB &= ~(1 << SIG_PIN);  // setting signal pin as INPUT
    while ((!(PINB & (1 << SIG_PIN)) || (micros_now() < t2)))
    {
    }                             // looping while signal pin is LOW or while time elapsed lower then t2
    if (!(PINB & (1 << SIG_PIN))) // if signal pin input is LOW
    {
        serial0_print_string("No object detected!");
        return 0xFFFF;
    }
    else // signal pin input is HIGH
    {
        micros_reset(); // reset microsecond counter and timer
        while (PINB & (1 << SIG_PIN))
        {
        }                                 // loop while signal pin input is HIGH
        return (micros_now() * constant); // returning range
    }
}

/************************************************************************
main function
    Includes the method calls, configurations and the main loop
Input: None
Output: int 1 (return status code)
************************************************************************/
int main(void)
{
    serial0_init(); // intializing serial comms
    micros_init();  // initializing Timer1
    _delay_ms(20);  // delay of 20ms

    uint16_t distance = 0;        // declaring and initializing variable distance of of type unsigned 16 bit int
    char serial_string[16] = {0}; // declaring and initializing variable serial_string of of type char array
    DDRB |= (1 << PB0);           // setting LED pin as OUTPUT
    PORTB = 0;                    // setting PORTB and LOW

    while (1)
    {
        distance = ping_sensor(); // updating distance variable with computed range
        if (distance != 0xFFFF)
        {
            sprintf(serial_string, "%4u cm\n", distance);
            serial0_print_string(serial_string);
            if (distance < 39) // if distance is in specified range
            {
                PORTB |= (1 << PB0); // set out of LED pin as HIGH
            }
            else
            {
                PORTB &= ~(1 << PB0); // set out of LED pin as LOW
            }
        }
    }
    return (1);
} // end main