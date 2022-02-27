#!/bin/sh

to_seconds() {
    minutes=${1%%:*}
    seconds=${1##*:}
    echo $(( minutes * 60 + seconds ))
}

sum=0
count=0

while true
do
    read line
    [ -z "$line" ] && break
    out="$(to_seconds "$line")"
    sum=$((sum + out))
    count=$((count + 1))
done

avg=$((sum / count))
seconds=$((avg % 60))
minutes=$(((avg - secons) / 60))

echo "$minutes:$seconds"
