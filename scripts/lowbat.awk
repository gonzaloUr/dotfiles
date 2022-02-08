#!/bin/awk -f

/^[[:digit:]]{1,2} [[:digit:]]{1,2} [[:digit:]]{1,2}$/ {
    sub("^0", "", $3)

    if ($3 % 30 == 0) {
        cmd = "cat /sys/class/power_supply/BAT0/capacity"
        cmd | getline capacity
        close(cmd)

        cmd = "cat /sys/class/power_supply/BAT0/status"
        cmd | getline status
        close(cmd)

        if (status != "Charging" && capacity <= 5) {
            system("loginctl suspend")
        }
    }
}
