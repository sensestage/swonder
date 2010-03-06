#!/bin/bash

## this script checks for wonder related programs on all nodes

source cluster.conf

for i in $CLUSTER_NODES; do echo $i; rsh $i pstree |grep twonder; rsh $i pstree |grep jfwonder; rsh $i pstree| grep jackd; done


echo "bender"
pstree |grep cwonder
pstree |grep jfwonder
pstree |grep scoreplayer
pstree |grep ardour
pstree |grep jackd
