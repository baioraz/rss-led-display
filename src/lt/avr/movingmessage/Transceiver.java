package lt.avr.movingmessage;

import java.io.IOException;
import java.net.URL;
import java.util.Timer;
import java.util.TimerTask;

import jd2xx.JD2XX;
import lt.avr.movingmessage.rss.RssReader;

public class Transceiver extends TimerTask {

	private static boolean debugEnabled = false;

	public static void main(String[] args) {
		
		Transceiver transceiver = new Transceiver();

		if (args.length > 1) {
			debugEnabled = true;
			if (args[0].equals("readConfigurationData")) {
				readConfigurationData();
			} else if (args[0].equals("writeConfigurationData")) {
				if (args.length == 3) {
					writeConfigurationData(10, args[1], args[2], "1");
				} else {
					System.out.println("wrong parameters");
				}
			} else if (args[0].equals("sendData")) {
				Timer timer = new Timer();
				timer.schedule(transceiver, 1, 1000 * 60 * 2);
			} else {
				System.out.println("wrong operation");
			}
		} else {
			System.out.println("no operation");
		}

	}

	@Override
	public void run() {
		try {
			String data = RssReader.getInstance("http://www.delfi.lt/rss/feeds/itbusiness.xml").getNextEntry();
			System.out.println(data);
			sendData(data);
		} catch (Exception e) {
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
	}

	public static void sendData(String data) {
		try {
			if (debugEnabled) {
				System.out.println("TRANSMITTING DATA");
			}

			JD2XX transceiver = new JD2XX();
			setUpParameters(transceiver, 10000, 10000);
			int numberOfPackets = (int)Math.floor(data.length() / 16);
			if (numberOfPackets > 128) {
				numberOfPackets = 128;
			}
			System.out.println(numberOfPackets);
			byte controlPacketTransmitStatus = transmitControlPacket(transceiver, (numberOfPackets < 128) ? numberOfPackets : 128, Constants.OPERATION_PUSH__DATA);
			if (controlPacketTransmitStatus == 1) {
				Thread.sleep(500);
				transmitData(transceiver, data, numberOfPackets);
				Thread.sleep(500);
				byte[] controlPacket = receiveControlPacket(transceiver);
				byte[] singleByteBuffer = new byte[1];
				transceiver.read(singleByteBuffer);
				if (debugEnabled) {
					System.out.println("Transaction complete status: " + singleByteBuffer[0]);
				}
			} else {
				System.out.println("Transaction failed");
			}
			closeTransceiver(transceiver);

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public static void writeConfigurationData(int scrollSpeed, String rssWeatherUrl, String rssNewsUrl, String pattern) {
		try {
			if (debugEnabled) {
				System.out.println("TRANSMITTING DATA");
			}

			JD2XX transceiver = new JD2XX();
			setUpParameters(transceiver, 10000, 10000);
			byte controlPacketTransmitStatus = transmitControlPacket(transceiver, 16, Constants.OPERATION_SAVE_CONFIGUTATION_DATA);
			if (controlPacketTransmitStatus == 1) {
				Thread.sleep(500);
				sendConfigurationData(transceiver, scrollSpeed, 100, 1, rssWeatherUrl, rssNewsUrl, pattern);
				Thread.sleep(500);
				byte[] controlPacket = receiveControlPacket(transceiver);
				byte[] dataBytes = receiveData(transceiver, controlPacket);
				byte[] singleByteBuffer = new byte[1];
				transceiver.read(singleByteBuffer);
				if (debugEnabled) {
					System.out.println("Transaction complete status: " + singleByteBuffer[0]);
				}
				printDataBytes(dataBytes);
			} else {
				System.out.println("Transaction failed");
			}
			closeTransceiver(transceiver);

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	private static void transmitData(JD2XX transceiver, String data, int numberOfPackets) throws IOException, InterruptedException {
		if (debugEnabled) {
			System.out.println("Sending data bytes");
		}

		byte[] packetData = new byte[2048];
		System.out.println(packetData.length);
		System.out.println(numberOfPackets * 16);
		System.arraycopy(data.getBytes(), 0, packetData, 0, numberOfPackets * 16);

		int start = 0;
		for (int i = 0; i < numberOfPackets; i++) {
			transceiver.write(packetData, start, 16);
			Thread.sleep(250);
			int b = transceiver.read();
			if (debugEnabled) {
				System.out.println("Data packet transmission status: " + b);
			}
			start+=16;
		}
		
		if (debugEnabled) {
			System.out.println("Data bytes sent");
		}
	}

	public static void readConfigurationData() {
		try {
			if (debugEnabled) {
				System.out.println("TRANSMITTING DATA");
			}

			JD2XX transceiver = new JD2XX();
			setUpParameters(transceiver, 10000, 10000);
			byte controlPacketTransmitStatus = transmitControlPacket(transceiver, 0, Constants.OPERATION_READ_CONFIGUTATION_DATA);
			if (controlPacketTransmitStatus == 1) {
				// Thread.sleep(1000);
				byte[] controlPacket = receiveControlPacket(transceiver);
				byte[] dataBytes = receiveData(transceiver, controlPacket);
				byte[] singleByteBuffer = new byte[1];
				transceiver.read(singleByteBuffer);
				if (debugEnabled) {
					System.out.println("Transaction complete status: " + singleByteBuffer[0]);
				}
				printDataBytes(dataBytes);
			} else {
				System.out.println("Transaction failed");
			}
			closeTransceiver(transceiver);

		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("Error");
			System.out.println(e.getMessage());
		}
	}

	private static void sendConfigurationData(JD2XX transceiver, int scrollSpeed, int receiveTimeout, int deviceDebugEnabled, String rssWeatherUrl,
			String rssNewsUrl, String pattern) throws IOException, InterruptedException {
		if (debugEnabled) {
			System.out.println("Sending config data bytes");
		}

		byte[] packetData = new byte[256];

		// First packet of data
		packetData[0] = (byte) scrollSpeed;
		packetData[1] = (byte) receiveTimeout;
		packetData[2] = (byte) deviceDebugEnabled;

		// Second packet of data (currently empty)

		// Copy 6 bytes of rss weather url starting from position - 32
		System.arraycopy(rssWeatherUrl.getBytes(), 0, packetData, 32, rssWeatherUrl.getBytes().length);
		// copy 6 bytes of rss news url starting from position - 128
		System.arraycopy(rssNewsUrl.getBytes(), 0, packetData, 128, rssNewsUrl.getBytes().length);
		// copy 2 pattern bytes starting from position - 224
		System.arraycopy(pattern.getBytes(), 0, packetData, 224, pattern.getBytes().length);
		for (int i = 1; i < packetData.length + 1; i++) {
			transceiver.write(packetData[i - 1]);
			if ((i % 16) == 0) {
				Thread.sleep(220);
				int b = transceiver.read();
				if (debugEnabled) {
					System.out.println("Data packet transmission status: " + b);
				}
			}
		}
		if (debugEnabled) {
			System.out.println("Data bytes sent");
		}
	}

	private static void printDataBytes(byte[] dataBytes) {
		for (int i = 0; i < dataBytes.length; i++) {
			System.out.print((char) dataBytes[i]);
		}
	}

	private static byte[] receiveData(JD2XX transceiver, byte[] controlPacket) throws IOException {
		if (debugEnabled) {
			System.out.println("RECEIVING DATA");
		}

		int numberOfPacketsToReceive = controlPacket[0];
		StringBuffer status = new StringBuffer();
		status.append((char) controlPacket[1]);
		status.append((char) controlPacket[2]);

		if ("OK".equals(status.toString())) {
			return receiveDataPackets(transceiver, numberOfPacketsToReceive);
		} else {
			return "Transaction failed".getBytes();
		}

	}

	private static byte[] receiveControlPacket(JD2XX transceiver) throws IOException {
		if (debugEnabled) {
			System.out.println("Receiving control packet");
		}
		byte[] singleByteBuffer = new byte[1];
		singleByteBuffer[0] = 0;

		// Read reception status
		transceiver.read(singleByteBuffer);
		if (debugEnabled) {
			System.out.println("reception status: " + singleByteBuffer[0]);
		}
		// Read config data packet
		byte[] packet = new byte[16];
		transceiver.read(packet);
		if (debugEnabled) {
			for (int i = 0; i < packet.length; i++) {
				System.out.println("Control data byte [" + i + "]:" + packet[i]);
			}
		}
		return packet;
	}

	private static byte[] receiveDataPackets(JD2XX transceiver, int numberOfPacketsToReceive) throws IOException {
		byte[] singleByteBuffer = new byte[1];
		if (debugEnabled) {
			System.out.println("Receiving data bytes");
		}

		byte[] tmpBuf = new byte[16 * numberOfPacketsToReceive];
		int bytesRead = 0;
		int start = 0;
		for (int i = 0; i < numberOfPacketsToReceive; i++) {
			transceiver.read(singleByteBuffer);
			if (debugEnabled) {
				System.out.println("Reception status [" + i + "]:" + singleByteBuffer[0]);
			}

			byte readBytes[] = transceiver.read(16);
			if (debugEnabled) {
				for (int j = 0; j < readBytes.length; j++) {
					System.out.println("Received data byte [" + j + "]:" + (char) readBytes[j]);
				}
			}
			bytesRead = bytesRead + readBytes.length;
			System.arraycopy(readBytes, 0, tmpBuf, start, readBytes.length);
			start = start + 16;
		}

		byte[] dataBytes = new byte[bytesRead];
		System.arraycopy(tmpBuf, 0, dataBytes, 0, bytesRead);

		return dataBytes;
	}

	private static byte transmitControlPacket(JD2XX transceiver, int numberOfPackets, int operation) throws IOException, InterruptedException {
		// Control packet
		byte[] packet = new byte[16];
		packet[0] = (byte) numberOfPackets;
		packet[1] = (byte) operation;
		transceiver.write(packet);

		// Thread.sleep(500);
		if (debugEnabled) {
			System.out.println("Control packet sent");
		}
		byte singleByteBuffer[] = new byte[1];
		// Read fifo status
		transceiver.read(singleByteBuffer);
		if (debugEnabled) {
			System.out.println("FIFO receive status: " + singleByteBuffer[0]);
		}
		transceiver.read(singleByteBuffer);
		if (debugEnabled) {
			System.out.println("Control packet transmission status: " + singleByteBuffer[0]);
		}
		return singleByteBuffer[0];
	}

	private static void setUpParameters(JD2XX transceiver, int readTimeout, int writeTimeout) throws IOException {
		Object object[] = transceiver.listDevicesBySerialNumber();
		for (Object object2 : object) {
			System.out.println(object2);
		}
		transceiver.openBySerialNumber("A9RZCGJW");

		// Configure the device.
		transceiver.setBaudRate(JD2XX.BAUD_230400);
		transceiver.setDataCharacteristics(8, JD2XX.STOP_BITS_1, JD2XX.PARITY_NONE);
		transceiver.setFlowControl(JD2XX.FLOW_NONE, 0, 0);
		transceiver.setTimeouts(readTimeout, writeTimeout);
	}

	public static void closeTransceiver(JD2XX transceiver) throws IOException {
		transceiver.purge(JD2XX.PURGE_RX);
		transceiver.purge(JD2XX.PURGE_TX);
		// transceiver.purge(JD2XX.PURGE_RX | JD2XX.PURGE_TX);
		transceiver.close();
		// transceiver.cyclePort();
		// transceiver.resetDevice();
		// transceiver.resetPort();

		transceiver = null;
	}

}
