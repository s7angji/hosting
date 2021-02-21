package com.example.chattingnioclient;


// �ȵ���̵� ���� �����忡�� �����͸� ������ �� ���� ������ �����带 �������.
class InputThread extends Thread {
	private final NioChatterClient cc;
	private final String s;

	public InputThread(NioChatterClient cc, String msg) {
		this.cc = cc;
		this.s = msg;
	}

	@Override
	public void run() {
		if(s.equals("quit"))
			cc.quit();
		else
			cc.sendMessage(s);
	}
}