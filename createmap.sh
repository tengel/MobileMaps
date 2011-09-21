#!/bin/bash
set -e

# This script creates a map file for the Java ME Map application. The resulting
# map file has the extension .mapth and contains maps of two different zoom 
# levels as well as the street index.
# The images are downloaded from openstreetmap.org. The street index is
# generated from the OSM Planet.osm file or extracts from it.
#
# Before you run the script you have to compile MapsIndexer and MapWriter.
# Furthermore, Python, osmosis and Java have to be installed.
# The index is generated from an .osm file. You have to download the planet.osm
# (or an excerpt of your area) from OpenStreetMap.org and set the PLANETOSM
# variable.
#

# path to planet.osm used for index
PLANETOSM=../planet.osm




if [ $# -lt 6 ]
then
    echo "usage: createmap.sh WEST NORTH EAST SOUTH ZOOM1 ZOOM2 [NAME]"
    echo "Create .mapth file for MobileMap. The output is written to ./maps/$NAME"
    echo 
    echo "  WEST, NORTH, EAST, SOUTH    coordinates of the desired area"
    echo "  ZOOM1, ZOOM2                zoomlevel of overview and detailed map (0..18, 0=whole world)"
    echo "  NAME                        name of output directory/file (default: map)"
    exit 2
fi

if [ $# -ge 7 ]
then
    NAME=$7
else
    NAME=map
fi

WEST=$1
NORTH=$2
EAST=$3
SOUTH=$4
ZOOM1=$5
ZOOM2=$6
NAME=$7
DIR=maps/$NAME

if [ -e $DIR ] ; then
    echo $0: $DIR: directory exists
    exit 1
fi

mkdir --parents $DIR

# download map images from openstreetmap.org
./getmap.pl $WEST $NORTH $EAST $SOUTH $ZOOM1 > $DIR/map1.png
./getmap.pl $WEST $NORTH $EAST $SOUTH $ZOOM2 > $DIR/map2.png

# create .osm file from area
osmosis --read-xml enableDateParsing=no file=$PLANETOSM --bounding-box top=$NORTH left=$WEST bottom=$SOUTH right=$EAST --write-xml file=$DIR/map.osm

# create nodes.txt and ways.txt from .osm file
./OSMParser/osmparse.py $DIR/map.osm
mv nodes.txt $DIR
mv ways.txt $DIR

# create mapindex.dat from nodes.txt and ways.txt
./MapsIndexer/mapindexer $DIR/nodes.txt $DIR/ways.txt $WEST $EAST $SOUTH $NORTH $DIR/mapindex.dat

# create .mapth file from images and mapindex.dat
java -classpath MapWriter/build/classes mapwriter.Main $DIR/map1.png $DIR/map2.png $DIR/mapindex.dat $DIR/$NAME.mapth

echo -e "$NAME\nwest: $WEST\nnorth: $NORTH\neast: $EAST\nsouth: $SOUTH\nzoom1: $ZOOM1\nzoom2: $ZOOM2\n" > $DIR/$NAME.txt
