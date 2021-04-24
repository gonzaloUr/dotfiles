#!/bin/awk -f

function update_bar() {
  system("xsetroot -name ' [" bar["brg"] " " bar["vol"] "] [" bar["bat"] "] [" bar["clk"] "] '")
}

function get_backlight() {
  "xbacklight" | getline out
  close("xbacklight")

  split(out, a, ".")
  bar["brg"] = a[1] "%"
}

function get_volume() {
  default_sink_cmd = "pactl info | grep 'Default Sink' | sed 's/Default Sink: //'"
  default_sink_cmd | getline default_sink
  close(default_sink_cmd)

  volume_cmd = "pactl list sinks | sed -n '/" default_sink "/,/^Sink/p' | grep '^[[:space:]]*Volume:' | sed -n 's/^.* \\([[:digit:]][[:digit:]]*%\\).*$/\\1/p' | head -n1"
  volume_cmd | getline volume
  close(volume_cmd)

  bar["vol"] = volume
}

function get_clock() {
  clock_cmd = "date '+%b %d %a %H:%M:%S'"
  clock_cmd | getline bar["clk"]
  close(clock_cmd)
}

function get_bat() {
  bat_cmd = "cat /sys/class/power_supply/BAT0/capacity"
  bat_cmd | getline bar["bat"]
  bar["bat"] = bar["bat"] "%"
  close(bat_cmd)
}

BEGIN {
  bar["vol"] = "-%"
  get_backlight()
  get_clock()
  get_bat()

  update_bar()
}

/^video\/brightness/ {
  get_backlight()
  update_bar()
}

/^button\/volume/ {
  get_volume()
  update_bar()
}

/^[[:digit:]]{1,2} [[:digit:]]{1,2} [[:digit:]]{1,2}$/ {
  get_clock()

  if ($3 == "00") {
    get_bat()
  }

  update_bar()
}
