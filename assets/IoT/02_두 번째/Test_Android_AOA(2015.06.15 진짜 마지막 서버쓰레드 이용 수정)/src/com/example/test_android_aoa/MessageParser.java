package com.example.test_android_aoa;

import java.util.StringTokenizer;

public class MessageParser {
	public static final int MESSAGE_PARSER_IS_ENTERING = 1; // 도어락이 근접경보를 요구해서 사용자가 준 것 (1개)
	public static final int MESSAGE_PARSER_OUT_MSG = 2; // 사용자가 등록하는 나갈때 인사말 (1개)
	public static final int MESSAGE_PARSER_IN_MSG = 3; // 사용자가 등록하는 들어올때 인사말 (1개)
	public static final int MESSAGE_PARSER_REMOVE_MSG = 4; // 사용자 인사말 모두 제거 (0개)
	public static final int MESSAGE_PARSER_REQUEST_OTP = 5; // 사용자가 otp 요구 (0개)

	public static final int MESSAGE_PARSER_EMPTY = 6; // 해독할 메세지가 비어있다.
	public static final int MESSAGE_PARSER_IMPERFECT = 7; // 받은 메세지가 완벽하지 않다.
	public static final int MESSAGE_PARSER_I_DONT_KNOW = 8; // 어떤 명령인지 알 수가 없다.

	private StringTokenizer mStringTokenizer;

	// 생성자
	public MessageParser(String msg) {
		mStringTokenizer = new StringTokenizer(msg, ";", true); // 구분자가 ";" 이 문자열이다 >_<
	}

	public void setMessage(String msg) { mStringTokenizer = new StringTokenizer(msg, ";", true); }// 구분자가 ";" 이 문자열이다 >_<

	private static final String CMD_IS_ENTERING = "is_entering";
	private static final String CMD_OUT_MSG = "out_msg";
	private static final String CMD_IN_MSG = "in_msg";
	private static final String CMD_REMOVE_MSG = "remove_msg";
	private static final String CMD_REQUEST_OTP = "request_otp";

	public ParsingResult parsing() {




		if(mStringTokenizer.countTokens() % 2 != 0) // 구분자를 true로 했으므로 모든 토큰들은 짝이 맞아야 한다.
			return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
		else if(mStringTokenizer.countTokens() == 0) // 해독할 메세지가 비어있다.
			return new ParsingResult(MESSAGE_PARSER_EMPTY, false, null);




		String cmd = nextToken();
		switch(cmd) {
		/***************************************************************************/
		case CMD_IS_ENTERING:
			if(countTokens() >= 1) { // 사용자가 도어락에 범위안에 있는지 하나의 boolean 정보만 있으면 된다 !!

				String isEntering = nextToken(); // OTP를 얻고 ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_IS_ENTERING, isRemain, isEntering);
			}
			else { // 얻을 정보가 부족하다.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
			}
		/***************************************************************************/
		case CMD_OUT_MSG:
			if(countTokens() >= 1) { // 사용자가 나갈때 하나의 메세지만 있으면 된다 !!

				String outMsg = nextToken(); // 나갈때 메세지를 얻고 ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_OUT_MSG, isRemain, outMsg);
			}
			else { // 얻을 정보가 부족하다.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
			}
		/***************************************************************************/
		case CMD_IN_MSG:
			if(countTokens() >= 1) { // 사용자가 들어올때 하나의 메세지만 있으면 된다 !!

				String inMsg = nextToken(); // 들어올때를 얻고 ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_IN_MSG, isRemain, inMsg);
			}
			else { // 얻을 정보가 부족하다.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null);
			}
		/***************************************************************************/
		case CMD_REMOVE_MSG:
			// 사용자가 등록한 나갈때, 들어올때 메세지 등록 취소
			boolean isRemain;
			if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
			else isRemain = true;
			return new ParsingResult(MESSAGE_PARSER_REMOVE_MSG, isRemain, null);
		/***************************************************************************/
		case CMD_REQUEST_OTP:
			// 사용자가 OTP를 요구했다
			if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
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
		mStringTokenizer.nextToken(); // 구분자를 true로 했기 때문에 ~!! 이 녀석은 ";" 이거다.. 필요없는거
		mReadPosition += realToken.length() + 1; // 읽은 만큼 포지션도 증가시키고 ~
		return realToken;
	}

	// 구분자를 true로 했기 때문에 구분자도 숫자에 카운트되서 두배로 뻥튀기된다.
	// 따라서 2로 나눔
	private int countTokens() { return mStringTokenizer.countTokens() / 2; }

	// 여기 클래스에다가 담아줌. 약간 구조체 느낌나는 거
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