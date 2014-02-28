#!/bin/bash

echo "Init" > /var/log/modet_crash.log

while :
do
	modet sync $1 272fedd8-df36-4368-b4de-965bc83936c9 >> /var/log/modet_crash.log 2>&1
	echo "Crashed!" >> /var/log/modet_crash.log

done
