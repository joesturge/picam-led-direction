#!/bin/bash


for i in $(seq 1 40); do
  echo "$i" > /sys/class/gpio/unexport 2>/dev/null
done
