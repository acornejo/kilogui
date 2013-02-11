#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>

#define LED1     1
#define LED2     2
#define IRLED    3

#define F_CRYSTAL 8000000
#define BAUDRATE 19200 //Set baud rate
#define UBRRVAL ((F_CRYSTAL/(BAUDRATE*16UL))-1)

void send_message(int,int,int);

static volatile	int ReceivedByte;

int main() {
	DDRB = (1<<1)|(1<<2); //for green leds
	DDRD = (1<<2)|(1<<3); //for blue led.

	//ensure ir led is off
	PORTD &= ~(1<<IRLED);

	//move interrupt vectors to bootloader interupts
	MCUCR = (1<<IVCE); //allow changing IVSEL
	MCUCR = (1<<IVSEL); //move interrupt vectors to start of boot loader

	// Setup the UART parameters
	cli();					//Disable global interrupts.
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
							// Enable reception, enable reception interrupt,
	                        // enable tranmission. Note: Transmission is enabled
						    // to ensure that the tx pin pulls high. This ensures that
						    // the USB transceiver does not get stuck.
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	 					    // Async, parity disabled, 8 bit communication, 1 stop bit
    UBRR0 = UBRRVAL;	    //Set buad rate, this must be set as the last parameter.
	sei(); 					//Enable global interrupts

	// Use LEDs to flash power on indicator signal.
	PORTB |= (1<<LED1);
	PORTD |= (1<<LED2);
    _delay_ms(200);
	PORTB &= ~(1<<LED1);
	PORTD &= ~(1<<LED2);
	_delay_ms(200);
	PORTB |= (1<<LED1);
	PORTD |= (1<<LED2);
    _delay_ms(200);
	PORTB &= ~(1<<LED1);
	PORTD &= ~(1<<LED2);


	while(1) {
		if(ReceivedByte!=0) {
            int message_received = ReceivedByte;
            ReceivedByte = 0;

            switch(message_received) {
            case 'a':
				PORTD = (1<<LED2);
				//send boot load message first
				for(int i=0;i<100;i++) {
					send_message(1,0,1);
					_delay_ms(5);
				}
				PORTD &= ~(1<<LED2);
				_delay_ms(8000);

				//next bootload untill uart says to stop
                while (!ReceivedByte) {
                    for(uint8_t page=0;page<220 && ReceivedByte == 0; page++) {
                        uint16_t checksum=page;
                        cli();
                        send_message(page,0,250);
                        _delay_ms(1);
                        for (int i=0; i<SPM_PAGESIZE; i+=2) {
                            int high =pgm_read_byte(0x0000+i+page*0x0080);
                            int low =pgm_read_byte(0x0000+i+page*0x0080+1);
                            send_message(high,low,0);
                            checksum+=high;
                            checksum+=low;
                        }
                        _delay_ms(1);
                        uint8_t checksum_low=checksum;
                        uint8_t checksum_high=checksum>>8;
                        send_message(checksum_high,checksum_low,254);
                        sei(); //allow for uart to rx
                        PORTD = (1<<LED2);
                        _delay_ms(10);
                        PORTD &= ~(1<<LED2);
                    }
                }
                break;
            case 'b':
				for(int i=0;i<100;i++) {
                    PORTD = (1<<LED2);
					send_message(2,0,1);
					_delay_ms(10);
					PORTD &= ~(1<<LED2);
					_delay_ms(2);
				}
                break;
            case 'c':
				while(ReceivedByte==0) {
                    PORTD = (1<<LED2);
					send_message(3,0,1);
					_delay_ms(1);
					PORTD &= ~(1<<LED2);
					_delay_ms(1);
				}
                break;
            case 'd':
				while(ReceivedByte==0) {
                    PORTD = (1<<LED2);
					send_message(4,0,1);
					_delay_ms(1);
					PORTD &= ~(1<<LED2);
					_delay_ms(1);
				}
                break;
            case 'e':
				for(int i=0;i<100;i++) {
                    PORTD = (1<<LED2);
					send_message(5,0,1);
					_delay_ms(10);
                    PORTD &= ~(1<<LED2);
					_delay_ms(2);
				}
                break;
            case 'f':
				for(int i=0;i<100;i++) {
                    PORTD = (1<<LED2);
					send_message(6,0,1);
					_delay_ms(10);
					PORTD &= ~(1<<LED2);
					_delay_ms(1);
				}
                break;
            case 'g':
				for(int i=0;i<100;i++) {
                    PORTD = (1<<LED2);
					send_message(7,0,1);
					_delay_ms(10);
                    PORTD &= ~(1<<LED2);
					_delay_ms(2);
				}
                break;
            case 'i':
                PORTD ^= (1<<LED2);
                break;
            case 'j':
				while(ReceivedByte==0) {
					PORTD = (1<<LED2);
					send_message(1,0,1);
					_delay_ms(10);
					PORTD &= ~(1<<LED2);
					_delay_ms(2);
				}
                break;
            case 'z':
				for(int i=0;i<100;i++) {
                    PORTD = (1<<LED2);
					send_message(8,0,1);
					_delay_ms(10);
					PORTD &= ~(1<<2);
					_delay_ms(2);
				}
                break;
            }
		}
	}

    return 0;
}

