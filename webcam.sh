#!/bin/bash
# Webcam capture script
#
# Run with cron like:
# */5 * * * * /path/to/webcam.sh
#
# Jakub Kaderka 2016
#

#Destination directory for images
IMGDIR=/srv/www/webcam
#Name of most recent image
LATEST=$IMGDIR/latest.jpg

mkdir -p $IMGDIR

date=$(date +"%A %d.%m.%Y %H:%M")
filename=$(date +"%d.%m.%Y_%H.%M").jpg;

ffmpeg -f video4linux2 -i /dev/video0 -vframes 1 $IMGDIR/$filename &> /dev/null

width=$(identify -format %w $IMGDIR/$filename)
convert -fill '#0008' -draw "rectangle 0,0,${width},20" \
	-pointsize 20 -fill white -gravity North -annotate 0 "$date" \
	$IMGDIR/$filename $LATEST
