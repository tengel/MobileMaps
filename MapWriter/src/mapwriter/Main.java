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

            BufferedImage largeImage = ImageIO.read(inLarge);
            BufferedImage smallImage = ImageIO.read(inSmall);
			
            System.out.println("Creating tiles.");
			
            int largeWidth  = largeImage.getWidth();
            int largeHeight = largeImage.getHeight();
            int smallWidth  = smallImage.getWidth();
            int smallHeight = smallImage.getHeight();
            int largeTilesX = largeWidth  / tileSize;
            int largeTilesY = largeHeight / tileSize;
            int smallTilesX = smallWidth  / tileSize;
            int smallTilesY = smallHeight / tileSize;
                
                        
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
			
            header.writeInt(largeTilesX);
            header.writeInt(largeTilesY);
            header.writeInt(tileSize);
            header.writeInt(tileSize);
            header.writeInt(largeTilesX * tileSize);
            header.writeInt(largeTilesY * tileSize);
            header.writeInt(smallTilesX);
            header.writeInt(smallTilesY);
            header.writeInt(smallTilesX * tileSize);
            header.writeInt(smallTilesY * tileSize);
			
            // Write the header.
            f.AppendFile(headerData.toByteArray());
			
            // Write the images to memory.

            for (int x = 0; x < largeTilesX; ++x)
            {
                for (int y = 0; y < largeTilesY; ++y)
                {
                    ByteArrayOutputStream tile = new ByteArrayOutputStream();
                    System.out.println("x=" + x *tileSize + " y=" + y * tileSize);
                    ImageIO.write(largeImage.getSubimage(x * tileSize, y * tileSize, tileSize, tileSize), "png", tile);
                    f.AppendFile(tile.toByteArray());
                }
                System.out.println("Progress: A: " + x + " / " + largeTilesX);
            }
			
			
			
            for (int x = 0; x < smallTilesX; ++x)
            {
                for (int y = 0; y < smallTilesY; ++y)
                {
                    ByteArrayOutputStream tile = new ByteArrayOutputStream();
                    ImageIO.write(smallImage.getSubimage(x * tileSize, y * tileSize, tileSize, tileSize), "png", tile);
                    f.AppendFile(tile.toByteArray());
                }
                System.out.println("Progress: B: " + x + " / " + smallTilesX);
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
