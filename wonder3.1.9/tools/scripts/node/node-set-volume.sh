#!/bin/bash

for i in `seq 15 78`; do amixer cset numid=$i $1; done
