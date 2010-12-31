#ifndef TILEID_H
#define TILEID_H

// A useful tile ID class this is convenient for both OSM and Streetmap.co.uk, although the values
// mean different things in each case.

struct TileID
{
	// For OSM the x and y values are the tile indices. These can go from 0 to 2^zoom-1.
	// The zoom level is from 0-18 with 0 being the most zoomed out.

	// For streetmap.co.uk the x and y values are the grid reference (in metres) of the
	// lower left corner of the square. The zoom level is from 0 to 7 with 7 being the most zoomed out.

	int x;
	int y;
	int zoom;
};

inline bool operator==(const TileID& a, const TileID& b)
{
	return a.x == b.x && a.y == b.y && a.zoom == b.zoom;
}

inline uint qHash(const TileID& t)
{
	return qHash(t.x + t.y + t.zoom);
}

#endif // TILEID_H
