#include "initializers.h"

/****************************************************
 ** Initializes External Interrupts
 *****************************************************/
void externalInterruptsInit() {
    MCUCR = (1 << ISC11) | (1 << ISC01);
    GICR = (1 << INT1) | (1 << INT0);
}


/****************************************************
 ** Initializes 16 bit timer
 *****************************************************/
void timer16Init(void) {
	// write high byte first for 16 bit register access:
	TCNT1H = 0; // set counter to zero
	TCNT1L = 0;
	// Mode 4 table 14-4 page 132. CTC mode and top in OCR1A
	// WGM13=0, WGM12=1, WGM11=0, WGM10=0
	TCCR1A = (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11);

	if (F_CPU == 20000000UL) {
		TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12) | (0 << WGM13); // crystal clock/1024
		// At what value to cause interrupt. You can use this for calibration
		// of the clock. Theoretical value for 20MHz: 19531=0x4C and 0x4B
		OCR1AH = 0x4C;
		OCR1AL = 0x4A;
	} else if (F_CPU == 16000000UL) {
		TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12) | (0 << WGM13); // crystal clock/1024
		// At what value to cause interrupt. You can use this for calibration
		// of the clock. Theoretical value for 20MHz: 19531=0x4C and 0x4B

		//1 sec
//		OCR1AH = 0x3D;
//		OCR1AL = 0x09;

		//4 times per sec
	    OCR1AH = 0x0F;
	    OCR1AL = 0x42;

	} else if (F_CPU == 14342000UL) {
	} else if (F_CPU == 8000000UL) {
	}

	// interrupt mask bit:
	TIMSK = (1 << OCIE1A);
}

/****************************************************
** Initializes 8 bit timer
*****************************************************/
void initializeTimer8_0() {
		TIMSK = (1 << TOIE0);		//Timer overflow enable
		TCNT0=0x00;
		TCCR0 =(1<<CS02)|(1<<CS00); //Prescaler clk/1024
}

/****************************************************
 ** Initializes ADC
 *****************************************************/
void adcInit() {
	ADMUX |= (1 << REFS0);

	ADCSR |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Use prescale factor 128 16000000/128=125kHz
	//ADCSR |= (1<<ADFR); //Enable free-running mode
	//ADCSR |= (1<<ADIE); //Enable ADC conversion complete interrupt
	ADCSR |= (1 << ADEN); //Enable the ADC

	//Global interrupts should be enabled before this
	ADCSR |= (1 << ADSC); //Start first conversion in Free-running mode
	//Now the conversions are running
}

/****************************************************
 ** Initializes ports
 *****************************************************/
void portsInit() {
	//	0		1		2		3		4		5		6		7
	//	ADC0	STORE	SHIFT	DATA	SDA		SCL		NA		NA
	//	0		1		1		1		0		0		0		0
	DDRC = 0x0E;
	//Enable pull ups on inputs
	PORTC = 0xC1;

	//	0		1		2		3		4		5		6		7
	//	SHUT	RST		SS		MOSI	MISO	SCK		XTAL	XTAL
	//	1		1		1		1		0		1		0		0
	//	1		0		1		0		1		0		0		0
	DDRB = 0x2F;
	PORTB = 0x10;

	//	0		1		2		3		4		5		6		7
	//	RXD		TXD		TIMER-	TIMER+	TEMPER	ROW		ROW		ROW
	//	0		1		0		0		0		1		1		1
	DDRD = 0xE2;
	//Enable pull ups on inputs
	PORTD = 0x1D;
}
