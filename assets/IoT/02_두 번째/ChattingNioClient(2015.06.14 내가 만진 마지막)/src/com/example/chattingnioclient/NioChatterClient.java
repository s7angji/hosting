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
	
	private StringBuilder mStringBuilder; // ���� ������ ��

	private final String serverIpAdress;
	private final int port = 6000;

	// Thread ���Ḧ ���� flag
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

	// ������
	public NioChatterClient(ChattingNioClientActivity hostActivity, String serverIpAdress) {
		
		mStringBuilder = new StringBuilder();

		this.hostActivity = hostActivity;
		this.serverIpAdress = serverIpAdress;
		this.hostHandler = hostActivity.mHandler;

		readBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);
		writeBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);

		// ��Ʈ��ũ�� �����ϰ� ���Ź޴� charset�� ��� 'UTF-8�� ����Ѵ�.
		Charset charset = Charset.forName("UTF-8");
		decoder = charset.newDecoder();
		encoder = charset.newEncoder();
	}

	private void initClientSocket() {
		try { // ����ä���� �����.
			clientchannel = SocketChannel.open();
			clientchannel.configureBlocking(false);
			clientchannel.connect(new InetSocketAddress(InetAddress.getByName(serverIpAdress), port));
			selector = Selector.open();
			clientchannel.register(selector, SelectionKey.OP_CONNECT);

			running = true;
		} catch(Exception e) {
			running = true; // TODO : �켱 ����־� �� ���� ���� �����尡 �� ���δ�

			sendHandlerMessage("����ä�� ���� �� ���� �߻�\n" +
					"** IP �ּҰ� �ٸ��� ���� ���ɼ��� �����ϴ�.\n" +
					"** Ȯ�� �� �ٽ� �õ��� �ּ���.", ChattingNioClientActivity.FINISH_MSG);
		}
	}

	@Override
	public void run() {

		initClientSocket();

		// ���� �ݺ� �۾����� ������ ���� ��û�̳�, �������� ���� �޼����� ��ٸ���.
		while(running) {

			try {
				int n = selector.select(TIME_OUT);
				if(n == 0) {
					if(isConnected) continue; // ������ ������ ��µ� �׳� Ÿ�Ӿƿ����� ���°� ��
					else {
						sendHandlerMessage("Time-Out �ȿ� ������� �������� ���߽��ϴ�.\n" +
								"** IP �ּҸ� �߸� �Է��ϼ��� ���� �ֽ��ϴ�.\n" +
								"** ����� �� ����� ���ͳ��� off������ ���� �ֽ��ϴ�.\n" +
								"** ����� �� ����� ������ off������ ���� �ֽ��ϴ�.\n" +
								"** �Ǵ� ���ͳݻ� ������ ���Դϴ�.\n" +
								"** Ȯ�� �� �ٽ� �õ��� �ּ���", ChattingNioClientActivity.SERVER_CONNECTED_TIMEOUT_MSG); // 5�� �ȿ� ������ ������ �� �ߴ�.
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
						// ������ ������ ��û�Ѵ�.
						serverNewConnection(key);
					}
					else if(key.isReadable()) {
						// ������ ������ �޼����� �д´�.
						readIncomingMessages(key);
					}
				}
			} catch (Exception e) {
				sendHandlerMessage("Connect�� �����߽��ϴ�.\n" +
						"** ����� ���� ������� �ʰ� ���� ���� �ֽ��ϴ�.\n" +
						"** ����� ���� ���ͳ��� ������� ���ϰ� ���� ���� �ֽ��ϴ�.\n" +
						"** �Ǵ� ���ͳݻ� ������ ���Դϴ�.\n" +
						"** Ȯ�� �� �ٽ� �õ����ּ���", ChattingNioClientActivity.FINISH_MSG);
				e.printStackTrace();
			}
		}
	}

	private void serverNewConnection(SelectionKey key) throws IOException{

		SocketChannel channel = (SocketChannel)key.channel();

		while(channel.isConnectionPending())
			channel.finishConnect();

		if(channel.isConnected()) {
			sendHandlerMessage("���������� ������ �����Ͽ����ϴ�.", ChattingNioClientActivity.CONNECT_MSG);
			isConnected = true;
		}
		else {
			key.cancel();
			sendHandlerMessage("(Connect�õ� ��) ������ ������ �����մϴ�.", ChattingNioClientActivity.FINISH_MSG);
		}

		// ���ӿ� ���������Ƿ� ���� ä�ηκ��� ���� �� �ִ�.
		channel.register(selector, SelectionKey.OP_READ);

	}

	private void readIncomingMessages(SelectionKey key) {
		SocketChannel channel = null;
		try {
			if(key == null) {
				sendHandlerMessage("isReadable()�� ��� �Դµ� �� key�� ����?", ChattingNioClientActivity.MSG);
				return;
			}

			channel = (SocketChannel)key.channel();
			readBuffer.clear();

			// ä�ηκ��� �޼����� �о� ���ۿ� �����Ѵ�.
			long nbytes = channel.read(readBuffer);

			// FIN ��Ŷ�� �޾ұ� ������ ä���� �ݰ� Ű�� ����Ѵ�.
			if(nbytes == -1) {
				key.cancel();
				sendHandlerMessage("�������� ������׽��ϴ�", ChattingNioClientActivity.FINISH_MSG);
			}

			// 'UTF-8'�� CharsetDecoder�� �̿��Ͽ� ����Ʈ���۸� ���ڿ��� �����.
			readBuffer.flip();
			String line = decoder.decode(readBuffer).toString().trim();
			readBuffer.clear();

			// TODO : ���⼭ ��������κ��� ���� �޼����� �ص��Ѵ� !!! >_<
			mStringBuilder.append(line);
			parsing();
			sendHandlerMessage(mStringBuilder.toString(), ChattingNioClientActivity.MSG);
			
		} catch(IOException e) {
			if(channel != null) {
				key.cancel();
				sendHandlerMessage("������ ������ ����Ǿ����ϴ�.", ChattingNioClientActivity.FINISH_MSG);
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
		// ���ڿ��� Charbuffer�� ������� �ٽ� ���ڵ��Ͽ� ����Ʈ���۸� �����.
		writeBuffer.clear();
		// ���ڿ��� CharBuffer�� �����.
		CharBuffer requestChars = CharBuffer.wrap(msg);
		try { // �ڹ��� ���ڿ��� 'UTF-8' ���ڵ带 ����Ͽ� ����Ʈ���۷� �����.
			ByteBuffer cbuf = encoder.encode(requestChars);
			// ���ڵ��� ����Ʈ���۸� ���� ����Ʈ���۷� �ٽ� ����� �ش�.
			writeBuffer.put(cbuf);
		} catch(CharacterCodingException e) {
			e.printStackTrace();
		}
		// CR�� LF�� �־� ��Ʈ�������� ���� �� �ֵ��� ����� �ش�.
		writeBuffer.putChar('\r');
		writeBuffer.putChar('\n');
		writeBuffer.flip();
	}

	private void channelWrite(SocketChannel channel) {
		long nbytes = 0;
		long toWrite = writeBuffer.remaining();

		// ������ �޼������� ���ٸ�, Ŀ���� �޼����� ����� ������.
		// ���� ������ ���� ó���Ѵ�.
		try {
			while(nbytes != toWrite) {
				nbytes += channel.write(writeBuffer);
			}
		} catch(Exception e) { }

		writeBuffer.rewind(); // �ٽ� �� �� �ֵ��� ����
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
				// �����۷����� OTP ����
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
				// �����۷����� ���� ����
				hostActivity.putPre(ChattingNioClientActivity.PREFERENCES_LATITUDE, String.valueOf(doorLocation.getLatitude()));
				// �����۷����� �浵 ����
				hostActivity.putPre(ChattingNioClientActivity.PREFERENCES_LONGITUDE, String.valueOf(doorLocation.getLongitude()));

				sendHandlerMessage("���� �浵 �� �����ߴ�", ChattingNioClientActivity.DOOR_LOCATION_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_PROXIMITY_ALERT:

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				sendHandlerMessage("�����溸�� ��� �˷��޶�� �׷���", ChattingNioClientActivity.PROXIMITY_ALERT_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_SUSPICIOUS_APPROACH:

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				sendHandlerMessage("�ǽɽ����� �����̶�� �˷��ֳ�", ChattingNioClientActivity.SUSPICIOUS_APPROACH_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_EMPTY:

				loop = false;

				sendHandlerMessage("����ִ� ������ �Ľ��϶�� �׷���. ����?", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_IMPERFECT:

				loop = false;

				sendHandlerMessage("���� �Ϻ��ϰ� ���� �޼����� ���� �� ��", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;
			case MessageParser.MESSAGE_PARSER_I_DONT_KNOW:

				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage("�� �������� �𸣰ڴ�", ChattingNioClientActivity.DEBUG_FROM_SERVER);
				break;
			default:

				// ���� �̻��ϴϱ� �� �� ����� ~!!
				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage("default ���� �� ���� ���̾�  �� ��", ChattingNioClientActivity.DEBUG_FROM_SERVER);
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
