Map
    The Java ME Mobile Map application. Originally created for London maps.


OSMParser
    Python script to create nodes.txt and ways.txt from an .osm file. The scipt
    is not able to work on large files. You should use osmosis to create a small
    .osm file for your area from the large planet.osm.

MapsIndexer
    This C++ program creates a .dat file containing the street index from the
    previously generated nodes.txt and ways.txt.
    To build it, it should be sufficient to run 'make' in the MapsIndexer
    directory.

MapWriter
    This is a Java application. It combines the priviously generated index file
    (.dat) and the images for the overview and detailed map to a .mapth file.
    This file has to downloaded to the phone.
    Run 'ant compile' to build MapWriter.

createmap.sh
    A simple bash script which performs all the steps to create a map.
