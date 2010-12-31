public class MapDefinition
{
	public MapDefinition()
	{
		description = "Bus Map";
		fileBase = "/day_buses_";
		xTiles = 23;
		yTiles = 16;
		tileWidth = 100;
		tileHeight = 100;
		totalWidth = 22*100+64;
		totalHeight = 15*100+83;
		xCentre = totalWidth/2;
		yCentre = totalHeight/2;
	}
	public int xTiles;
	public int yTiles;
	public int tileWidth;
	public int tileHeight;
	public int totalWidth;
	public int totalHeight;
	public int xCentre;
	public int yCentre;
	public String description;
	public String fileBase;
}