#!/bin/awk -f

function update_bar() {
    system("xsetroot -name " "' " bar["brg"] " " bar["vol"] " " bar["bat"] " " bar["clk"] " '")
}

function get_brg() {
    "xbacklight" | getline out
    close("xbacklight")
    split(out, a, ".")
    return "brg " a[1] "%"
}

function get_vol() {
    default_sink_cmd = "pactl info | grep 'Default Sink' | sed 's/Default Sink: //'"
    default_sink_cmd | getline default_sink
    close(default_sink_cmd)

    volume_cmd = "pactl list sinks | sed -n '/" default_sink "/,/^Sink/p' | grep '^[[:space:]]*Volume:' | sed -n 's/^.* \\([[:digit:]][[:digit:]]*%\\).*$/\\1/p' | head -n1"
    volume_cmd | getline volume
    close(volume_cmd)
    volume="vol " volume

    return volume
}

function get_clock() {
    clock_cmd = "date '+%b %d %a %H:%M:%S'"
    clock_cmd | getline ret
    close(clock_cmd)
    return ret
}

function get_bat() {
    bat_cmd = "cat /sys/class/power_supply/BAT0/capacity"
    bat_cmd | getline ret
    close(bat_cmd)
    return "bat " ret "%"
}

BEGIN {
    bar["vol"] = "-%"
    bar["brg"] = get_brg()
    bar["vol"] = get_vol()
    bar["bat"] = get_bat()
    bar["clk"] = get_clock()

    update_bar()
}

/^UDEV +\[[[:digit:]]+\.[[:digit:]]+\] +change +.* \(backlight\)$/ {
    bar["brg"] = get_brg()
    update_bar()
}

/Event 'change' on sink #/ {
    bar["vol"] = get_vol()
    update_bar()
}

/^[[:digit:]]{1,2} [[:digit:]]{1,2} [[:digit:]]{1,2}$/ {
    sub("^0", "", $3)

    bar["clk"] = get_clock()

    if ($3 % 30 == 0) {
        bar["bat"] = get_bat()
    }

    update_bar()
}
