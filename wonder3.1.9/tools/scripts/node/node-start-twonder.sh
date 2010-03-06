#!/bin/bash

# /home/torbenh/swonder-svn/src/render/twonder/twonder -d -c /home/torbenh/swonder-svn/src/render/twonder/`hostname`/

NUM_SOURCES=$1
SPK_PATH=$2
SWONDER_CONF=$3
TWONDER_EXE=$4


#/home/torbenh/swonder-svn/src/render/twonder/twonder -d -c /home/torbenh/spks/`hostname`/ -s $NUM_SOURCES --swonderconf /opt/unstable/share/swonder/defaults.xml
$TWONDER_EXE -d -c $SPK_PATH/`hostname`/ -s $NUM_SOURCES --swonderconf $SWONDER_CONF -o 0.2
sleep 1

for i in `seq 1 56`; do jack_connect twonder:speaker$i alsa_pcm:playback_$i; done
for i in `seq 0 $[$NUM_SOURCES-1]`; do jack_connect alsa_pcm:capture_$[$i+1] twonder:input$i; done

