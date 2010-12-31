from xml.dom import pulldom

doc = pulldom.parse("map.osm")
ways = open("ways.txt", "w")
nodes = open("nodes.txt", "w")

i = 0
for event, node in doc:
	if event == pulldom.START_ELEMENT:
		i = i + 1
		if node.localName == "node":
			doc.expandNode(node)
			nodes.write(node.getAttribute("id") + " " + node.getAttribute("lon") + " " + node.getAttribute("lat") + "\n")
		elif node.localName == "way":
			doc.expandNode(node)
			name = "";
			for tag in node.getElementsByTagName("tag"):
				if tag.getAttribute("k") == "name":
					name = tag.getAttribute("v")
	
			if name == "":
				continue;
			
			ways.write(name.encode("latin-1", "replace")) # Must be in a separate write for some silly encoding reason!
			ways.write("\n" + " ".join(nd.getAttribute("ref") for nd in node.getElementsByTagName("nd")) + "\n")
		
ways.close()
nodes.close()
