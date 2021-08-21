#!/bin/bash

for (( n=117946 ; n< 1000000000 ; n++ )) ; do
	hash=$(echo -n "ckczppom$n" | md5sum)
	if grep '^00000' <<<$hash ; then
		echo $n
		break
	fi
done
