/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package mapwriter;

import java.io.*;
import javax.imageio.*;
import java.awt.image.*;
import java.awt.*;

/**
 *
 * @author t
 */
public class Main
{
	final static int tileSize = 100;
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args)
	{
		if (args.length != 4)
		{
			System.out.println("Usage: mapwriter <input_map_image.png> <input_overview_map.png> <index_file.dat> <output_map.mapth>");
			return;
		}
		
		System.out.println("Reading from " + args[0] + " and " + args[1] + " and " + args[2] + " and writing to " + args[3]);

		File inLarge = null;
		File inSmall = null;
		File inIndex = null;
		try
		{
			inLarge = new File(args[0]);
			inSmall = new File(args[1]);
			inIndex = new File(args[2]);
			
			FileCacheWriter f = new FileCacheWriter();
			
			System.out.println("Reading image data.");

			BufferedImage bi = ImageIO.read(inLarge);
			
			System.out.println("Creating tiles.");
			
			int width = bi.getWidth();
			int height = bi.getHeight();
			
			// For now just clip the bottom, right hand sides. TODO: Fix.
			// We'll have our overview half size.
			
			int zoom = 2;
			
			int tilesX = width / tileSize;
			int tilesY = height / tileSize;
			tilesX = tilesX - tilesX % 4; // Use old value so I dont have to redo the street index.
			tilesY = tilesY - tilesY % 4;
			
			// Write image info.
			
			ByteArrayOutputStream headerData = new ByteArrayOutputStream();
			DataOutputStream header = new DataOutputStream(headerData);

			/** The reading code looks like:
			def.mapWidth = din.readInt(); // in tiles
			def.mapHeight = din.readInt();
			def.tileWidth = din.readInt(); // in pixels
			def.tileHeight = din.readInt();
			def.totalWidth = din.readInt(); // in pixels
			def.totalHeight = din.readInt();
			def.overviewMapWidth = din.readInt(); // in tiles
			def.overviewMapHeight = din.readInt();
			def.overviewTotalWidth = din.readInt(); // in pixels
			def.overviewTotalHeight = din.readInt();
			*/
			
			header.writeInt(tilesX);
			header.writeInt(tilesY);
			header.writeInt(tileSize);
			header.writeInt(tileSize);
			header.writeInt(tilesX * tileSize);
			header.writeInt(tilesY * tileSize);
			header.writeInt(tilesX / zoom);
			header.writeInt(tilesY / zoom);
			header.writeInt(tilesX * tileSize / zoom);
			header.writeInt(tilesY * tileSize / zoom);
			
			// Write the header.
			f.AppendFile(headerData.toByteArray());
			
			// Write the images to memory.

			for (int x = 0; x < tilesX; ++x)
			{
				for (int y = 0; y < tilesY; ++y)
				{
					ByteArrayOutputStream tile = new ByteArrayOutputStream();
					ImageIO.write(bi.getSubimage(x*tileSize, y*tileSize, tileSize, tileSize), "png", tile);
					f.AppendFile(tile.toByteArray());
				}
				System.out.println("Progress: A: " + x + " / " + tilesX);
			}
			
			bi = ImageIO.read(inSmall);
			
			for (int x = 0; x < tilesX / zoom; ++x)
			{
				for (int y = 0; y < tilesY / zoom; ++y)
				{
					ByteArrayOutputStream tile = new ByteArrayOutputStream();
					ImageIO.write(bi.getSubimage(x*tileSize, y*tileSize, tileSize, tileSize), "png", tile);
					f.AppendFile(tile.toByteArray());
				}
				System.out.println("Progress: A: " + x + " / " + tilesX/zoom);
			}
			
			FileInputStream fis = new FileInputStream(inIndex);
			byte[] idxData = new byte[(int)inIndex.length()];
			fis.read(idxData);
			
			f.AppendFile(idxData);
			
			f.WriteToFile(args[3]);
			
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
}
