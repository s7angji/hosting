package com.example.test_android_aoa;

import java.util.ArrayList;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.provider.MediaStore;
import android.provider.MediaStore.Images.Media;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class view_Photo extends Activity {
	String TAG = "PHOTO"; // log용도
	private Context mContext = this;
	
	// ///// MediaGallery ///////
	public final static int DISPLAYWIDTH = 200;
	public final static int DISPLAYHEIGHT = 200;
	TextView titleTextView;
	ImageButton imageButton;
	Button MediaDel;
	Cursor cursor;
	Bitmap bmp;
	String imageFilePath;
	int fileColumn;
	int titleColumn;
	int displayColumn;
	
    private ArrayList<ImageList> mListData = new ArrayList<ImageList>();
	
	ListView list;
	private ListView mListView = null;
	private ListViewAdapter mAdapter = null;		
	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.photo);
		
		mListView = (ListView) findViewById(R.id.mList);
		MediaDel = (Button) this.findViewById(R.id.list_delete);                                                  // 두께
		mListData = getList();
		mAdapter = new ListViewAdapter(this);
		mListView.setAdapter(mAdapter);
		mListView.setOnItemClickListener(new OnItemClickListener(){

			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				Intent intent = new Intent(view_Photo.this, DetailActivity.class);
				
				intent.putExtra("putImage", (Bitmap)mListData.get(position).mIcon);
				
				startActivity(intent);
			}
			
		});
		
		MediaDel.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {

				// media clean
				getContentResolver().delete(Media.EXTERNAL_CONTENT_URI,
						Media.DESCRIPTION + "='SmartDoorLock'", null);

				Toast.makeText(view_Photo.this, "memory clean",
						Toast.LENGTH_LONG).show();

			}
		});
	}
	static class ImageList{
		Bitmap mIcon;
		String mTitle;
	}
	// holder class //
	private class ViewHolder { // ViewHolder 클래스 생성.
		public ImageView mIcon;
		public TextView mText;
	}
	
	// adapter // 
	private class ListViewAdapter extends BaseAdapter {
	    private Context mContext = null;

	 
	    public ListViewAdapter(Context mContext) {
	        super();
	        this.mContext = mContext;
	    }
	 
	    @Override
	    public int getCount() {
	        return mListData.size();
	    }
	 
	    @Override
	    public Object getItem(int position) {
	        return mListData.get(position);
	    }
	 
	    @Override
	    public long getItemId(int position) {
	        return position;
	    }
	 
	    @Override
	    public View getView(int position, View convertView, ViewGroup parent) {
	        ViewHolder holder;
	        if (convertView == null) {
	            holder = new ViewHolder();
	     
	            LayoutInflater inflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	            convertView = inflater.inflate(R.layout.listview_item, null);
	     
	            holder.mIcon = (ImageView) convertView.findViewById(R.id.mImage);
	            holder.mText = (TextView) convertView.findViewById(R.id.mText);
	     
	            convertView.setTag(holder);
	        }else{
	            holder = (ViewHolder) convertView.getTag();
	        }
	     
	        ImageList mData = mListData.get(position);
	     
	        if (mData.mIcon != null) {
	            holder.mIcon.setVisibility(View.VISIBLE);
	            holder.mIcon.setImageBitmap(mData.mIcon);
	        }else{
	            holder.mIcon.setVisibility(View.GONE);
	        }
	     
	        holder.mText.setText(mData.mTitle);
	     
	        return convertView;
	    }  
	    
	    
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
	/* 이미지 가져오기  */
	public ArrayList<ImageList> getList(){
		ArrayList<ImageList> arr_ListItems = new ArrayList<ImageList>();
		String[] columns = { Media.DATA, Media.DESCRIPTION, Media.TITLE,
				Media.DISPLAY_NAME, Media.DATE_ADDED };
		cursor = managedQuery(Media.EXTERNAL_CONTENT_URI, columns,
				Media.DESCRIPTION + "='SmartDoorLock'", null, Media.DATE_ADDED
						+ " DESC");
		
		if(cursor.moveToFirst()){
			fileColumn = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
//			titleColumn = cursor
//					.getColumnIndexOrThrow(MediaStore.Images.Media.TITLE);
			displayColumn = cursor
					.getColumnIndexOrThrow(MediaStore.Images.Media.DISPLAY_NAME);
			
			do{
				ImageList item = new ImageList();
				item.mTitle = cursor.getString(displayColumn);
				imageFilePath = cursor.getString(fileColumn);
				item.mIcon = getBitmap(imageFilePath);
				
				arr_ListItems.add(item);
			} while(cursor.moveToNext());
			
		}
		cursor.close();
		return arr_ListItems;
	}

	/* 이미지 비트맵 변환 */
	private Bitmap getBitmap(String imageFilePath) {
		// 이미지가 아니라 이미지의 치수를 로드한다.
		BitmapFactory.Options bmpFactoryOptions = new BitmapFactory.Options();
		bmpFactoryOptions.inJustDecodeBounds = true;
		Bitmap bmp = BitmapFactory.decodeFile(imageFilePath, bmpFactoryOptions);

		int heightRatio = (int) Math.ceil(bmpFactoryOptions.outHeight
				/ (float) DISPLAYHEIGHT);
		int widthRatio = (int) Math.ceil(bmpFactoryOptions.outWidth
				/ (float) DISPLAYWIDTH);

		Log.i("HEIGHTRATIO", "" + heightRatio);
		Log.i("WIDTHRATIO", "" + widthRatio);

		if (heightRatio > 1 && widthRatio > 1) {
			if (heightRatio > widthRatio) {
				// 높이 비율이 더 커서 그에 따라 맞춘다.
				bmpFactoryOptions.inSampleSize = heightRatio;
			} else {
				// 너비 비율이 더커서 그에따라 맞춘다.
				bmpFactoryOptions.inSampleSize = widthRatio;
			}
		}
		// 실제로 디코딩 한다.
		bmpFactoryOptions.inJustDecodeBounds = false;
		bmp = BitmapFactory.decodeFile(imageFilePath, bmpFactoryOptions);

		return bmp;
	}	
}

