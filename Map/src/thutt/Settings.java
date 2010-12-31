
package thutt;


import javax.microedition.rms.*;
import java.io.*;

public class Settings
{
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
				xPos = new Integer(dis.readInt());
				yPos = new Integer(dis.readInt());
				layer = new Integer(dis.readInt());
				zoom = new Integer(dis.readInt());
				mapLoc = dis.readUTF();
			}
		}
		catch (Exception rse)
		{
			xPos = null;
			yPos = null;
			layer = null;
			zoom = null;
			mapLoc = null;
		}
	}
	
	public void writeSettings()
	{
		if (xPos == null || yPos == null || layer == null || zoom == null || mapLoc == null)
			return;
		
		try
		{
			if (recordStore == null)
				recordStore = RecordStore.openRecordStore("settings", true);
			
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			DataOutputStream dos = new DataOutputStream(baos);
			dos.writeInt(xPos.intValue());
			dos.writeInt(yPos.intValue());
			dos.writeInt(layer.intValue());
			dos.writeInt(zoom.intValue());
			dos.writeUTF(mapLoc);
			
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
	
	public Integer getXPos()
	{
		return xPos;
	}
	
	public Integer getYPos()
	{
		return yPos;
	}
	
	public Integer getLayer()
	{
		return layer;
	}
	
	public Integer getZoom()
	{
		return zoom;
	}
	
	public String getMapLoc()
	{
		return mapLoc;
	}
	
	public void setXPos(int x)
	{
		xPos = new Integer(x);
	}
	
	public void setYPos(int y)
	{
		yPos = new Integer(y);
	}
	
	public void setLayer(int l)
	{
		layer = new Integer(l);
	}
	
	public void setZoom(int z)
	{
		zoom = new Integer(z);
	}
	
	public void setMapLoc(String l)
	{
		mapLoc = l;
	}
	
	// Recordstore - interface to phone.
	private RecordStore recordStore;
	
	// Settings: null for unknown.
	private Integer xPos = null;
	private Integer yPos = null;
	private Integer layer = null;
	private Integer zoom = null;
	private String mapLoc = null;
}