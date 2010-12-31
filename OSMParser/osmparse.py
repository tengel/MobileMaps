#!/usr/bin/python

from xml.dom import pulldom

doc = pulldom.parse("england.osm")
#outFile = open("england.txt", "w")

nodes = {}
ways = {}
pubs = []
atms = []

i = 0
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
				
			wayName = name.encode("latin-1", "replace")
			refList = [nd.getAttribute("ref") for nd in node.getElementsByTagName("nd")]
			
			if ways.has_key(wayName):
				ways[wayName].append([int(x) for x in refList])
			else:
				ways[wayName] = [int(x) for x in refList]
		
		i = i + 1
		if i % 100 == 0:
			print(i / 100)
				
print(nodes)
print(ways)
print(pubs)
print(atms)
		
#outFile.close()
