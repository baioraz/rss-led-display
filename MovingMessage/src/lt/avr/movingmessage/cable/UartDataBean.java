package lt.avr.movingmessage.cable;


public class UartDataBean {

	private int status = 0;
	
	private int data = -1;

	public int getData() {
		return data;
	}

	public void setData(int data) {
		this.data = data;
	}

	public int getStatus() {
		return status;
	}

	public void setStatus(int status) {
		this.status = status;
	}
	
	
}
