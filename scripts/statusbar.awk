#!/bin/awk -f

function update_bar() {
    system("xsetroot -name ' " bar["brg"] " " bar["vol"] " " bar["mic"] " " bar["bat"] " " bar["clk"] " '")
}

function get_brg() {
    "xbacklight" | getline out
    close("xbacklight")
    split(out, a, ".")

    return "🔆 " a[1] "%"
    # return "brg " a[1] "%"
}

function get_vol() {
    vol_cmd = "vol.sh"
    vol_cmd | getline vol
    close(vol_cmd)

    return "🔊 " vol
    # return "vol " vol
}

function get_mic() {
    mic_vol_cmd = "mic.sh"
    mic_vol_cmd | getline mic_vol
    close(mic_vol_cmd)

    return "🎤 " mic_vol
    # return "mic " mic_vol
}

function get_clock() {
    clock_cmd = "date '+%a %d/%m %b %H:%M'"
    clock_cmd | getline ret
    close(clock_cmd)

    return "📅 " ret
    # return ret
}

function get_bat() {
    bat_cmd = "bat.sh"
    bat_cmd | getline ret
    close(bat_cmd)
    return "🔋 " ret
    # return "bat " ret
}

BEGIN {
    bar["vol"] = get_vol()
    bar["mic"] = get_mic()
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
