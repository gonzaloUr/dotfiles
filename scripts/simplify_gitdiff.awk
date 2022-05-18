#!/bin/awk -f

BEGIN {
    hunk = 0
}

/^@@ -[[:digit:]]+,[[:digit:]]+ \+[[:digit:]]+,[[:digit:]]+ @@ ?.*$/ {
    sub(/ @@ .*$/, "", $0)
    sub(/ @@$/, "", $0)
    sub(/^@@ /, "", $0)
    sub(/-/, "", $0)
    sub(/+/, "", $0)
    split($0, a, " ")
    split(a[1], a1, ",")
    split(a[2], a2, ",")
    hunk = 1
    line_number_diff = 0
}

/^\+.*/ {
    if (!hunk) {
        next
    }
    sub(/^\+/, "", $0)
    print a2[1] + line_number_diff
    line_number_diff++
}

/^-.*/ {
    if (!hunk) {
        next
    }
    sub(/^-/, "", $0)
}

/^ .*/ {
    if (hunk) {
        line_number_diff++
    }
}
