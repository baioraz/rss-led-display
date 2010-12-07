#include "ledboard.h"

/****************************************************
 ** Resets shift registers
 *****************************************************/
/*
 void board_reset(){
 RESET_LOW();
 //SHIFT_HIGH();
 //SHIFT_LOW();
 STORE_LOW();
 STORE_HIGH();
 RESET_HIGH();

 }
 */

/****************************************************
 ** Shifts data into shift registers
 *****************************************************/
void shift_row(unsigned char row) {

	for (unsigned char i = 0; i < BOARD_LENGHT; i++) {
		if (message_buffer[i] & row) {
			DATA_HIGH();
		}
		_delay_us(10);
		SHIFT_LOW();
		_delay_us(10);
		SHIFT_HIGH();
		DATA_LOW();
	}

}

/****************************************************
 ** Displays current message buffer
 *****************************************************/
void display_buffer(unsigned char speed) {

	unsigned char row = 0x02;
	for (unsigned char i = 0; i < 7; i++) {

		shift_row(row);

		STORE_LOW();
		STORE_HIGH();

		R1DIS();
		R2DIS();
		R4DIS();
		if (i & 0x01)
			R1EN();
		if (i & 0x02)
			R2EN();
		if (i & 0x04)
			R4EN();

		//PORTD = i << 5;

		if (speed > 9) {
			// Memory leak
			//_delay_us(7500 - 13 * 300);
			_delay_us(3750 - 13 * 150);
		} else if (speed != 0) {
			_delay_us(2700);
			//	_delay_us(500);
		}

		if (row < 0x80) {
			row = row << 1; // next line to be switched on
		} else {
			row = 0x02;
		}
	}
}
