#!/bin/bash

source cluster.conf
source twonder.settings

for i in $CLUSTER_NODES; do rsh $i sudo $SCRIPTS_PATH/node/node-start-jackd.sh $JACKD_PERIOD; done

#rsh n-101 sudo $NODE_BINPATH/node/start_jfwonder.sh


