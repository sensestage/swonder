#!/bin/bash

source cluster.conf

for i in $CLUSTER_NODES; do rsh $i amixer cset numid=14 0; done
for i in $CLUSTER_NODES; do rsh $i amixer cset numid=14 1; done
for i in $OPTICAL_NODES; do rsh $i amixer cset numid=14 0; done

