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

	// ������ ������ ��� Ŭ���̾�Ʈ�� ����Ʈ�� �����Ѵ�.
	private final LinkedList<SocketChannel> clients;

	private StringBuilder mStringBuilder; // ���� ������ ��
	private LinkedList<String> clientsName;

	// Thread ���Ḧ ���� flag
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

	// ������
	public NioChatterServer(MainActivity hostActivity) {

		mStringBuilder = new StringBuilder();

		this.hostActivity = hostActivity;
		this.hostHandler = hostActivity.mHandler;
		this.clientsName = hostActivity.clientsName;

		ipAddress = null;

		readBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);
		writeBuffer = ByteBuffer.allocateDirect(BUFFER_SIZE);

		// ��Ʈ��ũ�� �����ϰ� ���Ź޴� charset�� ��� 'UTF-8�� ����Ѵ�.
		Charset charset = Charset.forName("UTF-8");
		decoder = charset.newDecoder();
		encoder = charset.newEncoder();

		// ������ ������ ����ڵ��� �����ϱ� ���� ����Ʈ�� ���� ä���� �����Ѵ�.
		clients = new LinkedList<SocketChannel>();
	}

	private void initServerSocket() {
		try { // ��������ä���� �����.
			ipAddress = getLocalIpAddress(INET4ADDRESS);

			serverchannel = ServerSocketChannel.open();
			serverchannel.configureBlocking(false);
			serverchannel.socket().bind(new InetSocketAddress(ipAddress, port));
			selector = Selector.open();
			serverchannel.register(selector, SelectionKey.OP_ACCEPT);

			running = true;

			// ���� ������Ĺä���� ��Ĺ�� ���ε� �� IP �ּҸ� Activity�� �˷���
			sendHandlerMessage(ipAddress, MainActivity.GET_IP_SERVER);
		} catch(Exception e) {
			running = true; // �켱 ����־� �� ���� ���� �����尡 �� ���δ�

			sendHandlerMessage("����ä�� ���� �� ���� �߻�\n" +
					"** Ȯ�� �� �ٽ� �õ��� �ּ���.", MainActivity.FINISH_SERVER);
		}
	}

	@Override
	public void run() {

		initServerSocket();

		// ���� �ݺ� �۾����� ������ �����ϴ� Ŭ���̾�Ʈ�� ���۹޴� �޼����� ��ٸ���.
		while(running) {

			try {
				int n = selector.select(TIME_OUT);
				if(n == 0) // �׳� Ÿ�Ӿƿ����� ���°� ��
					continue;

				Set<SelectionKey> keys = selector.selectedKeys();
				Iterator<SelectionKey> it = keys.iterator();

				while(it.hasNext()) {
					SelectionKey key = it.next();
					it.remove();

					if(!key.isValid())
						continue;

					if(key.isAcceptable()) {
						// Ŭ���̾�Ʈ�� ���� ���� ��û �۾��� �����Ѵ�.
						clientNewAccept(key);
					}
					else if(key.isReadable()) {
						// Ŭ���̾�Ʈ�� ������ �޼����� �д´�.
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

		// �Ʒ� �ּ��� �̹� ����ä���� ��������� �ֱ� ������ ���� key���� ��������ä���� �Ȳ����� ��
		// ServerSocketChannel server = (ServerSocketChannel)key.channel();


		if((channel = serverchannel.accept()) != null) {

			String clientIp = channel.socket().getInetAddress().toString();
			sendHandlerMessage("** login from: " + clientIp, MainActivity.DEBUG_SERVER);

			// ������ ������ Ŭ���̾�Ʈ�� ����Ʈ�� ����
			clients.add(channel);

			channel.configureBlocking(false);
			channel.register(selector, SelectionKey.OP_READ);

			// TODO : Accept �����ϱ�, ���� ������� OTP�� ��ġ�� ������.
			sendMessage(channel, "otp;" + hostActivity.getPre(MainActivity.PREFERENCES_OTP) + ";");
			if(!hostActivity.getPre(MainActivity.PREFERENCES_LATITUDE).equals("default")
					&& !hostActivity.getPre(MainActivity.PREFERENCES_LONGITUDE).equals("default")) {

				sendMessage(channel, "door_location;"
						+ hostActivity.getPre(MainActivity.PREFERENCES_LATITUDE) + ";"
						+ hostActivity.getPre(MainActivity.PREFERENCES_LONGITUDE) + ";");
			}

			//  ���� ����� â
			sendHandlerMessage("** ���� ������ �׷���� " + clients.size() + " �Դϴ�.", MainActivity.DEBUG_SERVER);
		}
	}

	private void readIncomingMessages(SelectionKey key) {
		SocketChannel channel = null;
		try {
			if(key == null) {
				sendHandlerMessage("** isReadable()�� ��� �Դµ� �� key�� ����?", MainActivity.DEBUG_SERVER);
				return;
			}

			channel = (SocketChannel)key.channel();
			readBuffer.clear();

			// ä�ηκ��� �޼����� �о� ���ۿ� �����Ѵ�.
			long nbytes = channel.read(readBuffer);

			// Ŭ���̾�Ʈ IP �ּҸ� �˱� ���� ������ ����Ѵ�.
			Socket s = channel.socket();
			String clientIp = s.getInetAddress().toString();

			// �޼����� �����ϱ� ���� �г��� ����
			StringBuilder clientName = new StringBuilder();
			StringTokenizer st = new StringTokenizer(s.getInetAddress().toString(), "/");
			st = new StringTokenizer(st.nextToken(), ".");
			for(int i = 0; i < 4; i++)
				clientName.append(st.nextToken());

			// FIN ��Ŷ�� �޾ұ� ������ ä���� �ݰ� Ű�� ����Ѵ�.
			if(nbytes == -1) {

				// ����Ʈ�� �����ϴ� FIN ��Ŷ�� ������ ����ä���� ������Ų��.
				channel.close();
				key.cancel();
				clients.remove(channel);

				sendHandlerMessage("** " + clientIp + " ����Ǿ����ϴ�.", MainActivity.DEBUG_SERVER);
				return;
			}

			// 'UTF-8'�� CharsetDecoder�� �̿��Ͽ� ����Ʈ���۸� ���ڿ��� �����.
			readBuffer.flip();
			String line = decoder.decode(readBuffer).toString().trim();
			readBuffer.clear();

			// TODO : ���⼭ ��������κ��� ���� �޼����� �ص��Ѵ� !!! >_<
			mStringBuilder.append(line);
			parsing(clientName.toString());

		} catch(IOException e) {
			if(channel != null) {
				try {
					String line = new String("** " + channel.socket().getInetAddress() + "�� ������ ����Ǿ����ϴ�.");
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

	// Ư�� Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
	private void sendMessage(SocketChannel channel, String msg) {
		prepWriteBuffer(msg);
		channelWrite(channel);
	}

	// ������ ������ ��� Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
	public void sendBroadcastMessage(String msg) {
		prepWriteBuffer(msg);
		Iterator<SocketChannel> i = clients.iterator();
		while(i.hasNext()) {
			SocketChannel channel = i.next();
			channelWrite(channel);
		}
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

			case MessageParser.MESSAGE_PARSER_IS_ENTERING: // ������� �����溸�� �䱸�ؼ� ����ڰ� �� �� (1��)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				String isEntering = (String)pr.msg;
				// sendHandlerMessage(isEntering, MainActivity.DEBUG_SERVER);

				if(!hostActivity.isProximityAlertTimeout) {// �����溸������ ��û�ϰ� �ð����� �� ��ȿ�� ���̸�
					if(Boolean.valueOf(isEntering))
						hostActivity.putPre(MainActivity.PREFERENCES_IS_ENTERING, isEntering);
					// isEntering�� true�� ����� �Ѹ� ������ �ȴ� ~!!!!!!!!!!!!!!!!!!!! ***
				}

				sendHandlerMessage(clientName + " : " + isEntering, MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_OUT_MSG: // ����ڰ� ����ϴ� ������ �λ縻 (1��)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				String outMsg = (String)pr.msg;

				// ������ ������ Ŭ���̾�Ʈ�� IP��� �̸����� �����ؼ� ���� �޼����� �����Ѵ�
				if(!clientsName.contains(clientName))
					clientsName.add(clientName);
				hostActivity.putPre(clientName + MainActivity.PREFERENCES_OUT_MSG, outMsg);

				sendHandlerMessage(clientName + "(out_msg) : " + outMsg, MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_IN_MSG: // ����ڰ� ����ϴ� ���ö� �λ縻 (1��)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				String inMsg = (String)pr.msg;

				// ������ ������ Ŭ���̾�Ʈ�� IP��� �̸����� �����ؼ� ���� �޼����� �����Ѵ�
				if(!clientsName.contains(clientName))
					clientsName.add(clientName);
				hostActivity.putPre(clientName + MainActivity.PREFERENCES_IN_MSG, inMsg);

				sendHandlerMessage(clientName + "(in_msg) : " + inMsg, MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_REMOVE_MSG: // ����� �λ縻 ��� ���� (0��)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				if(clientsName.contains(clientName)) { // ����ڰ� ����� �λ縻�� �ִٸ� ~!! ������
					hostActivity.removePre(clientName + MainActivity.PREFERENCES_OUT_MSG);
					hostActivity.removePre(clientName + MainActivity.PREFERENCES_IN_MSG);
					clientsName.remove(clientName);
				}

				sendHandlerMessage(clientName + "(remove_msg)", MainActivity.DEBUG_SERVER);
				break;
			case MessageParser.MESSAGE_PARSER_REQUEST_OTP: // ����ڰ� otp �䱸 (0��)

				loop = pr.isRemain;
				mStringBuilder.delete(0, pr.readPosition);
				if(pr.isRemain) mp.setMessage(mStringBuilder.toString());

				sendHandlerMessage(clientName + "(request_otp)", MainActivity.REQUEST_OTP_FROM_CLIENT);
				sendHandlerMessage(clientName + "(request_otp)", MainActivity.DEBUG_SERVER);
				break;


			case MessageParser.MESSAGE_PARSER_IMPERFECT:

				loop = false;

				sendHandlerMessage(clientName + " : " + "���� �Ϻ��ϰ� �޼����� ���� �� ��", MainActivity.DEBUG_SERVER);
				break;

			case MessageParser.MESSAGE_PARSER_I_DONT_KNOW:

				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage(clientName + " : " + "�� �������� �𸣰ڴ�", MainActivity.DEBUG_SERVER);
				break;
			default:

				// ���� �̻��ϴϱ� �� �� ����� ~!!
				loop = false;
				mStringBuilder = null;
				mStringBuilder = new StringBuilder();

				sendHandlerMessage(clientName + " : " + "default ���� �� ���� ���̾�  �� ��", MainActivity.DEBUG_SERVER);
				break;
			}
		}
	}

	public void quit() {
		running = false;
		try { // ������ ������ ��� Ŭ���̾�Ʈ�� ����� ä�ε��� �ݴ´�.
			for(SocketChannel i : clients) {
				i.close();
			}

			// ��������ä���� �ݴ´�.
			serverchannel.close();
		} catch(IOException e) {
			e.printStackTrace();
		}
		interrupt();
	}
}
