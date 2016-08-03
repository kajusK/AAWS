#!/bin/bash
# Webcam capture script
#
# Run with cron like:
# */5 * * * * /path/to/webcam.sh
#
# Jakub Kaderka 2016
#

#number of files to keep stored, -1 to keep none
KEEP=-1

#Destination directory for images
IMGDIR=/srv/www/webcam
#Name of most recent image
LATEST=$IMGDIR/latest.jpg

mkdir -p $IMGDIR

date=$(date +"%A %d.%m.%Y %H:%M")
filename=img_$(date +"%d.%m.%Y_%H.%M.%S").jpg;

ffmpeg -f video4linux2 -i /dev/video0 -vframes 1 $IMGDIR/$filename &> /dev/null

#add text to image
width=$(identify -format %w $IMGDIR/$filename)
convert -fill '#0008' -draw "rectangle 0,0,${width},20" \
	-pointsize 20 -fill white -gravity North -annotate 0 "$date" \
	$IMGDIR/$filename $LATEST

#remove old
DELETE=$(ls $IMGDIR/img_* | sort -r | tail -n +$(($KEEP + 1)))
echo $DELETE
if [[ -n $DELETE ]]; then
	rm $DELETE
fi
