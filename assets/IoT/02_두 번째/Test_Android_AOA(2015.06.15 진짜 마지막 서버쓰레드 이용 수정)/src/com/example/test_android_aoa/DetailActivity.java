package com.example.test_android_aoa;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;

public class DetailActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.detail);
		ImageView view_image = (ImageView) findViewById(R.id.detail_image);

		Intent intent = new Intent(this.getIntent());

		Bitmap origiBitmap = (Bitmap) intent.getExtras().get("putImage");

		view_image.setImageBitmap(imgRotate(origiBitmap));
	}

	private Bitmap imgRotate(Bitmap bmp) {
		int width = bmp.getWidth();
		int height = bmp.getHeight();

		Matrix matrix = new Matrix();
		matrix.postRotate(90);

		Bitmap resizedBitmap = Bitmap.createBitmap(bmp, 0, 0, width, height,
				matrix, true);
		bmp.recycle();

		return resizedBitmap;
	}
}
