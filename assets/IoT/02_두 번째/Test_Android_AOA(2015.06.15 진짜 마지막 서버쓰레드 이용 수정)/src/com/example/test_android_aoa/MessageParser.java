package com.example.test_android_aoa;

import java.util.StringTokenizer;

public class MessageParser {
	public static final int MESSAGE_PARSER_IS_ENTERING = 1; // ������� �����溸�� �䱸�ؼ� ����ڰ� �� �� (1��)
	public static final int MESSAGE_PARSER_OUT_MSG = 2; // ����ڰ� ����ϴ� ������ �λ縻 (1��)
	public static final int MESSAGE_PARSER_IN_MSG = 3; // ����ڰ� ����ϴ� ���ö� �λ縻 (1��)
	public static final int MESSAGE_PARSER_REMOVE_MSG = 4; // ����� �λ縻 ��� ���� (0��)
	public static final int MESSAGE_PARSER_REQUEST_OTP = 5; // ����ڰ� otp �䱸 (0��)

	public static final int MESSAGE_PARSER_EMPTY = 6; // �ص��� �޼����� ����ִ�.
	public static final int MESSAGE_PARSER_IMPERFECT = 7; // ���� �޼����� �Ϻ����� �ʴ�.
	public static final int MESSAGE_PARSER_I_DONT_KNOW = 8; // � ������� �� ���� ����.

	private StringTokenizer mStringTokenizer;

	// ������
	public MessageParser(String msg) {
		mStringTokenizer = new StringTokenizer(msg, ";", true); // �����ڰ� ";" �� ���ڿ��̴� >_<
	}

	public void setMessage(String msg) { mStringTokenizer = new StringTokenizer(msg, ";", true); }// �����ڰ� ";" �� ���ڿ��̴� >_<

	private static final String CMD_IS_ENTERING = "is_entering";
	private static final String CMD_OUT_MSG = "out_msg";
	private static final String CMD_IN_MSG = "in_msg";
	private static final String CMD_REMOVE_MSG = "remove_msg";
	private static final String CMD_REQUEST_OTP = "request_otp";

	public ParsingResult parsing() {




		if(mStringTokenizer.countTokens() % 2 != 0) // �����ڸ� true�� �����Ƿ� ��� ��ū���� ¦�� �¾ƾ� �Ѵ�.
			return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
		else if(mStringTokenizer.countTokens() == 0) // �ص��� �޼����� ����ִ�.
			return new ParsingResult(MESSAGE_PARSER_EMPTY, false, null);




		String cmd = nextToken();
		switch(cmd) {
		/***************************************************************************/
		case CMD_IS_ENTERING:
			if(countTokens() >= 1) { // ����ڰ� ������� �����ȿ� �ִ��� �ϳ��� boolean ������ ������ �ȴ� !!

				String isEntering = nextToken(); // OTP�� ��� ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_IS_ENTERING, isRemain, isEntering);
			}
			else { // ���� ������ �����ϴ�.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
			}
		/***************************************************************************/
		case CMD_OUT_MSG:
			if(countTokens() >= 1) { // ����ڰ� ������ �ϳ��� �޼����� ������ �ȴ� !!

				String outMsg = nextToken(); // ������ �޼����� ��� ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_OUT_MSG, isRemain, outMsg);
			}
			else { // ���� ������ �����ϴ�.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
			}
		/***************************************************************************/
		case CMD_IN_MSG:
			if(countTokens() >= 1) { // ����ڰ� ���ö� �ϳ��� �޼����� ������ �ȴ� !!

				String inMsg = nextToken(); // ���ö��� ��� ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_IN_MSG, isRemain, inMsg);
			}
			else { // ���� ������ �����ϴ�.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
			}
		/***************************************************************************/
		case CMD_REMOVE_MSG:
			// ����ڰ� ����� ������, ���ö� �޼��� ��� ���
			boolean isRemain;
			if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
			else isRemain = true;
			return new ParsingResult(MESSAGE_PARSER_REMOVE_MSG, isRemain, null);
		/***************************************************************************/
		case CMD_REQUEST_OTP:
			// ����ڰ� OTP�� �䱸�ߴ�
			if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
			else isRemain = true;
			return new ParsingResult(MESSAGE_PARSER_REQUEST_OTP, isRemain, null);
		/***************************************************************************/
		default:
			return new ParsingResult(MESSAGE_PARSER_I_DONT_KNOW, false, null);
		/***************************************************************************/
		}
	}

	private int mReadPosition = 0;
	private String nextToken() {
		String realToken = mStringTokenizer.nextToken();
		mStringTokenizer.nextToken(); // �����ڸ� true�� �߱� ������ ~!! �� �༮�� ";" �̰Ŵ�.. �ʿ���°�
		mReadPosition += realToken.length() + 1; // ���� ��ŭ �����ǵ� ������Ű�� ~
		return realToken;
	}

	// �����ڸ� true�� �߱� ������ �����ڵ� ���ڿ� ī��Ʈ�Ǽ� �ι�� ��Ƣ��ȴ�.
	// ���� 2�� ����
	private int countTokens() { return mStringTokenizer.countTokens() / 2; }

	// ���� Ŭ�������ٰ� �����. �ణ ����ü �������� ��
	public class ParsingResult {
		public int MESSAGE_PARSING_RESULT;

		public boolean isRemain;
		public int readPosition;
		public String msg;

		public ParsingResult(int MESSAGE_PARSING_RESULT, boolean isRemain, String msg) {
			this.MESSAGE_PARSING_RESULT = MESSAGE_PARSING_RESULT;
			this.isRemain = isRemain;
			readPosition = mReadPosition;
			this.msg = msg;
		}
	}

}