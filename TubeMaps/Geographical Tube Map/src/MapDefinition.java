public class MapDefinition
{
	public MapDefinition()
	{
		description = "Geographical Tube Map";
		fileBase = "/geographical_";
		xTiles = 13;
		yTiles = 10;
		tileWidth = 100;
		tileHeight = 100;
		totalWidth = 1280;
		totalHeight = 902;
		xCentre = 640;
		yCentre = 484;
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