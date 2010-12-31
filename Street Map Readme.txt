Mobile London Street Map.

HOW TO BUILD

1. Download map tiles from streetmap.co.uk using MapWriter/maprip.py
2. Download the .osm file for the UK from openstreetmap.org (somewhere on there) and process it using (possibly) OSMParser/osmparse.py to get node.txt and ways.txt
3. Build MapsIndexer and run nodes.txt and ways.txt through it.
4. Use GIMP to create a small version of the giant map file.
5. Build MapWriter and use it with the previous files to map the London.mapth file.
6. Build the Map jar. Put both files on phone.

NEW STEPS FOR LAYERS (TODO)

1. Use the python script MapWriter/atmrip.py to download ATM locations.
