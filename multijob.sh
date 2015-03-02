#!/bin/bash

for i
do
while read line
	do 
	$line
	echo ----------------------
	done < $i
done

exit 0
