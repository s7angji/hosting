package com.example.test_android_aoa;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Set;
import java.util.StringTokenizer;

import android.os.Handler;
import android.os.Message;

public class NioChatterServer extends Thread {

	// 서버에 접속한 모든 클라이언트를 리스트로 관리한다.
	private final LinkedList<SocketChannel> clients;

	private StringBuilder mStringBuilder; // 버퍼 역할을 함
	private LinkedList<String> clientsName;

	// Thread 종료를 위한 flag
	private boolean running;

	private ServerSocketChannel serverchannel;
	private Selector selector;

	private final ByteBuffer readBuffer;
	private final ByteBuffer writeBuffer;
	private final CharsetDecoder decoder;
	private final CharsetEncoder encoder;

	private static final long TIME_OUT = 3000;
	private static final int BUFFER_SIZE = 2048;
	private final int port = 6000;
	public String ipAddress;

	MainActivity hostActivity;
	protected Handler hostHandler;

	public void sendHandlerMessage(String msg, int what) {
		Message m = new Message();
		m.what = what;
		m.obj = msg;
		hostHandler.sendMessage(m);
	}

	// 생성자
	public NioChatterServer(MainActivity hostActivity) {

		mStringBuilder = new StringBuilder();

		this.hostActivity = hostActivity;
		this.hostHandler = hostActivity.mHandler;
		this.clientsName = hostActivity.clientsName;

		ipAddress = null;

		readBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);
		writeBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);

		// 네트워크로 전송하고 수신받는 charset은 모두 'UTF-8을 사용한다.
		Charset charset = Charset.forName("UTF-8");
		decoder = charset.newDecoder();
		encoder = charset.newEncoder();

		// 서버에 접속한 사용자들을 관리하기 위해 리스트로 개별 채널을 관리한다.
		clients = new LinkedList<SocketChannel>();
	}

	private void initServerSocket() {
		try { // 서버소켓채널을 만든다.
			ipAddress = getLocalIpAddress(INET4ADDRESS);

			serverchannel = ServerSocketChannel.open();
			serverchannel.configureBlocking(false);
			serverchannel.socket().bind(new InetSocketAddress(ipAddress, port));
			selector = Selector.open();
			serverchannel.register(selector, SelectionKey.OP_ACCEPT);

			running = true;

			// 만든 서버소캣채널의 소캣에 바인드 된 IP 주소를 Activity에 알려줌
			sendHandlerMessage(ipAddress, MainActivity.GET_IP_SERVER);
		} catch(Exception e) {
			running = true; // 우선 살려둬야 날 만든 메인 스레드가 날 죽인다

			sendHandlerMessage("소켓채널 생성 중 예외 발생\n" +
					"** 확인 후 다시 시도해 주세요.", MainActivity.FINISH_SERVER);
		}
	}

	@Override
	public void run() {

		initServerSocket();

		// 무한 반복 작업으로 서버에 접속하는 클라이언트와 전송받는 메세지를 기다린다.
		while(running) {

			try {
				int n = selector.select(TIME_OUT);
				if(n == 0) // 그냥 타임아웃으로 나온것 뿐
					continue;

				Set<SelectionKey> keys = selector.selectedKeys();
				Iterator<SelectionKey> it = keys.iterator();

				while(it.hasNext()) {
					SelectionKey key = it.next();
					it.remove();

					if(!key.isValid())
						continue;

					if(key.isAcceptable()) {
						// 클라이언트로 부터 연결 요청 작업을 수행한다.
						clientNewAccept(key);
					}
					else if(key.isReadable()) {
						// 클라이언트가 전송한 메세지를 읽는다.
						readIncomingMessages(key);
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	private void clientNewAccept(SelectionKey key) throws Exception{

		SocketChannel channel;

		// 아래 주석은 이미 서버채널이 멤버변수로 있기 때문에 굳이 key에서 서버소켓채널을 안꺼내도 됨
		// ServerSocketChannel server = (ServerSocketChannel)key.channel();


		if((channel = serverchannel.accept()) != null) {

			String clientIp = channel.socket().getInetAddress().toString();
			sendHandlerMessage("** login from: " + clientIp, MainActivity.DEBUG_SERVER);

			// 서버에 접속한 클라이언트는 리스트로 관리
			clients.add(channel);

			channel.configureBlocking(false);
			channel.register(selector, SelectionKey.OP_READ);

			// TODO : Accept 됬으니까, 현재 도어락의 OTP와 위치를 날린다.
			sendMessage(channel, "otp;" + hostActivity.getPre(MainActivity.PREFERENCES_OTP) + ";");
			if(!hostActivity.getPre(MainActivity.PREFERENCES_LATITUDE).equals("default")
					&& !hostActivity.getPre(MainActivity.PREFERENCES_LONGITUDE).equals("default")) {

				sendMessage(channel, "door_location;"
						+ hostActivity.getPre(MainActivity.PREFERENCES_LATITUDE) + ";"
						+ hostActivity.getPre(MainActivity.PREFERENCES_LONGITUDE) + ";");
			}

			//  서버 디버그 창
			sendHandlerMessage("** 현재 참여한 그룹원은 " + clients.size() + " 입니다.", MainActivity.DEBUG_SERVER);
		}
	}

	private void readIncomingMessages(SelectionKey key) {
		SocketChannel channel = null;
		try {
			if(key == null) {
				sendHandlerMessage("** isReadable()로 들어 왔는데 왜 key가 없냐?", MainActivity.DEBUG_SERVER);
				return;
			}

			channel = (SocketChannel)key.channel();
			readBuffer.clear();

			// 채널로부터 메세지를 읽어 버퍼에 저장한다.
			long nbytes = channel.read(readBuffer);

			// 클라이언트 IP 주소를 알기 위해 소켓을 사용한다.
			Socket s = channel.socket();
			String clientIp = s.getInetAddress().toString();

			// 메세지를 저장하기 위한 닉네임 추출
			StringBuilder clientName = new StringBuilder();
			StringTokenizer st = new StringTokenizer(s.getInetAddress().toString(), "/");
			st = new StringTokenizer(st.nextToken(), ".");
			for(int i = 0; i < 4; i++)
				clientName.append(st.nextToken());

			// FIN 패킷을 받았기 때문에 채널을 닫고 키를 취소한다.
			if(nbytes == -1) {

				// 리스트에 존재하는 FIN 패킷을 전송한 소켓채널을 삭제시킨다.
				channel.close();
				key.cancel();
				clients.remove(channel);

				sendHandlerMessage("** " + clientIp + " 종료되었습니다.", MainActivity.DEBUG_SERVER);
				return;
			}

			// 'UTF-8'인 CharsetDecoder를 이용하여 바이트버퍼를 문자열로 만든다.
			readBuffer.flip();
			String line = decoder.decode(readBuffer).toString().trim();
			readBuffer.clear();

			// TODO : 여기서 도어락으로부터 받은 메세지를 해독한다 !!! >_<
			mStringBuilder.append(line);
			parsing(clientName.toString());

		} catch(IOException e) {
			if(channel != null) {
				try {
					String line = new String("** " + channel.socket().getInetAddress() + "이 비정상 종료되었습니다.");
					sendHandlerMessage(line, MainActivity.DEBUG_SERVER);

					channel.close();
					key.cancel();
					clients.remove(channel);

				} catch(IOException ignore) {
					ignore.printStackTrace();
				}
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	// 특정 클라이언트에 메세지를 전송한다.
	private void sendMessage(SocketChannel channel, String msg) {
		prepWriteBuffer(msg);
		channelWrite(channel);
	}

	// 서버에 접속한 모든 클라이언트에 메세지를 전송한다.
	public void sendBroadcastMessage(String msg) {
		prepWriteBuffer(msg);
		Iterator<SocketChannel> i = clients.iterator();
		while(i.hasNext()) {
			SocketChannel channel = i.next();
			channelWrite(channel);
		}
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

	public final static int INET4ADDRESS = 1;
	public final static int INET6ADDRESS = 2;

	public static String getLocalIpAddress(int type) {
		try {
			for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) {
				NetworkInterface intf = en.nextElement();
				for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();) {
					InetAddress inetAddress = enumIpAddr.nextElement();
					if (!inetAddress.isLoopbackAddress()) {
						switch (type) {
						case INET6ADDRESS:
							if (inetAddress instanceof Inet6Address) {
								return inetAddress.getHostAddress().toString();
							}
							break;
						case INET4ADDRESS:
							if (inetAddress instanceof Inet4Address) {
								return inetAddress.getHostAddress().toString();
							}
							break;
						}
					}
				}
			}
		} catch (SocketException e) {
			e.printStackTrace();
		}
		return null;
	}

	private void parsing(String clientName) {
		MessageParser mp = new MessageParser(mStringBuilder.toString());
		boolean loop = true;
		while(loop) {
			MessageParser.ParsingResult pr = mp.parsing();

			switch(pr.MESSAGE_PARSING_RESULT) {

			case MessageParser.MESSAGE_PARSER_IS_ENTERING: // 도어락이 근접경보를 요구해서 사용자가 준 것 (1개)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				String isEntering = (String)pr.msg;
				// sendHandlerMessage(isEntering, MainActivity.DEBUG_SERVER);

				if(!hostActivity.isProximityAlertTimeout) {// 근접경보정보를 요청하고 시간내에 온 유효한 값이면
					if(Boolean.valueOf(isEntering))
						hostActivity.putPre(MainActivity.PREFERENCES_IS_ENTERING, isEntering);
					// isEntering이 true인 사람이 한명만 있으면 된다 ~!!!!!!!!!!!!!!!!!!!! ***
				}

				sendHandlerMessage(clientName + " : " + isEntering, MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_OUT_MSG: // 사용자가 등록하는 나갈때 인사말 (1개)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				String outMsg = (String)pr.msg;

				// 서버에 접속한 클라이언트는 IP라는 이름으로 구분해서 각각 메세지를 저장한다
				if(!clientsName.contains(clientName))
					clientsName.add(clientName);
				hostActivity.putPre(clientName + MainActivity.PREFERENCES_OUT_MSG, outMsg);

				sendHandlerMessage(clientName + "(out_msg) : " + outMsg, MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_IN_MSG: // 사용자가 등록하는 들어올때 인사말 (1개)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				String inMsg = (String)pr.msg;

				// 서버에 접속한 클라이언트는 IP라는 이름으로 구분해서 각각 메세지를 저장한다
				if(!clientsName.contains(clientName))
					clientsName.add(clientName);
				hostActivity.putPre(clientName + MainActivity.PREFERENCES_IN_MSG, inMsg);

				sendHandlerMessage(clientName + "(in_msg) : " + inMsg, MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_REMOVE_MSG: // 사용자 인사말 모두 제거 (0개)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				if(clientsName.contains(clientName)) { // 사용자가 등록한 인사말이 있다면 ~!! 지우자
					hostActivity.removePre(clientName + MainActivity.PREFERENCES_OUT_MSG);
					hostActivity.removePre(clientName + MainActivity.PREFERENCES_IN_MSG);
					clientsName.remove(clientName);
				}

				sendHandlerMessage(clientName + "(remove_msg)", MainActivity.DEBUG_SERVER);
				break;
			case MessageParser.MESSAGE_PARSER_REQUEST_OTP: // 사용자가 otp 요구 (0개)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				sendHandlerMessage(clientName + "(request_otp)", MainActivity.REQUEST_OTP_FROM_CLIENT);
				sendHandlerMessage(clientName + "(request_otp)", MainActivity.DEBUG_SERVER);
				break;


			case MessageParser.MESSAGE_PARSER_IMPERFECT:

				loop = false;

				sendHandlerMessage(clientName + " : " + "아직 완벽하게 메세지가 전달 안 됨", MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_I_DONT_KNOW:

				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage(clientName + " : " + "뭔 내용인지 모르겠다", MainActivity.DEBUG_SERVER);
				break;
			default:

				// 뭔가 이상하니까 싹 다 비우자 ~!!
				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage(clientName + " : " + "default 있을 수 없는 일이야  ㅠ ㅠ", MainActivity.DEBUG_SERVER);
				break;
			}
		}
	}

	public void quit() {
		running = false;
		try { // 서버에 접속한 모든 클라이언트와 연결된 채널들을 닫는다.
			for(SocketChannel i : clients) {
				i.close();
			}

			// 서버소켓채널을 닫는다.
			serverchannel.close();
		} catch(IOException e) {
			e.printStackTrace();
		}
		interrupt();
	}
}
