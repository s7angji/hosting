package com.example.chattingnioclient;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.widget.Toast;

public class LocationReceiver extends BroadcastReceiver implements LocationListener {
	public boolean isEntering = false;

	ChattingNioClientActivity hostActivity;

	private static final int NOTIFICATION_ID = 9999;
	public static final String PROXIMITY_ACTION = "com.example.chattingnioclient";
	LocationManager mLocationManager;

	private double latitude;
	private double longitude;
	private int radius;

	// ������
	public LocationReceiver(ChattingNioClientActivity hostActivity, double latitude, double longitude, int radius) { // TODO : ChattingNioClientActivity �޶��� �� ����
		this.hostActivity = hostActivity;

		this.latitude = latitude;
		this.longitude = longitude;
		this.radius = radius;

		mLocationManager = (LocationManager)hostActivity.getSystemService(Context.LOCATION_SERVICE);
		mLocationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 
				3500,
				0, 
				this);

		IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(PROXIMITY_ACTION);
		hostActivity.registerReceiver(this, intentFilter);
	}

	// �����溸�� �޾��� ��, ��ó�� ~_~ . . .
	@Override
	public void onReceive(Context context, Intent intent) {
		if(intent.hasExtra(LocationManager.KEY_PROXIMITY_ENTERING)) {

			if (intent.getBooleanExtra(LocationManager.KEY_PROXIMITY_ENTERING, false))
				onEnteringProximity(context);
			else
				onExitingProximity(context);
		}
	}

	public void onEnteringProximity(Context context) {
		// TODO : ���� ����ٰ� ���� �ڵ��ϸ� �ȴ� ~_~!! ����� �״� ��
		isEntering = true;
		displayNotification(hostActivity, "������� ���� ��");
	}

	public void onExitingProximity(Context context) {
		// TODO : ���� ����ٰ� ���� �ڵ��ϸ� �ȴ� ~_~!! ����� �״� ��
		isEntering = false;
		displayNotification(hostActivity, "��������� ����� ��");
	}

	private void displayNotification(Context context, String message) {
		NotificationManager notificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);

		PendingIntent pi = PendingIntent.getActivity(context, 0, new Intent(), 0);

		Notification notification = new Notification(R.drawable.ic_launcher, message, System.currentTimeMillis());
		notification.setLatestEventInfo(context, "����� ���� �溸", message, pi);

		notification.defaults |= Notification.DEFAULT_SOUND;
		notification.flags |= Notification.FLAG_AUTO_CANCEL;

		notificationManager.notify(NOTIFICATION_ID, notification);
	}

	public void release() {
		mLocationManager.removeUpdates(this);
		hostActivity.unregisterReceiver(this);
	}

	private float getDistance(Location location) {
		float[] results = new float[1];

		Location.distanceBetween(latitude,
				longitude,
				location.getLatitude(),
				location.getLongitude(),
				results);

		return results[0];
	}

	@Override
	public void onLocationChanged(Location location) {
		//Toast.makeText(hostActivity, "���� : " + location.getLatitude() + " �浵 : " + location.getLongitude() + "  " + String.valueOf(isEntering), Toast.LENGTH_SHORT).show();
		float distance = getDistance(location);
		if((distance <= radius) && !isEntering) {
			isEntering = true;

			Intent intent =
					new Intent(PROXIMITY_ACTION);
			intent.putExtra(LocationManager.KEY_PROXIMITY_ENTERING, true);
			hostActivity.sendBroadcast(intent);
		}
		else if((distance > radius) && isEntering) {
			isEntering = false;

			Intent intent =
					new Intent(PROXIMITY_ACTION);
			intent.putExtra(LocationManager.KEY_PROXIMITY_ENTERING, false);
			hostActivity.sendBroadcast(intent);
		}
		// ���� �溸�� �˸� �ʿ�� ���� !!
		// ������, ������� ���������� GPS ���ι��̴��� �� ��Ȯ�� ��ġ ������ �˾Ƴ��� �Ѵ� ^^
		// �Ǵ�, ��ǥ�� ��ġ���� �ִٸ� ������ ��ġ ���濡 �Ű��� �� �������� ~!
		else {
			// ��ǥ��ġ �ݰ�� �Ÿ��� ������. distanceFromRadius �� ���� ���� ���� �� ������ ��ġ�� �ʿ��ϰ� �ȴ�.
			float distanceFromRadius = Math.abs(distance - radius);

			// ���� ������ ����� �Ÿ�
			float locationEvaluationDistance =
					(distanceFromRadius - location.getAccuracy()) / 2;
			// requestLocationUpdates�� minDistance�� ���� ������.
			// �ּҴ� 1m ���濡 �͸� ����̰�, ���� ��ǥ�� ��ġ���� �ָ� �ִٸ� ���͸� �Ҹ� ���̱� ���� �� �͸� ������� ^^
			float updateDistance = Math.max(1, locationEvaluationDistance);

			// �ǳ������� �׽�Ʈ�ϹǷ� NETWORK_PROVIDER�� �̿���. �׷��� ��� GPS_PROVIDER���°� ���� ~~!!!
			String provider;
			if(distanceFromRadius <= location.getAccuracy()
					|| LocationManager.NETWORK_PROVIDER.equals(location.getProvider())) {
				provider = LocationManager.NETWORK_PROVIDER;
				//Toast.makeText(hostActivity, "GPS_PROVIDER�� �̿��ϰڴ�.", Toast.LENGTH_LONG).show();
			}
			else {
				provider = LocationManager.NETWORK_PROVIDER;
				//Toast.makeText(hostActivity, "NETWORK_PROVIDER�� �̿��ϰڴ�.", Toast.LENGTH_SHORT).show();
			}
			mLocationManager.removeUpdates(this);
			mLocationManager.requestLocationUpdates(provider, 3500, updateDistance, this);
		}
	}

	@Override
	public void onProviderDisabled(String provider) {
		isEntering = false;
		// Toast.makeText(hostActivity, "GPS ������ ���� �� �����ϴ�", Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		// no-op
	}

	@Override
	public void onProviderEnabled(String provider) {
		// no-op
	}
}