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
	Location mLocation = null; // ���� �� ��ġ�� ���� ���� �ؿ� �ڵ���� �����ϴ� ���̴� !!
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

	// ������
	public LocationController(MainActivity hostActivity) { // TODO : MainActivity�� �ٲ� �� �ִ� !
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

		alertDialog.setTitle("GPS ������ �ʿ��մϴ� !!")
		.setMessage("GPS ������ ���� �ʾҽ��ϴ�.\n ����â���� ���ðڽ��ϱ�?")
		.setNegativeButton("Cancel", 
				new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				dialog.cancel();
				sendHandlerMessage("GPS�� Ȱ��ȭ ���� �ʾҽ��ϴ�.", MainActivity.FINISH_REQUEST_FROM_LOCATION);
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

	// �� �����δ� ���� LocationListener �ݹ� �������̽� �߻� �޼ҵ� ���� !!
	@Override
	public void onLocationChanged(Location location) {

		mLocation = location; // ���� ���� ���ϴ��� ����� ^^*

		// ������� �����̹Ƿ� �� �ѹ��� �̿��Ѵ�.
		release();

		sendHandlerMessage("�ʿ��� ��ġ ������ ������ϴ�.\n�� �̻��� ��ġ ������ �������� �ʽ��ϴ�.", MainActivity.GET_LOCATION_FROM_LOCATION);
	}

	@Override
	public void onProviderDisabled(String provider) {
		if(mLocation == null)
			sendHandlerMessage("�ʿ��� ��ġ ������ ���� ���߽��ϴ�. �ٽ� �õ����ּ���.", MainActivity.FINISH_REQUEST_FROM_LOCATION);
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
