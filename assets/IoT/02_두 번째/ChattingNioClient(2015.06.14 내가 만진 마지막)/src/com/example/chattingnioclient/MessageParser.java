package com.example.chattingnioclient;

import java.util.StringTokenizer;

public class MessageParser {
	public static final int MESSAGE_PARSER_OTP = 1; // 도어락이 OTP를 알려줌
	public static final int MESSAGE_PARSER_DOOR_LOCATION = 2; // 도어락이 자신의 위치 정보를 알려줌
	public static final int MESSAGE_PARSER_PROXIMITY_ALERT = 3; // 현재 내가 도어락 근처에 있는지, 도어락이 근접경보를 요청함
	public static final int MESSAGE_PARSER_SUSPICIOUS_APPROACH = 4; // 수상한 접근이 의심된다고 도어락이 알림

	public static final int MESSAGE_PARSER_EMPTY = 5; // 해독할 메세지가 비어있다.
	public static final int MESSAGE_PARSER_IMPERFECT = 6; // 받은 메세지가 완벽하지 않다.
	public static final int MESSAGE_PARSER_I_DONT_KNOW = 7; // 어떤 명령인지 알 수가 없다.

	private StringTokenizer mStringTokenizer;

	// 생성자
	public MessageParser(String msg) {
		mStringTokenizer = new StringTokenizer(msg, ";", true); // 구분자가 ";" 이 문자열이다 >_<
	}

	public void setMessage(String msg) { mStringTokenizer = new StringTokenizer(msg, ";", true); }// 구분자가 ";" 이 문자열이다 >_<

	private static final String CMD_OTP = "otp";
	private static final String CMD_DOOR_LOCATION = "door_location";
	private static final String CMD_PROXIMITY_ALERT = "proximity_alert";
	private static final String CMD_SUSPICIOUS_APPROACH = "suspicious_approach";

	public ParsingResult parsing() {




		if(mStringTokenizer.countTokens() % 2 != 0) // 구분자를 true로 했으므로 모든 토큰들은 짝이 맞아야 한다.
			return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null, null);
		else if(mStringTokenizer.countTokens() == 0) // 해독할 메세지가 비어있다.
			return new ParsingResult(MESSAGE_PARSER_EMPTY, false, null, null);




		String cmd = nextToken();
		switch(cmd) {
		/***************************************************************************/
		case CMD_OTP:
			if(countTokens() >= 1) { // OTP는 하나의 정보만 있으면 된다 !!

				String otp = nextToken(); // OTP를 얻고 ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_OTP, isRemain, new Otp(otp), null);
			}
			else { // 얻을 정보가 부족하다.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null, null);
			}
		/***************************************************************************/
		case CMD_DOOR_LOCATION:
			if(countTokens() >= 2) { // 도어락 위치는 위도, 경도 이렇게 두가지가 필요하다!!
				double latitude = Double.parseDouble(nextToken()); // 위도를 얻고 ~
				double longitude = Double.parseDouble(nextToken()); // 경도를 얻고 ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_DOOR_LOCATION, isRemain, null, new DoorLocation(latitude, longitude));
			}
			else { // 얻을 정보가 부족하다.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null, null);
			}
		/***************************************************************************/
		case CMD_PROXIMITY_ALERT:
			// 지금 도어락이 현재 사용자가 도어락에 근접했는지 아닌지를 알고 싶어한다는 것 !!
			boolean isRemain;
			if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
			else isRemain = true;
			return new ParsingResult(MESSAGE_PARSER_PROXIMITY_ALERT, isRemain, null, null);
		/***************************************************************************/
		case CMD_SUSPICIOUS_APPROACH:
			// 지금 도어락에 문열려는 놈이 수상하다고 알린거
			if(countTokens() == 0) isRemain = false; // 모든걸 처리 했는지 판단하고 ~
			else isRemain = true;
			return new ParsingResult(MESSAGE_PARSER_SUSPICIOUS_APPROACH, isRemain, null, null);
		/***************************************************************************/
		default:
			return new ParsingResult(MESSAGE_PARSER_I_DONT_KNOW, false, null, null);
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
		public Otp otp;
		public DoorLocation doorLocation;

		public boolean isRemain;
		public int readPosition;

		public ParsingResult(int MESSAGE_PARSING_RESULT, boolean isRemain, Otp otp, DoorLocation doorLocation) {
			this.MESSAGE_PARSING_RESULT = MESSAGE_PARSING_RESULT;
			this.isRemain = isRemain;
			readPosition = mReadPosition;
			this.otp = otp;
			this.doorLocation = doorLocation;
		}
	}

	public class Otp {
		private String otp;

		public Otp(String otp) { this.otp = otp; }

		public String getOtp() { return otp; }
		public void setOtp(String otp) { this.otp = otp; }

	}

	public class DoorLocation {
		private double latitude;
		private double longitude;

		public DoorLocation(double latitude, double longitude) {
			this.latitude = latitude;
			this.longitude = longitude;
		}

		public double getLatitude() { return latitude; }
		public void setLatitude(double latitude) { this.latitude = latitude; }
		public double getLongitude() { return longitude; }
		public void setLongitude(double longitude) { this.longitude = longitude; }
	}

}
