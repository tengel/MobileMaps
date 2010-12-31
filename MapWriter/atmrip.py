import urllib2;
import re;
import os;

# E.g. see this URL:  http://locator.link.co.uk/map/?wicket:interface=:1:map::IActivePageBehaviorListener:3:&amp;wicket:ignoreIfNotActive=true&center=(51.51453822629176,%20-0.1183176040649414)&bounds=((51.50919653172424,%20-0.13119220733642578),%20(51.51987929452591,%20-0.10544300079345703))&zoom=15&infoWindow.hidden=true&random=0.4959550190454967

lat_from = 51.509;
lat_to = 51.519;
lon_from = -0.131;
lon_to = -0.118;

page_loc = "http://locator.link.co.uk/map/?wicket:interface=:1:map::IActivePageBehaviorListener:3:&amp;wicket:ignoreIfNotActive=true&center=(%f, %f)&bounds=((%f, %f), (%f, %f))&zoom=15&infoWindow.hidden=false&random=0.123456789" % ((lat_from + lat_to)/2, (lon_from + lon_to)/2, lat_from, lon_from, lat_to, lon_to);

print page_loc;

page_req = urllib2.Request(page_loc);
page_req.add_header("Cookie", "JSESSIONID=CA7E5F7E29A92CB87938FBEE49F0743B; __utma=250470939.618796592544296800.1225138450.1225138450.1225144191.2; __utmc=250470939; __utmz=250470939.1225138450.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utmb=250470939.2.10.1225144191");

doc = urllib2.urlopen(page_req).read();

print doc;

p = re.compile(r"GLatLng\(\s*[0-9.-]*,\s*[0-9.-]");
		
urls = p.findall(doc);
		
r = re.compile(r"GLatLng\(");
	
urls = [r.sub("", u) for u in urls];

print urls;
