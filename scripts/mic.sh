#!/bin/sh

default_source="$(pactl get-default-source)"
pactl get-source-volume "$default_source" |\
    grep '^[[:space:]]*Volume' |\
    sed -n 's/^.* \([[:digit:]][[:digit:]]*%\).*$/\1/p'
