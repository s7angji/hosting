package com.example.chattingnioclient;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.util.Iterator;
import java.util.Set;

import android.os.Handler;
import android.os.Message;

public class NioChatterClient extends Thread {
	
	private StringBuilder mStringBuilder; // 버퍼 역할을 함

	private final String serverIpAdress;
	private final int port = 6000;

	// Thread 종료를 위한 flag
	private boolean running;
	private boolean isConnected = false;

	public SocketChannel clientchannel;
	private Selector selector;

	private final ByteBuffer readBuffer;
	private final ByteBuffer writeBuffer;
	private final CharsetDecoder decoder;
	private final CharsetEncoder encoder;

	private static final long TIME_OUT = 5000;
	private static final int BUFFER_SIZE = 2048;

	protected ChattingNioClientActivity hostActivity;
	protected Handler hostHandler;

	public void sendHandlerMessage(String msg, int what) {
		Message m = new Message();
		m.what = what;
		m.obj = msg;
		hostHandler.sendMessage(m);
	}

	// 생성자
	public NioChatterClient(ChattingNioClientActivity hostActivity, String serverIpAdress) {
		
		mStringBuilder = new StringBuilder();

		this.hostActivity = hostActivity;
		this.serverIpAdress = serverIpAdress;
		this.hostHandler = hostActivity.mHandler;

		readBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);
		writeBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);

		// 네트워크로 전송하고 수신받는 charset은 모두 'UTF-8을 사용한다.
		Charset charset = Charset.forName("UTF-8");
		decoder = charset.newDecoder();
		encoder = charset.newEncoder();
	}

	private void initClientSocket() {
		try { // 소켓채널을 만든다.
			clientchannel = SocketChannel.open();
			clientchannel.configureBlocking(false);
			clientchannel.connect(new InetSocketAddress(InetAddress.getByName(serverIpAdress), port));
			selector = Selector.open();
			clientchannel.register(selector, SelectionKey.OP_CONNECT);

			running = true;
		} catch(Exception e) {
			running = true; // TODO : 우선 살려둬야 날 만든 메인 스레드가 날 죽인다

			sendHandlerMessage("소켓채널 생성 중 예외 발생\n" +
					"** IP 주소가 바르지 못할 가능성이 높습니다.\n" +
					"** 확인 후 다시 시도해 주세요.", ChattingNioClientActivity.FINISH_MSG);
		}
	}

	@Override
	public void run() {

		initClientSocket();

		// 무한 반복 작업으로 서버에 접속 요청이나, 서버에서 오는 메세지를 기다린다.
		while(running) {

			try {
				int n = selector.select(TIME_OUT);
				if(n == 0) {
					if(isConnected) continue; // 서버에 연결은 됬는데 그냥 타임아웃으로 나온것 뿐
					else {
						sendHandlerMessage("Time-Out 안에 도어락과 연결하지 못했습니다.\n" +
								"** IP 주소를 잘못 입력하셨을 수도 있습니다.\n" +
								"** 도어락 쪽 기기의 인터넷이 off상태일 수도 있습니다.\n" +
								"** 도어락 쪽 기기의 전원이 off상태일 수도 있습니다.\n" +
								"** 또는 인터넷상 문제일 것입니다.\n" +
								"** 확인 후 다시 시도해 주세요", ChattingNioClientActivity.SERVER_CONNECTED_TIMEOUT_MSG); // 5초 안에 서버와 연결을 못 했다.
					}
				}

				Set<SelectionKey> keys = selector.selectedKeys();
				Iterator<SelectionKey> it = keys.iterator();

				while(it.hasNext()) {
					SelectionKey key = it.next();
					it.remove();

					if(!key.isValid())
						continue;

					if(key.isConnectable()) {
						// 서버로 연결을 요청한다.
						serverNewConnection(key);
					}
					else if(key.isReadable()) {
						// 서버가 전송한 메세지를 읽는다.
						readIncomingMessages(key);
					}
				}
			} catch (Exception e) {
				sendHandlerMessage("Connect에 실패했습니다.\n" +
						"** 도어락 앱이 실행되지 않고 있을 수도 있습니다.\n" +
						"** 도어락 앱이 인터넷을 사용하지 못하고 있을 수도 있습니다.\n" +
						"** 또는 인터넷상 문제일 것입니다.\n" +
						"** 확인 후 다시 시도해주세요", ChattingNioClientActivity.FINISH_MSG);
				e.printStackTrace();
			}
		}
	}

	private void serverNewConnection(SelectionKey key) throws IOException{

		SocketChannel channel = (SocketChannel)key.channel();

		while(channel.isConnectionPending())
			channel.finishConnect();

		if(channel.isConnected()) {
			sendHandlerMessage("정상적으로 서버에 접속하였습니다.", ChattingNioClientActivity.CONNECT_MSG);
			isConnected = true;
		}
		else {
			key.cancel();
			sendHandlerMessage("(Connect시도 중) 서버에 문제가 존재합니다.", ChattingNioClientActivity.FINISH_MSG);
		}

		// 접속에 성공했으므로 이제 채널로부터 읽을 수 있다.
		channel.register(selector, SelectionKey.OP_READ);

	}

	private void readIncomingMessages(SelectionKey key) {
		SocketChannel channel = null;
		try {
			if(key == null) {
				sendHandlerMessage("isReadable()로 들어 왔는데 왜 key가 없냐?", ChattingNioClientActivity.MSG);
				return;
			}

			channel = (SocketChannel)key.channel();
			readBuffer.clear();

			// 채널로부터 메세지를 읽어 버퍼에 저장한다.
			long nbytes = channel.read(readBuffer);

			// FIN 패킷을 받았기 때문에 채널을 닫고 키를 취소한다.
			if(nbytes == -1) {
				key.cancel();
				sendHandlerMessage("서버에서 종료시켰습니다", ChattingNioClientActivity.FINISH_MSG);
			}

			// 'UTF-8'인 CharsetDecoder를 이용하여 바이트버퍼를 문자열로 만든다.
			readBuffer.flip();
			String line = decoder.decode(readBuffer).toString().trim();
			readBuffer.clear();

			// TODO : 여기서 도어락으로부터 받은 메세지를 해독한다 !!! >_<
			mStringBuilder.append(line);
			parsing();
			sendHandlerMessage(mStringBuilder.toString(), ChattingNioClientActivity.MSG);
			
		} catch(IOException e) {
			if(channel != null) {
				key.cancel();
				sendHandlerMessage("서버가 비정상 종료되었습니다.", ChattingNioClientActivity.FINISH_MSG);
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	public void sendMessage(String msg){
		prepWriteBuffer(msg);
		channelWrite(clientchannel);
	}

	private void prepWriteBuffer(String msg) {
		// 문자열을 Charbuffer로 만든다음 다시 인코딩하여 바이트버퍼를 만든다.
		writeBuffer.clear();
		// 문자열을 CharBuffer로 만든다.
		CharBuffer requestChars = CharBuffer.wrap(msg);
		try { // 자바의 문자열을 'UTF-8' 인코드를 사용하여 바이트버퍼로 만든다.
			ByteBuffer cbuf = encoder.encode(requestChars);
			// 인코딩된 바이트버퍼를 전송 바이트버퍼로 다시 만들어 준다.
			writeBuffer.put(cbuf);
		} catch(CharacterCodingException e) {
			e.printStackTrace();
		}
		// CR과 LF를 넣어 스트림에서도 읽을 수 있도록 만들어 준다.
		writeBuffer.putChar('\r');
		writeBuffer.putChar('\n');
		writeBuffer.flip();
	}

	private void channelWrite(SocketChannel channel) {
		long nbytes = 0;
		long toWrite = writeBuffer.remaining();

		// 전송할 메세지량이 많다면, 커널은 메세지를 나누어서 보낸다.
		// 따라서 다음과 같이 처리한다.
		try {
			while(nbytes != toWrite) {
				nbytes += channel.write(writeBuffer);
			}
		} catch(Exception e) { }

		writeBuffer.rewind(); // 다시 쓸 수 있도록 만듬
	}

	private void parsing() {
		MessageParser mp = new MessageParser(mStringBuilder.toString());
		boolean loop = true;
		while(loop) {
			MessageParser.ParsingResult pr = mp.parsing();

			switch(pr.MESSAGE_PARSING_RESULT) {

			case MessageParser.MESSAGE_PARSER_OTP:

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				MessageParser.Otp otp = pr.otp;
				sendHandlerMessage(otp.getOtp(), ChattingNioClientActivity.MSG);
				// 프리퍼런스에 OTP 저장
				hostActivity.putPre(ChattingNioClientActivity.PREFERENCES_OTP, otp.getOtp());

				sendHandlerMessage(otp.getOtp(), ChattingNioClientActivity.OTP_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_DOOR_LOCATION:

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				MessageParser.DoorLocation doorLocation = pr.doorLocation;
				sendHandlerMessage(String.valueOf(doorLocation.getLatitude()), ChattingNioClientActivity.MSG);
				sendHandlerMessage(String.valueOf(doorLocation.getLongitude()), ChattingNioClientActivity.MSG);
				// 프리퍼런스에 위도 저장
				hostActivity.putPre(ChattingNioClientActivity.PREFERENCES_LATITUDE, String.valueOf(doorLocation.getLatitude()));
				// 프리퍼런스에 경도 저장
				hostActivity.putPre(ChattingNioClientActivity.PREFERENCES_LONGITUDE, String.valueOf(doorLocation.getLongitude()));

				sendHandlerMessage("위도 경도 값 저장했다", ChattingNioClientActivity.DOOR_LOCATION_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_PROXIMITY_ALERT:

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				sendHandlerMessage("근접경보가 어떤지 알려달라고 그러네", ChattingNioClientActivity.PROXIMITY_ALERT_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_SUSPICIOUS_APPROACH:

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				sendHandlerMessage("의심스러운 접근이라고 알려주네", ChattingNioClientActivity.SUSPICIOUS_APPROACH_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_EMPTY:

				loop = false;

				sendHandlerMessage("비어있는 정보를 파싱하라고 그런다. 뭐냐?", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_IMPERFECT:

				loop = false;

				sendHandlerMessage("아직 완벽하게 전부 메세지가 전달 안 됨", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;
			case MessageParser.MESSAGE_PARSER_I_DONT_KNOW:

				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage("뭔 내용인지 모르겠다", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;
			default:

				// 뭔가 이상하니까 싹 다 비우자 ~!!
				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage("default 있을 수 없는 일이야  ㅠ ㅠ", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;
			}
		}
	}

	public void quit() {
		running = false;
		try {
			clientchannel.close();
		} catch(IOException e) {
			e.printStackTrace();
		}
		interrupt();
	}
}
