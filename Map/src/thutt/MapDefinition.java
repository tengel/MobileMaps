
package thutt;


public class MapDefinition
{
	// Map size in tiles.
	public int mapWidth;
	public int mapHeight;
	
	// Tile size in pixels.
	public int tileWidth;
	public int tileHeight;
	
	// Map size in pixels.
	public int totalWidth;
	public int totalHeight;
	
	// Overview map size in tiles - they are the same size.
	public int overviewMapWidth;
	public int overviewMapHeight;
	
	// Overview map size in pixels.
	public int overviewTotalWidth;
	public int overviewTotalHeight;
}
/*
public class NewMapDefinition
{
	// There is one of these for each zoom level. Starting at the most zoomed.
	public class MapImageDefinition
	{
		// Tile size in pixels.
		public int tileWidth;
		public int tileHeight;

		// Map size in tiles.
		public int mapTileWidth;
		public int mapTileHeight;

		// Map size in pixels.
		public int mapPixelWidth;
		public int mapPixelHeight;
	}
	
	public Vector images;
	
	public class LayerDefinition
	{
		// Layer description, e.g. ('Pubs', 'Bus Stops', 'Phone Boxes', 'Cash Machines', 'Supermarkets')
		public String description;
		
		// Dot colour.
		public int colour;
	}
	
	public Vector layers;
}*/