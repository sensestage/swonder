#!/bin/bash

source cluster.conf
for i in $CLUSTER_NODES; do rsh $i sudo $SCRIPTS_PATH/node/node-set-volume.sh $1; done

