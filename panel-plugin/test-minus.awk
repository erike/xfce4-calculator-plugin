#!/usr/bin/awk -f

function abs(x) {
    return (x < 0) ? -x : x
}

BEGIN{
    "./calctest '10 - 4 - 3'" | getline res
    if (abs(res - 3.0) > 1.0e-15) {
        print res
        exit 1
    } else
        exit 0
}
