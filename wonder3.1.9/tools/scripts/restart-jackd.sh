#!/bin/bash

JACKD_PERIOD=$1

/sbin/start-stop-daemon --start --make-pidfile --pidfile /var/run/jackd.pid --exec /usr/bin/jackd -- -d alsa -d hw:0 -r 44100 -p $JACKD_PERIOD &>/var/log/jackd.log &


