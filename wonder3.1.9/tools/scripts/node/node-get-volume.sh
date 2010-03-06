#!/bin/bash

for i in `seq 15 78`; do amixer cget numid=$i; done
