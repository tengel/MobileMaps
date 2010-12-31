import javax.microedition.rms.*;
import java.io.*;

public class Settings
{
	public Settings()
	{
		xPos = -1;
		yPos = -1;		
	}
	
	public void readSettings()
	{
		try
		{
			recordStore = RecordStore.openRecordStore("settings", true);
			
			if (recordStore.getNumRecords() >= 1)
			{
				byte[] record = recordStore.getRecord(1);
				ByteArrayInputStream bais = new	ByteArrayInputStream(record);
				DataInputStream dis = new DataInputStream(bais);
				xPos = dis.readInt();
				yPos = dis.readInt();
			}
		}
		catch (Exception rse)
		{
			xPos = -1;
			yPos = -1;
			rse.printStackTrace();
		}
	}
	
	public void writeSettings()
	{
		try
		{
			if (recordStore == null)
				recordStore = RecordStore.openRecordStore("settings", true);
			
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			DataOutputStream dos = new DataOutputStream(baos);
			dos.writeInt(xPos);
			dos.writeInt(yPos);
			
			byte[] b = baos.toByteArray();
			
			if (recordStore.getNumRecords() >= 1)
				recordStore.setRecord(1, b, 0, b.length);
			else
				recordStore.addRecord(b, 0, b.length);
			
			recordStore.closeRecordStore();
		}
		catch (Exception E)
		{
			E.printStackTrace();
		}
	}
	
	public int getXPos(int def)
	{
		return xPos != -1 ? xPos : def;
	}
	
	public int getYPos(int def)
	{
		return yPos != -1 ? yPos : def;
	}
	
	public void setXPos(int x)
	{
		xPos = x;
	}
	
	public void setYPos(int y)
	{
		yPos = y;
	}
	
	private RecordStore recordStore;
	
	// -1 for unknown.
	private int xPos;
	private int yPos;
}