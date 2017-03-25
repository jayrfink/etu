#!/bin/bash
# Etu Daemon wrapper example complete with rc file.

progname=${0##*/}
toppid=$$


# Config
RCFILE=/usr/local/etc/eturc # change to your location
etu_bin=`whereis etu` # switch to which, locate - whatever

# Defaults
quality=80
height=100
width=120
interval=300

bomb()
{
	cat >&2 <<ERRORMESSAGE

ERROR: $@
*** ${progname} aborted ***
ERRORMESSAGE
	kill ${toppid}
	exit 1
}

# User can override the built in RCFILE
if [ $1 -eq "-c" ]; then
	RCFILE=$2
fi

src=$(grep SRCDIR $RCFILE 2>/dev/null)
dst=$(grep DSTDIR $RCFILE 2>/dev/null)
quality=$(grep QUALITY $RCFILE 2>/dev/null)
width=$(grep WIDTH $RCFILE 2>/dev/null)
height=$(grep HEIGHT $RCFILE 2>/dev/null)
interval=$(grep INTERVAL $RCFILE 2>/dev/null)


$etu_bin -D $interval -s $src -d $dst  -h $height -w $width -q $quality ||
	bomb "Cannot execute ${etu_bin}"

exit $?


