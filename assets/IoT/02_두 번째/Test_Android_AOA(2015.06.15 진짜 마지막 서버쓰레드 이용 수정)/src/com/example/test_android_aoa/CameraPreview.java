package com.example.test_android_aoa;

import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.content.ContentValues;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.net.Uri;
import android.provider.MediaStore.Images.Media;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

public class CameraPreview extends SurfaceView implements
		SurfaceHolder.Callback {
	String TAG = "CAMERA";
	private SurfaceHolder mHolder;
	private Camera mCamera;
	private PreviewCallback mPreviewCallback;
	public boolean check = false;

	public CameraPreview(Context context, Camera camera) {
		super(context);
		mCamera = camera;
		// SurfaceHolder 가 가지고 있는 하위 Surface가 파괴되거나 업데이트 될경우 받을 콜백을 세팅한다
		mHolder = getHolder();
		mHolder.addCallback(this);
		// deprecated 되었지만 3.0 이하 버젼에서 필수 메소드라서 호출해둠.
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

	}

	public void surfaceCreated(SurfaceHolder holder) {
		// Surface가 생성되었으니 프리뷰를 어디에 띄울지 지정해준다. (holder 로 받은 SurfaceHolder에 뿌려준다.
		try {
			Camera.Parameters parameters = mCamera.getParameters();
			if (getResources().getConfiguration().orientation != Configuration.ORIENTATION_LANDSCAPE) {
				parameters.set("orientation", "portrait");
				mCamera.setDisplayOrientation(270);
				parameters.setRotation(270);
			} else {
				parameters.set("orientation", "landscape");
				mCamera.setDisplayOrientation(0);
				parameters.setRotation(0);
			}
			mCamera.setParameters(parameters);

			mCamera.setPreviewDisplay(holder);
			mCamera.startPreview();
		} catch (IOException e) {
			Log.d(TAG, "Error setting camera preview: " + e.getMessage());
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		// 프리뷰 제거시 카메라 사용도 끝났다고 간주하여 리소스를 전부 반환한다
		if (mCamera != null) {
			mCamera.stopPreview();
			mCamera.setPreviewCallback(null);
			mCamera.release();
			mCamera = null;
		}
	}

	private Camera.Size getBestPreviewSize(int width, int height) {
		Camera.Size result = null;
		Camera.Parameters p = mCamera.getParameters();
		for (Camera.Size size : p.getSupportedPreviewSizes()) {
			if (size.width <= width && size.height <= height) {
				if (result == null) {
					result = size;
				} else {
					int resultArea = result.width * result.height;
					int newArea = size.width * size.height;

					if (newArea > resultArea) {
						result = size;
					}
				}
			}
		}
		return result;

	}

	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		// 프리뷰를 회전시키거나 변경시 처리를 여기서 해준다.
		// 프리뷰 변경시에는 먼저 프리뷰를 멈춘다음 변경해야한다.

		if (mHolder.getSurface() == null) {
			// 프리뷰가 존재하지 않을때
			return;
		}

		// 우선 멈춘다
		try {
			mCamera.stopPreview();
		} catch (Exception e) {
			// 프리뷰가 존재조차 하지 않는 경우다
		}

		Camera.Parameters parameters = mCamera.getParameters();
		Camera.Size size = getBestPreviewSize(w, h);
		parameters.setPreviewSize(size.width, size.height);
		mCamera.setParameters(parameters);
		// 프리뷰 변경, 처리 등을 여기서 해준다.

		// 새로 변경된 설정으로 프리뷰를 재생성한다
		try {
			mCamera.setPreviewDisplay(mHolder);
			mCamera.startPreview();

		} catch (Exception e) {
			Log.d(TAG, "Error starting camera preview: " + e.getMessage());
		}
		
		// preview capture //
		mCamera.setPreviewCallback(new Camera.PreviewCallback() {
			
			public void onPreviewFrame(byte[] data, Camera camera) {
				if (check == true) {
					Camera.Parameters parameters = camera.getParameters();
					
					int format = parameters.getPreviewFormat();

					// YUV formats require more conversion
					if (format == ImageFormat.NV21
							|| format == ImageFormat.YUY2
							|| format == ImageFormat.NV16) {
						int w = parameters.getPreviewSize().width;
						int h = parameters.getPreviewSize().height;

						// Get the YuV image
						YuvImage yuv_image = new YuvImage(data, format, w, h,
								null);
						// Convert YuV to Jpeg
						Rect rect = new Rect(0, 0, w, h);
						ByteArrayOutputStream output_stream = new ByteArrayOutputStream();

						yuv_image.compressToJpeg(rect, 100, output_stream);
						byte[] byt = output_stream.toByteArray();

						OutputStream outStream = MainActivity.dir.fileUri;

						try {
							outStream.write(byt);
							outStream.flush();
							outStream.close();

						} catch (FileNotFoundException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						} finally {

						}
					}
				}
				check = false;
			}
		});

	}

}
