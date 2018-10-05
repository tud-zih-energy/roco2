import itertools

markers = "xsov^Dp+<>*h8"


def marker(i):
    return markers[i % len(markers)]


def marker_cycle():
    return itertools.cycle(markers)