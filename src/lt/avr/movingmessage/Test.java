package lt.avr.movingmessage;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class Test {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			Process p = Runtime.getRuntime().exec("java -cp bin;lib\\jd2xx.jar;lib\\simple-xml-2.3.3.jar -Djava.library.path=lib lt.avr.movingmessage.Transceiver sendData http://www.delfi.lt http://www.delfi.lt");
			//Process p = Runtime.getRuntime().exec("java -cp bin;lib\\jd2xx.jar -Djava.library.path=lib lt.avr.movingmessage.Transceiver sendData http://www.delfi.lt http://www.delfi.lt 41123");
			//Process p = Runtime.getRuntime().exec("java -cp bin;lib\\jd2xx.jar -Djava.library.path=lib lt.avr.movingmessage.Transceiver forceWirelessMode http://www.delfi.lt http://www.delfi.lt 41123");
			
			BufferedInputStream bis = new BufferedInputStream(p.getInputStream()); 
			
			int i = 0;
			//if (bis.available() > 0) {
				while ((i = bis.read()) != -1) {
					System.out.print((char) i);
				}
			//}

			InputStream o = p.getErrorStream();
			while ((i = o.read()) != -1) {
				System.out.print((char)i);
			}
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
