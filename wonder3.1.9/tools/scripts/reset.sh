#!/bin/bash

./kill-all-twonder.sh
echo "10%"
./kill-all-jackd.sh
echo "20%"

#./setup-synco.sh
sudo ./restart-cwonder.sh
echo "30%"
sleep 3

./start-all-jackd.sh
echo "50%"
sleep 5 # wait a while for cwonder and jackd to have started properly
./start-jfwonder.sh
echo "55%"
./restart-twonder.sh
echo "100%"

