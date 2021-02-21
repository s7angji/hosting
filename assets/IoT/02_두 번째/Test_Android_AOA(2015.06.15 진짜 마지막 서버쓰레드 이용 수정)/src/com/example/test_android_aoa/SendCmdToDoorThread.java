package com.example.test_android_aoa;

public class SendCmdToDoorThread extends Thread {
	DoorController dc;
	byte command;
	byte target;
	byte value;
	
	// »ý¼ºÀÚ
	public SendCmdToDoorThread(DoorController dc, byte command, byte target, byte value) {
		this.dc = dc;
		this.command = command;
		this.target = target;
		this.value = value;
	}
	
	@Override
	public void run() {
		if(dc.isAlive())
			dc.sendCommand(command, target, value);
	}

}
