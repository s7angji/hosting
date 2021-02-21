package com.example.test_android_aoa;


//�ȵ���̵� ���� �����忡�� �����͸� ������ �� ���� ������ �����带 �������.
class InputThread extends Thread {
	private final NioChatterServer cc;
	private final String s;

	public InputThread(NioChatterServer cc, String msg) {
		this.cc = cc;
		this.s = msg;
	}

	@Override
	public void run() {
		if(s.equals("quit"))
			cc.quit();
		else
			cc.sendBroadcastMessage(s);
	}
}