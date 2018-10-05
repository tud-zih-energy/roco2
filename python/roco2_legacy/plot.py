import numpy as np
from matplotlib import pyplot


def ideal(x=1, y=None, steps=100, ls=':', color='black', linewidth=.5, **kwargs):
    l_x = np.linspace(0, x, steps)
    if y is None:
        l_y = l_x
    else:
        l_y = np.linspace(0, y, steps)
    pyplot.plot(l_x, l_y, ls=ls, color=color, linewidth=linewidth)
