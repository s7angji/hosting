package com.example.chattingnioclient;

import java.util.StringTokenizer;

public class MessageParser {
	public static final int MESSAGE_PARSER_OTP = 1; // ������� OTP�� �˷���
	public static final int MESSAGE_PARSER_DOOR_LOCATION = 2; // ������� �ڽ��� ��ġ ������ �˷���
	public static final int MESSAGE_PARSER_PROXIMITY_ALERT = 3; // ���� ���� ����� ��ó�� �ִ���, ������� �����溸�� ��û��
	public static final int MESSAGE_PARSER_SUSPICIOUS_APPROACH = 4; // ������ ������ �ǽɵȴٰ� ������� �˸�

	public static final int MESSAGE_PARSER_EMPTY = 5; // �ص��� �޼����� ����ִ�.
	public static final int MESSAGE_PARSER_IMPERFECT = 6; // ���� �޼����� �Ϻ����� �ʴ�.
	public static final int MESSAGE_PARSER_I_DONT_KNOW = 7; // � ������� �� ���� ����.

	private StringTokenizer mStringTokenizer;

	// ������
	public MessageParser(String msg) {
		mStringTokenizer = new StringTokenizer(msg, ";", true); // �����ڰ� ";" �� ���ڿ��̴� >_<
	}

	public void setMessage(String msg) { mStringTokenizer = new StringTokenizer(msg, ";", true); }// �����ڰ� ";" �� ���ڿ��̴� >_<

	private static final String CMD_OTP = "otp";
	private static final String CMD_DOOR_LOCATION = "door_location";
	private static final String CMD_PROXIMITY_ALERT = "proximity_alert";
	private static final String CMD_SUSPICIOUS_APPROACH = "suspicious_approach";

	public ParsingResult parsing() {




		if(mStringTokenizer.countTokens() % 2 != 0) // �����ڸ� true�� �����Ƿ� ��� ��ū���� ¦�� �¾ƾ� �Ѵ�.
			return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null, null);
		else if(mStringTokenizer.countTokens() == 0) // �ص��� �޼����� ����ִ�.
			return new ParsingResult(MESSAGE_PARSER_EMPTY, false, null, null);




		String cmd = nextToken();
		switch(cmd) {
		/***************************************************************************/
		case CMD_OTP:
			if(countTokens() >= 1) { // OTP�� �ϳ��� ������ ������ �ȴ� !!

				String otp = nextToken(); // OTP�� ��� ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_OTP, isRemain, new Otp(otp), null);
			}
			else { // ���� ������ �����ϴ�.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null, null);
			}
		/***************************************************************************/
		case CMD_DOOR_LOCATION:
			if(countTokens() >= 2) { // ����� ��ġ�� ����, �浵 �̷��� �ΰ����� �ʿ��ϴ�!!
				double latitude = Double.parseDouble(nextToken()); // ������ ��� ~
				double longitude = Double.parseDouble(nextToken()); // �浵�� ��� ~
				boolean isRemain;
				if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
				else isRemain = true;

				return new ParsingResult(MESSAGE_PARSER_DOOR_LOCATION, isRemain, null, new DoorLocation(latitude, longitude));
			}
			else { // ���� ������ �����ϴ�.
				return new ParsingResult(MESSAGE_PARSER_IMPERFECT, false, null, null);
			}
		/***************************************************************************/
		case CMD_PROXIMITY_ALERT:
			// ���� ������� ���� ����ڰ� ������� �����ߴ��� �ƴ����� �˰� �;��Ѵٴ� �� !!
			boolean isRemain;
			if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
			else isRemain = true;
			return new ParsingResult(MESSAGE_PARSER_PROXIMITY_ALERT, isRemain, null, null);
		/***************************************************************************/
		case CMD_SUSPICIOUS_APPROACH:
			// ���� ������� �������� ���� �����ϴٰ� �˸���
			if(countTokens() == 0) isRemain = false; // ���� ó�� �ߴ��� �Ǵ��ϰ� ~
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
