#!/bin/bash

source cluster.conf

rsh n-101 sudo $SCRIPTS_PATH/node/node-start-jfwonder.sh
