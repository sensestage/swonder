#!/bin/bash

JACKD_PERIOD=$1

#jackd -d alsa -r 44100 -p $JACKD_PERIOD 2>&1 >/dev/null </dev/null &


/sbin/start-stop-daemon --start --make-pidfile --pidfile /var/run/jackd.pid --exec /usr/bin/jackd -- -p 512 -d alsa -d hw:0 -r 44100 -p $JACKD_PERIOD &>/var/log/jackd.log &
