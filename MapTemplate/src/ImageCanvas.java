

import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.Sprite;
import java.util.*;
import java.io.*;

// This class displays a selected image centered in the screen
class ImageCanvas extends Canvas implements ImageCache.ImageCacheListener
{
	// The map definition.
	private MapDefinition def;
	// The image cache.
	private ImageCache cache;
	
	// The position of the screen centre in pixels.
	private int currentXPos;
	private int currentYPos;
	
	// The desired position of the screen centre in pixels.
	private int desiredXPos;
	private int desiredYPos;
	
	// The zoom level. 0 = completely zoomed in, 1 = zoomed out a bit, 2 = zoomed out more.
	private int zoom;

	// The timer to update the scrolling.
	private Timer timer = new Timer();
	// The timer task. Moves currentX,YPos towards desiredX,YPos.
    private Scroller scroller = null;

    class Scroller extends TimerTask
	{
		private final static int ANIM_FRAMES = 15;
		private final double[] ANIM_FACTORS = { 14, 13, 12, 11, 10,
												9, 8, 7, 6, 5,
												5, 4, 3, 2, 1};
		
		private int framesLeft = ANIM_FRAMES;

		public void run()
		{
			int diffX = desiredXPos - getXPos();
			int diffY = desiredYPos - getYPos();
			diffX *= ANIM_FACTORS[framesLeft-1] / framesLeft;
			diffY *= ANIM_FACTORS[framesLeft-1] / framesLeft;
			setXPos(getXPos() + diffX);
			setYPos(getYPos() + diffY);
			
			
			if (--framesLeft == 0)
			{
				cancel();
				scroller = null;
			}
			repaint();
		}
	}
	
	ImageCanvas()
	{
		currentXPos = 0;
		currentYPos = 0;
		desiredXPos = 0;
		desiredYPos = 0;
		zoom = 0;
	}
	
	public void loadMap(MapDefinition d, Integer x, Integer y, Integer z)
	{
		cache = new ImageCache();
		cache.setListener(this);
		cache.LoadMap(d.mapWidth * d.mapHeight * d.overviewMapWidth * d.overviewMapHeight);
		def = d;

		if (def == null)
			return;
		
		if (x == null)
			setXPos(def.totalWidth / 2);
		else
			setXPos(x.intValue());
		
		if (y == null)
			setYPos(def.totalHeight / 2);
		else
			setYPos(y.intValue());
		
		if (z == null)
			zoom = 0;
		else
			zoom = z.intValue();
	}
	
	private int tileNumber(int x, int y, int zoom)
	{
		if (def == null)
			return -1;
		
		switch (zoom)
		{
			case 0:
				if (x < 0 || x >= def.mapWidth || y < 0 || y >= def.mapHeight)
					return -1;
				return y + def.mapHeight * x;
			case 1:
				if (x < 0 || x >= def.overviewMapWidth || y < 0 || y >= def.overviewMapHeight)
					return -1;
				return y + def.overviewMapHeight * x + (def.mapWidth * def.mapHeight);
		}
		return -1;
	}
	
	protected void paint(Graphics g)
	{
		int width = getWidth();
		int height = getHeight();
		
		g.setColor(0x000000); // Black.
		g.fillRect(0, 0, width, height);
		
		if (def == null)
			return;
		
		int margin = 20; // Reduce flicker slightly.
		
		int xp = currentXPos; // Avoid these being changed mid-draw!
		int yp = currentYPos;
		int mw = getCurrentMapWidth();
		int mh = getCurrentMapHeight();

		int l = xp - width/2;
		int r = xp + width/2;
		int t = yp - height/2;
		int b = yp + height/2;
		
		l -= margin; r += margin; t -= margin; b += margin;
	
		int xStart = l / def.tileWidth;
		int yStart = t / def.tileHeight;
		
		int xStop = r / def.tileWidth;
		int yStop = b / def.tileHeight;
		
		// Bounds checking.
		if (xStart < 0) xStart = 0;
		if (yStart < 0) yStart = 0;
		if (xStart >= mw) xStart = mw - 1;
		if (yStart >= mh) yStart = mh - 1;
		if (xStop < 0) xStop = 0;
		if (yStop < 0) yStop = 0;
		if (xStop >= mw) xStop = mw - 1;
		if (yStop >= mh) yStop = mh - 1;

		// Work out which tiles we need and load them (and release the others).
		Vector desired = new Vector();
		
		for (int x = xStart; x <= xStop; ++x)
		{
			for (int y = yStart; y <= yStop; ++y)
			{
				// We want this one.
				desired.addElement(new Integer(tileNumber(x, y, zoom)));

				// And draw it if we already have it.
				Image tile = cache.getImage(tileNumber(x, y, zoom));

				if (tile != null)
					g.drawRegion(tile, 0, 0, tile.getWidth(), tile.getHeight(), Sprite.TRANS_NONE,
								x * def.tileWidth - xp + width/2, y * def.tileHeight - yp + height/2, 0);
			}
		}
		cache.setDesired(desired);
	}
	
