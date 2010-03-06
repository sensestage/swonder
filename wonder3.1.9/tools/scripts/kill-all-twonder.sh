#!/bin/bash

source cluster.conf

for i in $CLUSTER_NODES; do rsh $i sudo $SCRIPTS_PATH/node/node-kill-twonder.sh; done


