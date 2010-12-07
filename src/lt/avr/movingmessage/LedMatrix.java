package lt.avr.movingmessage;

import gnu.io.CommPortIdentifier;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import jd2xx.JD2XX;

public class LedMatrix {

	private static LedMatrix ledMatrix = null;

	// Create and start the thread
	//private static DataTransmissionThread thread = null;

	private LedMatrix() {
	}

	public static LedMatrix getInstance() {
		if (ledMatrix == null) {
			synchronized (LedMatrix.class) {
				if (ledMatrix == null) {
					ledMatrix = new LedMatrix();
				}
			}
		}
		return ledMatrix;
	}

	public static List<String> getComPorts() {
		List<String> comPorts = new ArrayList<String>();
		Enumeration<CommPortIdentifier> en = CommPortIdentifier.getPortIdentifiers();
		if (en.hasMoreElements()) {
			comPorts.clear();
		}
		while (en.hasMoreElements()) {
			CommPortIdentifier commPortIdentifier = (CommPortIdentifier) en.nextElement();
			if (CommPortIdentifier.PORT_SERIAL == commPortIdentifier.getPortType()) {
				System.out.println(commPortIdentifier.getName());
				comPorts.add(commPortIdentifier.getName());
			}
		}
		return comPorts;
	}

	public void readConfigurationData() {
		
		try {
			String[] cmd = {"java"};
			Runtime.getRuntime().exec(cmd);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}


	}



	public void stopDataTransmission() {
//		if (thread != null) {
//			thread.allDone = true;
//		}
	}

}
