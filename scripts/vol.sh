#!/bin/sh

default_sink="$(pactl get-default-sink)"
pactl get-sink-volume "$default_sink" |\
    grep '^[[:space:]]*Volume' |\
    sed -n 's/^.* \([[:digit:]][[:digit:]]*%\).*$/\1/p'
