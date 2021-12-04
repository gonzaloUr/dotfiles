#!/bin/sh

# get all known power supplies.
power_supplies=/sys/class/power_supply/*

# power supplies of type 'Bat' (batteries).
batts=
batts_length=0

for x in $power_supplies
do
    out="$(cat "$x/type")"

    if [ "$out" = 'Battery' ]
    then
        if [ $batts_length -eq 0 ]
        then
            batts="$x"
        else
            batts="$batts:$x"
        fi

        batts_length=$((batts_length + 1))
    fi
done

# if there is only one battery print its percentage.
if [ $batts_length -eq 1 ]
then
    printf '%s%%' "$(cat $batts/capacity)"

# if there are multiple batteries print name and percentage for each one of them.
elif [ $batts_length -gt 1 ]
then
    i=1
    tmp="$IFS"
    IFS=':'
    for x in $batts
    do
        name=${x##?*/}
        cap="$(cat $x/capacity)"

        if [ $i -ne $batts_length ]
        then
            printf '%s %s%% ' "$name" "$cap"
        else
            printf '%s %s%%' "$name" "$cap"
        fi

        i=$((i + 1))
    done
    IFS="$tmp"
fi
