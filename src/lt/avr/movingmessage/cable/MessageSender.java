package lt.avr.movingmessage.cable;

import lt.avr.movingmessage.CharacterBytes;

public class MessageSender {

	public static int sendMessage(String message) {

		int[] byteArray = new int[2048];
		
		int index = 0;
		
		//Write 41 space
		//for(int i = index; i<41; i++){
		//	byteArray[i] = CharacterBytes.characters[0][0];
			index = 83;
		//}
		
		int messageIndex = 0;
		int j = 0;
		while (index < 2048 && messageIndex < message.length()) {

			byteArray[index] = CharacterBytes.characters[(int)message.charAt(messageIndex)][j];
			index++;
			j++;
			if (j > 5) {
				j = 0;
				messageIndex++;
			}
		}
		
//		for(int i = index; i<1024; i++){
//			byteArray[i] = CharacterBytes.characters[0][0];
//		}

		for (int i = 0; i < 2048; i++) {
			//System.out.print(byteArray[i]);
			AvrUsbUartWrapper.sendUnsignedByteToUart(byteArray[i]);
		}
		return 0;
	}

}
