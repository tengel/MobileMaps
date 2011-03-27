#!/usr/bin/python

import sys
from xml.dom import pulldom

if len(sys.argv) < 2:
        print "usage: %s FILE" % sys.argv[0]
        sys.exit(2)

doc = pulldom.parse(sys.argv[1])
#outFile = open("england.txt", "w")

nodes = {}
ways = {}
pubs = []
atms = []

for event, node in doc:
	if event == pulldom.START_ELEMENT:
		if node.localName == "node":
			doc.expandNode(node)
			
			nodeID = node.getAttribute("id")
			nodeLat = node.getAttribute("lat")
			nodeLon = node.getAttribute("lon")

			amenity = "";
			for tag in node.getElementsByTagName("tag"):
				if tag.getAttribute("k") == "amenity":
					amenity = tag.getAttribute("v")
			
			nodes[int(nodeID)] = ( float(nodeLat), float(nodeLon) )
			
			if amenity == "pub":
				pubs.append(int(nodeID))
			elif amenity == "atm" or amenity == "bank":
				atms.append(int(nodeID))
				
		elif node.localName == "way":
			doc.expandNode(node)
			
			name = "";
			for tag in node.getElementsByTagName("tag"):
				if tag.getAttribute("k") == "name":
					name = tag.getAttribute("v")
			if name == "":
				continue;
				
			wayName = name.encode("utf-8", "replace")
			refList = [nd.getAttribute("ref") for nd in node.getElementsByTagName("nd")]
			
                        if not ways.has_key(wayName):
                                ways[wayName] = []
                                
                        for x in refList:
                                ways[wayName].append(int(x))

				
nodestxt = open("nodes.txt", 'w')
for n in nodes:
        s = "%d %f %f\n" % (n, nodes[n][1], nodes[n][0])
        nodestxt.write(s)
nodestxt.close()

waystxt = open("ways.txt", 'w')
for w in ways:
        waystxt.write(w)
        waystxt.write("\n")
        waystxt.write(str(ways[w])[1:-1])
        waystxt.write("\n")
waystxt.close()