	// The trigger image has been loaded.
	public void imagesLoaded()
	{
		if (scroller == null)
			repaint();
	}
	
	private void addAnim(int dx, int dy)
	{
		if (scroller != null)
		{
			scroller.cancel();
			scroller = null;
		}
		else
		{
			desiredXPos = currentXPos;
			desiredYPos = currentYPos;
		}
		
		desiredXPos += dx;
		desiredYPos += dy;
		
		int width = getWidth();
		int height = getHeight();
		
		if (desiredXPos < width/2)
			desiredXPos = width/2;
		if (desiredXPos > getCurrentMapWidth() - width/2)
			desiredXPos = getCurrentMapWidth() - width/2;
		if (desiredYPos < height/2)
			desiredYPos = height/2;
		if (desiredYPos > getCurrentMapHeight() - height/2)
			desiredYPos = getCurrentMapHeight() - height/2;
		
		scroller = new Scroller();
		timer.schedule(scroller, 0, 1000/30);		
	}

	protected void keyPressed(int keyCode)
	{
		if (def == null)
			return;

		int amount = Math.min(getWidth(), getHeight()) / 2;

		switch (getGameAction(keyCode))
		{
			case RIGHT:
				addAnim(amount, 0);
				break;
			case LEFT:
				addAnim(-amount, 0);
				break;
			case DOWN:
				addAnim(0, amount);
				break;
			case UP:
				addAnim(0, -amount);
				break;
			default:
				switch (keyCode)
				{
					case KEY_NUM1:
						addAnim(-amount, -amount);
						break;
					case KEY_NUM2:
						addAnim(0, -amount);
						break;
					case KEY_NUM3:
						addAnim(amount, -amount);
						break;
					case KEY_NUM4:
						addAnim(-amount, 0);
						break;
					case KEY_NUM5:
						cancelScrolling();
						zoom = 1 - zoom;
						switch (zoom)
						{
							case 0:
								setXPos(getXPos() * def.totalWidth / def.overviewTotalWidth);
								setYPos(getYPos() * def.totalHeight / def.overviewTotalHeight);
								break;
							case 1:
								setXPos(getXPos() * def.overviewTotalWidth / def.totalWidth);
								setYPos(getYPos() * def.overviewTotalHeight / def.totalHeight);
								break;
						}
						repaint();
						break;
					case KEY_NUM6:
						addAnim(amount, 0);
						break;
					case KEY_NUM7:
						addAnim(-amount, amount);
						break;
					case KEY_NUM8:
						addAnim(0, amount);
						break;
					case KEY_NUM9:
						addAnim(amount, amount);
						break;
					case KEY_POUND:
						cancelScrolling();
						setXPos(getCurrentMapWidth() / 2);
						setYPos(getCurrentMapHeight() / 2);
						repaint();
						break;
				}
				break;
		}
	}
	
	protected void keyRepeated(int keyCode)
	{
		keyPressed(keyCode);
	}

	public void setXPos(int x)
	{
		currentXPos = x;
		int width = getWidth();
		if (currentXPos < width/2)
			currentXPos = width/2;
		if (currentXPos > getCurrentMapWidth() - width/2)
			currentXPos = getCurrentMapWidth() - width/2;
	}
	
	public void setYPos(int y)
	{
		currentYPos = y;
		int height = getHeight();
		if (currentYPos < height/2)
			currentYPos = height/2;
		if (currentYPos > getCurrentMapHeight() - height/2)
			currentYPos = getCurrentMapHeight() - height/2;
	}
	
	public void cancelScrolling()
	{
		if (scroller != null)
		{
			scroller.cancel();
			scroller = null;
		}
		
		desiredXPos = currentXPos;
		desiredYPos = currentYPos;
	}
	
	public int getXPos()
	{
		return currentXPos;
	}
	
	public int getYPos()
	{
		return currentYPos;
	}
	
	public int getCurrentMapWidth() // In pixels
	{
		switch (zoom)
		{
			case 0:
				return def.totalWidth;
			case 1:
				return def.overviewTotalWidth;
		}
		return 0;
	}
	
	public int getCurrentMapHeight() // In pixels
	{
		switch (zoom)
		{
			case 0:
				return def.totalHeight;
			case 1:
				return def.overviewTotalHeight;
		}
		return 0;
	}
	
	public int getZoom()
	{
		return zoom;
	}
	
	
}
