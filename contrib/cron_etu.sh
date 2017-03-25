#!/bin/bash
# Sample cron script for etu - ultra simple

progname=${0##*/}
toppid=$$


# Config
etu_bin=`whereis etu` # switch to which, locate - whatever

quality=80
height=100
width=120

bomb()
{
	cat >&2 <<ERRORMESSAGE

ERROR: $@
*** ${progname} aborted ***
ERRORMESSAGE
	kill ${toppid}
	exit 1
}


# change the args to src dst for easy entry
src=shift
dst=shift

etu_bin -s $src -d $dst  -h $height -w $width -q $quality ||
	bomb "Cannot execute ${etu_bin}"

exit 0


