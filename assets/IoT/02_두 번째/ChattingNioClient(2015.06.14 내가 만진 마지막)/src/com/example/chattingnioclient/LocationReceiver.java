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

	// 생성자
	public LocationReceiver(ChattingNioClientActivity hostActivity, double latitude, double longitude, int radius) { // TODO : ChattingNioClientActivity 달라질 수 있음
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

	// 근접경보를 받았을 때, 대처함 ~_~ . . .
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
		// TODO : 드디어 여기다가 이제 코딩하면 된다 ~_~!! 힘들어 죽는 줄
		isEntering = true;
		displayNotification(hostActivity, "도어락에 접근 중");
	}

	public void onExitingProximity(Context context) {
		// TODO : 드디어 여기다가 이제 코딩하면 된다 ~_~!! 힘들어 죽는 줄
		isEntering = false;
		displayNotification(hostActivity, "도어락에서 벗어나는 중");
	}

	private void displayNotification(Context context, String message) {
		NotificationManager notificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);

		PendingIntent pi = PendingIntent.getActivity(context, 0, new Intent(), 0);

		Notification notification = new Notification(R.drawable.ic_launcher, message, System.currentTimeMillis());
		notification.setLatestEventInfo(context, "도어락 근접 경보", message, pi);

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
		//Toast.makeText(hostActivity, "위도 : " + location.getLatitude() + " 경도 : " + location.getLongitude() + "  " + String.valueOf(isEntering), Toast.LENGTH_SHORT).show();
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
		// 근접 경보를 알릴 필요는 없다 !!
		// 하지만, 어느정도 근접했으면 GPS 프로바이더로 더 정확한 위치 정보를 알아내야 한다 ^^
		// 또는, 목표한 위치보다 멀다면 세밀한 위치 변경에 신경을 덜 쓰게하자 ~!
		else {
			// 목표위치 반경과 거리를 따진다. distanceFromRadius 이 값이 작을 수록 더 정밀한 위치를 필요하게 된다.
			float distanceFromRadius = Math.abs(distance - radius);

			// 오차 범위를 고려한 거리
			float locationEvaluationDistance =
					(distanceFromRadius - location.getAccuracy()) / 2;
			// requestLocationUpdates의 minDistance의 값을 정하자.
			// 최소는 1m 변경에 귀를 기울이고, 내가 목표한 위치보다 멀리 있다면 배터리 소모를 줄이기 위해 덜 귀를 기울이자 ^^
			float updateDistance = Math.max(1, locationEvaluationDistance);

			// 실내에서만 테스트하므로 NETWORK_PROVIDER만 이용함. 그래도 사실 GPS_PROVIDER쓰는게 맞음 ~~!!!
			String provider;
			if(distanceFromRadius <= location.getAccuracy()
					|| LocationManager.NETWORK_PROVIDER.equals(location.getProvider())) {
				provider = LocationManager.NETWORK_PROVIDER;
				//Toast.makeText(hostActivity, "GPS_PROVIDER를 이용하겠다.", Toast.LENGTH_LONG).show();
			}
			else {
				provider = LocationManager.NETWORK_PROVIDER;
				//Toast.makeText(hostActivity, "NETWORK_PROVIDER를 이용하겠다.", Toast.LENGTH_SHORT).show();
			}
			mLocationManager.removeUpdates(this);
			mLocationManager.requestLocationUpdates(provider, 3500, updateDistance, this);
		}
	}

	@Override
	public void onProviderDisabled(String provider) {
		isEntering = false;
		// Toast.makeText(hostActivity, "GPS 정보를 얻을 수 없습니다", Toast.LENGTH_SHORT).show();
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