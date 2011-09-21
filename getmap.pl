#!/usr/bin/perl
use strict;
use LWP;
use GD;
use Math::Trig;

sub getTileNumber {
  my ($lat,$lon,$zoom) = @_;
  my $xtile = int( ($lon+180)/360 *2**$zoom ) ;
  my $ytile = int( (1 - log(tan(deg2rad($lat)) + sec(deg2rad($lat)))/pi)/2 *2**$zoom ) ;
  return ($xtile, $ytile);
}

if (scalar(@ARGV) < 5)
{
    print "usage: WEST NORTH EAST SOUTH ZOOM\r\n";
    exit(2);
}

# first top left (north west)
my $startlon = $ARGV[0];
my $startlat = $ARGV[1];
# then bottom right (south east)
my $endlon = $ARGV[2];
my $endlat = $ARGV[3];
#zoom level:
my $zoom = $ARGV[4];

(my $xstart, my $ystart) = getTileNumber($startlat, $startlon, $zoom);
(my $xend, my $yend) = getTileNumber($endlat, $endlon, $zoom);

my $tilesize = 256;
# calc image size
my $xsize = ($xend-$xstart+1) * $tilesize;
my $ysize = ($yend-$ystart+1) * $tilesize;

my $img = GD::Image->new($xsize, $ysize, 1);
my $white = $img->colorAllocate(248,248,248);
$img->filledRectangle(0,0,$xsize,$ysize,$white);
my $ua = LWP::UserAgent->new();
$ua->env_proxy;
for (my $x=$xstart;$x<=$xend;$x++) {
  for (my $y=$ystart;$y<=$yend;$y++) {
    my $url = sprintf("http://c.tile.openstreetmap.org/%d/%d/%d.png", $zoom, $x, $y);
    print STDERR "$url... ";
    my $resp = $ua->get($url);
    print STDERR $resp->status_line;
    print STDERR "\n";
    next unless $resp->is_success;
    my $tile = GD::Image->new($resp->content);
    next if ($tile->width == 1);
    $img->copy($tile, ($x-$xstart)*256,($y-$ystart)*256,0,0,256,256);
  }
}
$img->trueColorToPalette(0, 255);
binmode STDOUT;
print $img->png(9);
