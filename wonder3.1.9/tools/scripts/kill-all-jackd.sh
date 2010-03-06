#!/bin/bash

source cluster.conf

#this script kills all running jackd's on the audio rendering nodes

for i in $CLUSTER_NODES; do rsh $i sudo $SCRIPTS_PATH/node/node-kill-jackd.sh; done


