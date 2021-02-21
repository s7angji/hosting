package com.example.test_android_aoa;


//안드로이드 메인 스레드에서 데이터를 전송할 수 없기 때문에 스레드를 만들었다.
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