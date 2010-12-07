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

				Timer timer = new Timer();
				timer.schedule(transceiver, 1, 1000 * 60 * 2);

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
			System.out.println("TRANSMITTING DATA");

			JD2XX transceiver = new JD2XX();
			setUpParameters(transceiver, 10000, 10000);
			int numberOfPackets = (int)Math.floor(data.length() / 16);
			if (numberOfPackets > 128) {
				numberOfPackets = 128;
			}
			System.out.println(numberOfPackets);
			byte controlPacketTransmitStatus = transmitControlPacket(transceiver, (numberOfPackets < 128) ? numberOfPackets : 128, Constants.OPERATION_PUSH__DATA);
			if (controlPacketTransmitStatus == 1) {
				Thread.sleep(2000);
				transmitData(transceiver, data, numberOfPackets);
				byte[] singleByteBuffer = new byte[1];
				transceiver.read(singleByteBuffer);
				System.out.println("Transaction complete status: " + singleByteBuffer[0]);

			} else {
				System.out.println("Transaction failed");
			}
			closeTransceiver(transceiver);

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	private static void transmitData(JD2XX transceiver, String data, int numberOfPackets) throws IOException, InterruptedException {
		System.out.println("Sending data bytes");

		byte[] packetData = new byte[2048];
		System.out.println(packetData.length);
		System.out.println(numberOfPackets * 16);
		System.arraycopy(data.getBytes(), 0, packetData, 0, numberOfPackets * 16);

		int start = 0;
		for (int i = 0; i < numberOfPackets; i++) {
			transceiver.write(packetData, start, 16);
			Thread.sleep(300);
			int b = transceiver.read();
			System.out.println("Data packet transmission status: " + b);
			start+=16;
		}
		
		System.out.println("Data bytes sent");

	}

	private static void printDataBytes(byte[] dataBytes) {
		for (int i = 0; i < dataBytes.length; i++) {
			System.out.print((char) dataBytes[i]);
		}
	}

	private static byte transmitControlPacket(JD2XX transceiver, int numberOfPackets, int operation) throws IOException, InterruptedException {
		// Control packet
		byte[] packet = new byte[16];
		packet[0] = (byte) numberOfPackets;
		transceiver.write(packet);

		System.out.println("Control packet sent");

		byte singleByteBuffer[] = new byte[1];
		// Read fifo status
		transceiver.read(singleByteBuffer);
		System.out.println("FIFO receive status: " + singleByteBuffer[0]);
		transceiver.read(singleByteBuffer);
		System.out.println("Control packet transmission status: " + singleByteBuffer[0]);
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