void send_message(int a, int b, int c)
{
	sei();
	DDRD |= (1<<3);
	uint16_t data_out[4];
	uint8_t data_to_send[4]={a,b,c,128};

	//prepare data checksum to send
	data_to_send[3]+=data_to_send[0]+data_to_send[1]+data_to_send[2];

	//prepare data to send
	for(int i=0;i<4;i++)
	{
		data_out[i]=(data_to_send[i] & (1<<0))*128 +
				(data_to_send[i] & (1<<1))*32 +
				(data_to_send[i] & (1<<2))*8 +
				(data_to_send[i] & (1<<3))*2+
				(data_to_send[i] & (1<<4))/2+
				(data_to_send[i] & (1<<5))/8 +
				(data_to_send[i] & (1<<6))/32 +
				(data_to_send[i] & (1<<7))/128;

		data_out[i]=data_out[i]<<1;
		data_out[i]++;
	}

	cli();//start critical

    //send start pulse
    PORTD |= (1<<IRLED);
    asm volatile("nop\n\t");
    asm volatile("nop\n\t");
    PORTD &= ~(1<<IRLED);

    //wait for own signal to die down
    for(int k=0;k<53;k++)
        asm volatile("nop\n\t");

    //check for collision
    for(int k=0;k<193;k++) {
        if((ACSR & (1<<ACO))>0)
            asm volatile("nop\n\t");
        if((ACSR & (1<<ACO))>0)
            asm volatile("nop\n\t");
    }

    for(int byte_sending=0;byte_sending<4;byte_sending++) {
        int i=8;
        while(i>=0) {
            if(data_out[byte_sending] & 1) {
                PORTD |= (1<<IRLED);
                asm volatile("nop\n\t");
                asm volatile("nop\n\t");
            } else {
                PORTD &= ~(1<<IRLED);
                asm volatile("nop\n\t");
                asm volatile("nop\n\t");
            }
            PORTD &= ~(1<<IRLED);
            for(int k=0;k<35;k++) {
                asm volatile("nop\n\t");
            }
            data_out[byte_sending]=data_out[byte_sending]>>1;
            i--;
        }
    }

    //ensure led is off
    PORTD &= ~(1<<IRLED);

    //wait for own signal to die down
    for(int k=0;k<50;k++)
        asm volatile("nop\n\t");

    ACSR |= (1<<ACI);
    sei();//end critical
}

//*************************************************************
//* Interrupt Service Routine for USART reception             *
//* Dependencies: Global interrupts must be enabled           *
//* Notes: The receive interrupt flag is automatically        *
//* cleared when the receive buffer is read                   *
//*************************************************************

// Start of USART reception interrupt service.
ISR(USART_RX_vect) {
    ReceivedByte = UDR0; // Fetch the recieved byte (and clear the interrupt flag)
    PORTB |= (1<<LED1);
}
//End of USART interrupt service

