#!/bin/sh
echo 'tput colors test'
echo
echo 'tput setaf [0-9] output sets foreground color'
echo 'tput setab [0-9] output sets background color'
echo 'tput sgr0        output turns off previous settings'
echo

for bg_color in $(seq 0 8)
do
    if [ $bg_color -ne 8 ]
    then
        echo -n "$(tput setab $bg_color)"
    fi
    for fg_color in $(seq 0 8)
    do
        if [ $fg_color -ne 8 ]
        then
            echo -n "$(tput setaf $fg_color)"
        fi
        printf ' F:%s B:%s ' "$fg_color" "$bg_color"
    done
    echo $(tput sgr0)
done

for fg_color in {0..7}; do
  set_foreground=$(tput setaf $fg_color)
  for bg_color in {0..7}; do
    set_background=$(tput setab $bg_color)
    echo -n $set_background$set_foreground
    printf ' F:%s B:%s ' $fg_color $bg_color
  done
  echo $(tput sgr0)
done
