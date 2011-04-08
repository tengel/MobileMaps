#include "Index.h"
#include "Transform.h"
#include "Util.h"
#include "Way.h"

#include <iostream>
#include <fstream>

using namespace std;

// 521588, 188030  ->   538766, 173061

// Approx: 51.575761, -0.238812   -> 51.439199, -0.003016

int main(int argc, char* argv[])
{
    if (argc != 8)
	{
		cout << "Usage: " << argv[0] << " nodes.txt ways.txt min_easting max_easting min_northing max_northing outputfile.dat" << endl;
		return 1;
	}
	
	string nodesFileName = argv[1];
	string waysFileName = argv[2];
	double eastMin = StoD(argv[3], -180.0);
	double eastMax = StoD(argv[4], 180.0);
	double northMin = StoD(argv[5], -90.0);
	double northMax = StoD(argv[6], 90.0);
	string outFileName = argv[7];
	
	Index idx;
	
	ifstream nodes(nodesFileName.c_str());
	ifstream ways(waysFileName.c_str());
	
	map<int, LatLong> nodeMap;
	
	if (!nodes)
	{
		cerr << "Error opening " << nodesFileName << endl;
		return 1;
	}
	
	cout << "Reading nodes" << endl;
	int i = 0;
	while (nodes)
	{
		int idx = -1;
		double lon = 0.0;
		double lat = 0.0;
		
		nodes >> idx >> lon >> lat;

		LatLong ll = LatLong(lat, lon);
		
		if (++i % 1000 == 0)
			cout << i/1000 << endl;
		
		if (idx != -1)
		{
                    if (lon >= eastMin && lon <= eastMax &&
                        lat >= northMin && lat <= northMax)
                    {
				nodeMap[idx] = ll;
                    }
		}
	}
	
	cout << "Reading ways" << endl;
	i = 0;
	map<string, Way> wayMap;
	
	while (ways)
	{
		if (++i % 1000 == 0)
			cout << i/1000 << endl;
		string name;
		std::getline(ways, name);
		
		string nds;
		std::getline(ways, nds);

		vector<string> nds_split = Split(nds, ",");
		
		for (int i = 0; i < nds_split.size(); ++i)
		{
			int n = StoI(nds_split[i], -1);
			if (nodeMap.count(n) == 1)
			{
				wayMap[name].addNode(nodeMap[n]);
			}
		}
	}

	for (map<string, Way>::const_iterator i = wayMap.begin(); i != wayMap.end(); ++i)
	{
		if (i->second.radius() > 5000) // 5 km I think.
		{
			// Assume it is two roads.
			bool isTall = i->second.isTall();
			
			LatLong r1 = i->second.getSouthOrWestAveragePosition();
			LatLong r2 = i->second.getNorthOrEastAveragePosition();
			
			idx.add(i->first + (isTall ? " (south)" : " (west)"),
                                0xFFFF * (r1.longitude - eastMin) / (eastMax - eastMin),
				0xFFFF * (r1.latitude - northMin) / (northMax - northMin));
                        
			idx.add(i->first + (isTall ? " (north)" : " (east)"),
                                0xFFFF * (r2.longitude - eastMin) / (eastMax - eastMin),
				0xFFFF * (r2.latitude - northMin) / (northMax - northMin));
		}
		else
		{
			LatLong ll = i->second.averagePosition();
			idx.add(i->first,
                                0xFFFF * (ll.longitude - eastMin) / (eastMax - eastMin),
				0xFFFF * (ll.latitude - northMin) / (northMax - northMin));
		}
	}
	
	idx.write(outFileName);
	
    return 0;
}

