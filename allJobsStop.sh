#!/bin/bash


while read -r line
do
	IFS=$'\t'
	set $line
    ./jobCommander stop $1
done < <(./jobCommander poll queued)

while read -r line
do
	IFS=$'\t'
	set $line
    ./jobCommander stop $1
done < <(./jobCommander poll running)




