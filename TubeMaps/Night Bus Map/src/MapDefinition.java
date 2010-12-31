public class MapDefinition
{
	public MapDefinition()
	{
		description = "Night Bus Map";
		fileBase = "/night_buses_";
		xTiles = 24;
		yTiles = 18;
		tileWidth = 100;
		tileHeight = 100;
		totalWidth = 23*100+93;
		totalHeight = 17*100+92;
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