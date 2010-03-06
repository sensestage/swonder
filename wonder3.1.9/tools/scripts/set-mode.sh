#!/bin/bash

#### this file contains configuration information for the mode,
#### which is needed by the startup scripts

#### modes are divided into: stable/unstable, and live/nolive
#### the stable mode is the stable working version
#### the unstable mode is the development version
#### live means that live audio is processed, and thus the jack period is lower, as well as the number of sources

#### the argument options are: stable, unstable, live, nolive
#### the arguments can occur in any order
#### default is: nolive stable

SWMODE=stable
LIVEMODE=nolive

while [ "$1" != "" ]
	do
	case $1
	in
		stable) SWMODE=stable;;
		unstable) SWMODE=unstable;;
		live) LIVEMODE=live;;
		nolive) LIVEMODE=nolive;;
		*) ;;
	esac
	shift;
done

source current.mode

if [ "$SWMODE" != "$CURMODE" ]
	then
		sudo ./change-mode.sh $SWMODE
		source current.mode
fi

if [ "$LIVEMODE" = "live" ]
	then
		echo "JACK_PERIOD=128 NUM_SOURCES=16" > twonder.settings
	else
		echo "JACK_PERIOD=512 NUM_SOURCES=41" > twonder.settings
fi
