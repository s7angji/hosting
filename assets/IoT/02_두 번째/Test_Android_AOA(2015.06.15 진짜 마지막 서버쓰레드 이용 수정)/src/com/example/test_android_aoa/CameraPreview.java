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
		// SurfaceHolder �� ������ �ִ� ���� Surface�� �ı��ǰų� ������Ʈ �ɰ�� ���� �ݹ��� �����Ѵ�
		mHolder = getHolder();
		mHolder.addCallback(this);
		// deprecated �Ǿ����� 3.0 ���� �������� �ʼ� �޼ҵ�� ȣ���ص�.
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

	}

	public void surfaceCreated(SurfaceHolder holder) {
		// Surface�� �����Ǿ����� �����並 ��� ����� �������ش�. (holder �� ���� SurfaceHolder�� �ѷ��ش�.
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
		// ������ ���Ž� ī�޶� ��뵵 �����ٰ� �����Ͽ� ���ҽ��� ���� ��ȯ�Ѵ�
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
		// �����並 ȸ����Ű�ų� ����� ó���� ���⼭ ���ش�.
		// ������ ����ÿ��� ���� �����並 ������� �����ؾ��Ѵ�.

		if (mHolder.getSurface() == null) {
			// �����䰡 �������� ������
			return;
		}

		// �켱 �����
		try {
			mCamera.stopPreview();
		} catch (Exception e) {
			// �����䰡 �������� ���� �ʴ� ����
		}

		Camera.Parameters parameters = mCamera.getParameters();
		Camera.Size size = getBestPreviewSize(w, h);
		parameters.setPreviewSize(size.width, size.height);
		mCamera.setParameters(parameters);
		// ������ ����, ó�� ���� ���⼭ ���ش�.

		// ���� ����� �������� �����並 ������Ѵ�
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
