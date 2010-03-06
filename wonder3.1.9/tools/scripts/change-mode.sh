#!/bin/bash

#### this file contains configuration information for the current mode,
#### which is needed by the startup scripts
#### this script should be called with sudo

SWMODE=$1

source cluster.conf

if [ "$SWMODE" = "" ]
	then SWMODE=stable;
fi

if [ "$SWMODE" = "stable" ]
	then
		rm $BASEPATH/current
		ln -s $BASEPATH/stable $BASEPATH/current
		ldconfig
		echo "CURMODE=stable SPK_PATH=\"/home/torbenh/spks\" " > current.mode
fi

if [ "$SWMODE" = "unstable" ]
	then
		rm $BASEPATH/current
		ln -s $BASEPATH/unstable $BASEPATH/current
		ldconfig
		echo "CURMODE=unstable SPK_PATH=\"$BASEPATH/current/share/speakers\" " > current.mode
fi

#NOTE: once the stable version does not need the old speaker set anymore, we can move the SPK_PATH somewhere else

