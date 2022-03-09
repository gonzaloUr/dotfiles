#!/bin/sh

if [ "$1" = 'set' ] && [ -z "$2" ]
then
    out="$PWD/$(fzf)"
    mkdir -p "$XDG_CACHE_HOME/wallpaper_script/"
    [ -n "$out" ] && echo "$out" > "$XDG_CACHE_HOME/wallpaper_script/wallpaper"
fi

if ! [ -f "$XDG_CACHE_HOME/wallpaper_script/wallpaper" ]
then
    echo 'no wallpaper is set' >&2
    echo "please 'run wal.sh' set for interactive selection menu" >&2
    exit 1
fi

if pgrep xwallpaper
then
    pkill xwallpaper
fi

path="$(cat "$XDG_CACHE_HOME/wallpaper_script/wallpaper")"
xwallpaper --daemon --zoom "$path"
