#!/usr/bin/awk -f

function abs(x) {
    return (x < 0) ? -x : x
}

BEGIN{
    expr = "'sqrt(1) + log(1) + ln(1) + exp(1) + sin(1) + cos(1) + tan(1) + asin(1) + arcsin(1) + acos(1) + arccos(1) + atan(1) + arctan(1) + log2(1) + log10(1) + lg(1) + abs(1) + cbrt(1)'"
    "./calctest " expr | getline res
    if (abs(res - 3.0) > 1.0e-15) {
        print res
        exit 1
    } else
        exit 0
}
