package thutt;

import javax.microedition.lcdui.*;
import java.util.*;

/* TextDisplay
 * 
 * This Displayable allows you to display a formatted page.
 * 
 */

public class TextDisplay extends Canvas
{
	// Create a new text display. txt is parse when shown.
	TextDisplay(String txt)
	{
		if (txt == null)
			txt = "";
		
		text = txt;
	}
	
	// The text to parse.
	String text;

	// A single line of text. Can be heading or normal text.
	class Line
	{
		public Line(String text, boolean heading)
		{
			t = text;
			h = heading;
		}

		public String text()
		{
			return t;
		}

		public boolean heading()
		{
			return h;
		}

		String t;
		boolean h;
	}

	
	class Layout
	{
		Font textFont = null;
		Font headingFont = null;
		int totalHeight = 0;
		Vector lines = null;
		int parsePos = 0;
		
		public Layout(String s, int width)
		{
			headingFont = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_LARGE);
			textFont = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_PLAIN, Font.SIZE_MEDIUM);
			lines = new Vector();
			
			StringBuffer curLine = new StringBuffer();
			boolean heading = true;
			
			String w = getNextWord(s);
			while (!w.equals(""))
			{
				if (w.equals("#")) // Heading.
				{
					if (curLine.length() != 0)
						lines.addElement(new Line(curLine.toString(), heading));
					
					if (heading)
						totalHeight += headingFont.getHeight() * 2;
					else
						totalHeight += textFont.getHeight();
						
					heading = true;
					curLine.setLength(0);
				}
				else if (w.equals("~")) // Normal text.
				{
					lines.addElement(new Line(curLine.toString(), heading));

					if (heading)
						totalHeight += headingFont.getHeight() * 2;
					else
						totalHeight += textFont.getHeight();

					heading = false;
					curLine.setLength(0);
				}
				else
				{
					int newWidth = 0;
					if (heading)
						newWidth = headingFont.stringWidth(curLine.toString() + " " + w);
					else
						newWidth = textFont.stringWidth(curLine.toString() + " " + w);
					if (newWidth > width)
					{
						lines.addElement(new Line(curLine.toString(), heading));

						if (heading)
							totalHeight += headingFont.getHeight() * 2;
						else
							totalHeight += textFont.getHeight();

						
						curLine.setLength(0);
						curLine.append(w);
					}
					else
					{
						curLine.append(" " + w);
					}
				}
				w = getNextWord(s);
			}
			if (curLine.length() != 0)
			{
				lines.addElement(new Line(curLine.toString(), heading));
				if (heading)
					totalHeight += headingFont.getHeight() * 2;
				else
					totalHeight += textFont.getHeight();
			}				
		}
		
		public int height()
		{
			return totalHeight;
		}
		
		// Return the next space-delimited word from s, starting at i.
		private String getNextWord(String s)
		{		
			String w = "";
			
			if (parsePos >= s.length())
				return w;
			
			int e = s.indexOf(' ', parsePos);
			
			if (e == -1)
			{
				w = s.substring(parsePos);
				parsePos = s.length();
			}
			else
			{
				w = s.substring(parsePos, e);
				parsePos = e + 1;
			}
			
			return w;
		}
		
		public void paint(Graphics g)
		{
			int height = getHeight();
			
			int y = -currentPos;
			int lh = 0;
			for (int i = 0; i < lines.size(); ++i)
			{
				Line l = (Line)lines.elementAt(i);
				if (l.heading())
					lh = headingFont.getHeight() * 2;
				else
					lh = textFont.getHeight();
				
				if (y + lh > 0 && y < height)
				{
					g.setFont(l.heading() ? headingFont : textFont);
					g.drawString(l.text(), 4, l.heading() ? y + 2 * lh / 5 : y, Graphics.TOP | Graphics.LEFT);
				}
				y += lh;
			}
		}
	}
	
	Layout layout = null;
	int currentPos = 0;
	int desiredPos = 0;
	
	protected void paint(Graphics g)
	{		
		int width = getWidth();
		int height = getHeight();
		
		g.setColor(0xf6f2dd); // London A-Z background colour.
		g.fillRect(0, 0, width, height);
		
		g.setColor(0x000000);
		if (layout == null)
			layout = new Layout(text, width-4); // Small margin.
		
		layout.paint(g);
	}
	
	protected void keyPressed(int keyCode)
	{
		if (scroller != null)
		{
			scroller.cancel();
			scroller = null;
		}

		int height = getHeight();
		
		if (getGameAction(keyCode) == DOWN || keyCode == KEY_NUM8)
			desiredPos += 2*height/4;
		else if (getGameAction(keyCode) == UP || keyCode == KEY_NUM2)
			desiredPos -= 2*height/4;
		else
			return;

		if (desiredPos < 0)
			desiredPos = 0;
		if (desiredPos > layout.height() - getHeight())
			desiredPos = layout.height() - getHeight();
		
		scroller = new Scroller();
		timer.schedule(scroller, 0, 1000/30);
	}
	
	private Timer timer = new Timer();
	private Scroller scroller = null;
	
	protected void setPos(int y)
	{
		currentPos = y;
		if (currentPos < 0)
			currentPos = 0;
		if (layout == null)
		{
			currentPos = 0;
			return;
		}
		if (currentPos > layout.height() - getHeight())
			currentPos = layout.height() - getHeight();
	}
	
	protected int getPos()
	{
		return currentPos;
	}
	
	class Scroller extends TimerTask
	{
		private final static int ANIM_FRAMES = 15;
		private final double[] ANIM_FACTORS = { 0.5, 0.5, 0.5, 0.5, 0.5,
												0.4, 0.4, 0.35, 0.35, 0.3,
												0.3, 0.25, 0.25, 0.2, 0.2};
		
		private int framesLeft = ANIM_FRAMES;

		public void run()
		{
			int diff = desiredPos - getPos();
			diff *= ANIM_FACTORS[framesLeft-1];
			setPos(getPos() + diff);
			
			repaint();
			
			if (--framesLeft == 0)
			{
				cancel();
				scroller = null;
			}
        }
	}
}
