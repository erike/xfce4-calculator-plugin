#!/usr/bin/awk -f

function abs(x) {
    return (x < 0) ? -x : x
}

BEGIN{
    "./calctest '4.5 + 7/2'" | getline res
    if (abs(res - 8.0) > 1.0e-15) {
        print res
        exit 1
    } else
        exit 0
}
