#include "tests.h"



void currentTest(unsigned char * message_buffer) {

	//Prepare empty message
 	for(unsigned char j = 0; j < 80; j++){
 		message_buffer[j] = 0x00;
  	}

	for (int k = 0; k < 80; k++) {


		message_buffer[k] = 0xFF;


		for (int i = 0; i < 100; i++) {
			display_buffer(0);
		}
	
		
	}

}
