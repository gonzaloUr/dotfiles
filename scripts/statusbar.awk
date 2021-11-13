#!/bin/awk -f

function update_bar() {
    system("xsetroot -name " "' \x01"  bar["brg"] " | \x02" bar["vol"] " | \x03" bar["mic"] " | \x04" bar["bat"] " | \x05" bar["clk"] " '")
}

function get_brg() {
    "xbacklight" | getline out
    close("xbacklight")
    split(out, a, ".")
    return "🔆 " a[1] "%"
}

function get_vol() {
    default_sink_cmd = "pactl get-default-sink"
    default_sink_cmd | getline default_sink
    close(default_sink_cmd)

    volume_cmd = "pactl list sinks | sed -n '/" default_sink "/,/^Sink/p' | grep '^[[:space:]]*Volume:' | sed -n 's/^.* \\([[:digit:]][[:digit:]]*%\\).*$/\\1/p' | head -n1"
    volume_cmd = "pactl get-sink-volume '" default_sink "' | grep '^[[:space:]]*Volume' | sed -n 's/^.* \\([[:digit:]][[:digit:]]*%\\).*$/\\1/p'"
    volume_cmd | getline volume
    close(volume_cmd)

    active_sink_port_cmd = "pactl list sinks | sed -n '/" default_sink "/,/^Sink/p' | grep 'Active Port:' | sed 's/Active Port: \\(.*\\)/\\1/g ; s/[[:space:]]*//'"
    active_sink_port_cmd | getline active_sink_port
    close(active_sink_port_cmd)

    sub("^analog-output-", "", active_sink_port)

    return "🔊 " volume " " active_sink_port
}

function get_mic() {
    default_source_cmd = "pactl get-default-source"
    default_source_cmd | getline default_source
    close(default_source_cmd)

    source_volume_cmd = "pactl get-source-volume '" default_source "' | grep '^[[:space:]]*Volume' | sed -n 's/^.* \\([[:digit:]][[:digit:]]*%\\).*$/\\1/p'"
    source_volume_cmd | getline mic_volume
    close(source_volume_cmd)

    active_source_port_cmd = "pactl list sources | sed -n '/" default_source "/,/^Source/p' | grep 'Active Port:' | sed 's/Active Port: \\(.*\\)/\\1/g ; s/[[:space:]]*//'"
    active_source_port_cmd | getline active_source_port
    close(active_source_port_cmd)

    sub("^analog-input-", "", active_source_port)
    sub("-mic$", "", active_source_port)

    return "🎤 " mic_volume " " active_source_port
}

function get_clock() {
    clock_cmd = "date '+%d/%m %H:%M'"
    clock_cmd | getline ret
    close(clock_cmd)

    return "📅 " ret
}

function get_bat() {
    bat_cmd = "cat /sys/class/power_supply/BAT0/capacity"
    bat_cmd | getline ret
    close(bat_cmd)
    return "🔋 " ret "%"
}

BEGIN {
    bar["vol"] = "🔊 -%"
    bar["mic"] = "🎤 -%"
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

/Event 'change' on source #/ {
    bar["mic"] = get_mic()
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
