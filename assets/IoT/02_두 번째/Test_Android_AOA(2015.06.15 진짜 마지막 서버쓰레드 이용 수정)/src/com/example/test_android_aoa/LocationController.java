package com.example.test_android_aoa;

import java.util.List;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;

public class LocationController implements LocationListener {
	Location mLocation = null; // 고작 이 위치를 얻어내기 위해 밑에 코드들이 존재하는 것이다 !!
	public boolean hasLocation() { return (mLocation != null) ? true : false; }

	private LocationManager mLocationManager;
	private List<String> mEnabledProviders;

	MainActivity hostActivity;
	Handler hostHandler;

	public void sendHandlerMessage(String msg, int what) {
		Message m = new Message();
		m.what = what;
		m.obj = msg;
		hostHandler.sendMessage(m);
	}

	// 생성자
	public LocationController(MainActivity hostActivity) { // TODO : MainActivity는 바뀔 수 있다 !
		this.hostActivity = hostActivity;
		this.hostHandler = hostActivity.mHandler;

		mLocationManager = (LocationManager) hostActivity.getSystemService(MainActivity.LOCATION_SERVICE);

		requestLocation(true);
	}

	public boolean requestLocation(boolean showAlertDialog) {
		Criteria criteria = new Criteria();
		criteria.setAccuracy(Criteria.ACCURACY_COARSE);
		mEnabledProviders = mLocationManager.getProviders(criteria, true);
		if (mEnabledProviders.isEmpty()) {
			if(showAlertDialog) { showGpsSettingsAlert(); return true; }
			else { return false; }
		}
		else {
			for(String enabledProvider : mEnabledProviders) {
				mLocationManager.requestSingleUpdate(enabledProvider,
						this,
						null);
			}
			return true;
		}
	}

	public void release() {
		if(mLocationManager != null) {
			mLocationManager.removeUpdates(this);
			mLocationManager = null;
		}
	}

	public void showGpsSettingsAlert(){
		AlertDialog.Builder alertDialog = new AlertDialog.Builder(hostActivity);

		alertDialog.setTitle("GPS 정보가 필요합니다 !!")
		.setMessage("GPS 셋팅이 되지 않았습니다.\n 설정창으로 가시겠습니까?")
		.setNegativeButton("Cancel", 
				new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				dialog.cancel();
				sendHandlerMessage("GPS가 활성화 되지 않았습니다.", MainActivity.FINISH_REQUEST_FROM_LOCATION);
			}
		})
		.setPositiveButton("Settings", 
				new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int which) {
				Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
				hostActivity.startActivityForResult(intent, MainActivity.REQUEST_GPS_AT_ON_ACTIVITY_RESULT);
			}
		});

		alertDialog.show();
	}

	// 이 밑으로는 전부 LocationListener 콜백 인터페이스 추상 메소드 구현 !!
	@Override
	public void onLocationChanged(Location location) {

		mLocation = location; // 진정 내가 원하던걸 얻었군 ^^*

		// 도어락은 고정이므로 단 한번만 이용한다.
		release();

		sendHandlerMessage("필요한 위치 정보를 얻었습니다.\n더 이상의 위치 정보는 수집하지 않습니다.", MainActivity.GET_LOCATION_FROM_LOCATION);
	}

	@Override
	public void onProviderDisabled(String provider) {
		if(mLocation == null)
			sendHandlerMessage("필요한 위치 정보를 얻지 못했습니다. 다시 시도해주세요.", MainActivity.FINISH_REQUEST_FROM_LOCATION);
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
