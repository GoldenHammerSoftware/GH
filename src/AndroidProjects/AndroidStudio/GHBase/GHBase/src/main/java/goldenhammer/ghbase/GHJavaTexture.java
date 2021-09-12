package goldenhammer.ghbase;

import java.io.IOException;

import android.content.res.AssetManager;
import android.util.Log;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import java.io.InputStream;

import android.opengl.GLUtils;
import javax.microedition.khronos.opengles.GL10;

public class GHJavaTexture 
{	
	Bitmap mBitmap;
	public int mWidth, mHeight, mDepth;
	
	public GHJavaTexture(Bitmap bitmap) {
		mBitmap = bitmap;
    	mWidth = bitmap.getWidth();
    	mHeight = bitmap.getHeight();
    	int rowBytes = bitmap.getRowBytes();
    	mDepth = rowBytes/mWidth;
	} 

	public static GHJavaTexture loadTextureFromFile(String filename, AssetManager assetMgr)
	{
		Bitmap bitmap = GHJavaTexture.loadBitmapFromFile(filename, assetMgr);
		if (bitmap == null) return null;
		return new GHJavaTexture(bitmap);
	}
	
    protected static Bitmap loadBitmapFromFile(String filename, AssetManager assetMgr)
    {
    	Bitmap bitmap = null;
    	try {
    		InputStream filestream = assetMgr.open(filename);
    		if (filestream == null) {
        		String failString = "Couldn't find java bitmap " + filename;
        		Log.e("bmjava", failString);
        		return null;
        	}
    	    // load non-alpha textures as 16 bit
    	    BitmapFactory.Options bmpOptions = new BitmapFactory.Options();
    	    bmpOptions.inPreferredConfig = Bitmap.Config.RGB_565;
    	    bitmap = BitmapFactory.decodeStream(filestream, null, bmpOptions);
    	     
    	    filestream.close();
    	}
    	catch(IOException e) {
    		Log.e("tag", e.getMessage());
    	}
    	return bitmap;
    }
    
    protected Bitmap shrinkBitmap(Bitmap bitmap)
    {
    	int width = bitmap.getWidth();
    	int height = bitmap.getHeight();
		width /= 4;
		height /= 4;
		Bitmap bitmap2 = Bitmap.createScaledBitmap(bitmap, width, height, true);
		bitmap.recycle();  
		return bitmap2;  
    }
    
    public void giveToOGL(int wantMips)
    {
    	if (mBitmap == null) return;
    	
    	// test hack
    	//bitmap = shrinkBitmap(bitmap);
    	
		if (wantMips == 0) {
			GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, mBitmap, 0);
			return;
		}

		Bitmap bitmap = mBitmap;
    	int width = bitmap.getWidth();
    	int height = bitmap.getHeight();
    	// build mipmaps.
    	int level = 0;  
    	while (height >= 1 || width >= 1) 
    	{  
        	Bitmap.Config bmpCfg = bitmap.getConfig();
        	int bmpColorCode = GL10.GL_RGBA;
        	if (bmpCfg == Bitmap.Config.RGB_565) {
        		bmpColorCode = GL10.GL_RGB;
        	}

    		GLUtils.texImage2D(GL10.GL_TEXTURE_2D, level, bmpColorCode, bitmap, 0);
    		
    		if (height == 1 || width == 1) break;
    		level++;  
    		width /= 2;
    		height /= 2;
    		Bitmap bitmap2 = Bitmap.createScaledBitmap(bitmap, width, height, true);
    		bitmap.recycle();  
    		bitmap = bitmap2;  
    	}
    }
}
