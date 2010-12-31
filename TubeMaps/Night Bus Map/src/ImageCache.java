import javax.microedition.lcdui.*;
import java.util.*;

// The image cache. This records which images are available and loads then etc.

public class ImageCache implements Runnable
{
	// This interface is used to update the ImageCanvas when a new image is loaded.
	public interface ImageLoadedNotification
	{
		public void imageLoaded(int imNum);
	}
	
	/// Public interface.
	
	// Create an image cache, the cache starts straight away.
	// Initially all images are set as undesired. numImages sets the number of images.
	// Images are loaded from baseName + imNum + ".png".
	// if less than maxCachedImages are desired, the undesired but loaded ones will be kept around.
	// If more than maxCachedImages are desired then they will all be loaded as expected.
	public ImageCache(int numImages, String baseName, int maxCachedImages)
	{
		if (numImages < 0)
			numImages = 0;
		
		this.baseName = baseName;
		
		images = new Image[numImages];
		desired = new boolean[numImages]; // I assume these will all start as false.
//		loadOrder = new int[numImages];
		
		loadQueue = new Vector();
		
		loaderThread = new Thread(this);
		loaderThread.start();
	}
	
	// Use this blocking function to load images before the first paint.
	// Make sure to use this before any setDesired call.
	synchronized public void preloadImage(int imNum)
	{
		if (imNum < 0 || imNum >= images.length)
			return;
		
		images[imNum] = loadImage(baseName + imNum + ".png");
		desired[imNum] = true;		
	}
	
	// Set an image to be desired. Initially none are.
	synchronized public void setDesired(int imNum, boolean des)
	{
		if (imNum < 0 || imNum >= desired.length)
			return;
		
		desired[imNum] = des;
		
		// Now see if we must add or remove it from the load Queue.
		int index = loadQueue.indexOf(new Integer(imNum));
		if (des && images[imNum] == null && index == -1)
		{
			loadQueue.addElement(new Integer(imNum));
			notify();
		}
		else if (!des && index != -1)
		{
			loadQueue.removeElementAt(index);
		}
		
		if (!des)
			images[imNum] = null;
	}
	
	// Returns null if imNum is invalid or if the image isn't loaded yet.
	synchronized public Image getImage(int imNum)
	{
		if (imNum < 0 || imNum >= images.length)
			return null;
		
		return images[imNum];
	}
	
	// When a desired image is loaded, n will be notified.
	synchronized public void setNotify(ImageLoadedNotification n)
	{
		notify = n;
	}
	
	/// Private parts.
	public void run()
	{
		try
		{
			while (true)
			{
				int imNum;
				synchronized (this)
				{
					while (loadQueue.isEmpty())
						wait();
					
					imNum = ((Integer)loadQueue.elementAt(0)).intValue();
				}
				
				// The slow bit - don't synchronise.
				Image im = loadImage(baseName + imNum + ".png");
				
				synchronized (this)
				{
					images[imNum] = im;
					loadQueue.removeElementAt(0);
					if (notify != null)
						notify.imageLoaded(imNum);
					}
				
				loaderThread.yield();
			}
		}
		catch (InterruptedException e)
		{
		}
	}
	
	// Load an image, returns null on failure.
	private Image loadImage(String filename)
	{
		Image im = null;
		
		try
		{
			im = Image.createImage(filename);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			im = Image.createImage(100, 100);
			
		}
				
		return im;
	}
	
	// The images - null for no image.
	private Image[] images;
	// Whether each image is desired or not.
	private boolean[] desired;
	// The order each image was loaded in - used so we can free the oldest cached image.
//	private int[] loadOrder;
	// The loadOrder of the last loaded image.
//	private int lastLoadOrder;
	// The base name for the images. <imNum>.png is appended.
	private String baseName;
	// The maximum number of cached images to store (unless more are explicitly requested).
//	private int maxCachedImages;
	
	// The queue of images to load. FIFO.
	private Vector loadQueue;
	
	// Who to notify when imaes are loaded.
	private ImageLoadedNotification notify;
	
	private Thread loaderThread;
}