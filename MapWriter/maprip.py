import urllib;
import re;
import os;

# montage -adjoin -tile 35x -mode Concatenate *.gif out.png

# Each tile is 500 units (metres?) wide. We get five at a time, 2500 apart.

# From x=522520&y=174120 to x=539760&y=186280

xrange = range(522520, 539760, 2500);
yrange = range(174120, 188710, 2500);

print xrange, yrange;

print len(xrange) * len(yrange);

k = 0;

for x in xrange:
	for y in yrange:
		k = k + 1;
		print "%d%% Done" % (100*k/(len(xrange) * len(yrange)));
		doc = urllib.urlopen("http://www.streetmap.co.uk/newmap.srf?x=%d&y=%d&z=2" % (x, y)).read();
		p = re.compile(r"/image\.dll.*?loc2=..");
		
		urls = p.findall(doc);
		
		r = re.compile(r"street=Y.*loc2");
		
		urls = [r.sub("street=Y&loc2", u) for u in urls];
		
		offx = (0, 500, 1000, 1500, 2000) * 5;
		offy = (2000, 2000, 2000, 2000, 2000, 1500, 1500, 1500, 1500, 1500, 1000, 1000, 1000, 1000, 1000, 500, 500, 500, 500, 500, 0, 0, 0, 0, 0);
		#offy = (0, 0, 0, 0, 0, 500, 500, 500, 500, 500, 1000, 1000, 1000, 1000, 1000, 1500, 1500, 1500, 1500, 1500, 2000, 2000, 2000, 2000, 2000);
		
		for i, u in enumerate(urls):
			f = "%d_%d.gif" % (1000000 - (y + offy[i]), x + offx[i]); # So that sorting is correct.
			if not os.path.exists(f):
				print "Downloading http://www.streetmap.co.uk" + u + " to " + f;
				urllib.urlretrieve("http://www.streetmap.co.uk" + u, f);

