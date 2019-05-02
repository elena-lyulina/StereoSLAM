import matplotlib
import numpy as np
import matplotlib.pyplot as plt


def hist (file, num_bins, color, title) :
    data =[]
    with open (file) as f :
        for val in f.read ().split () :
            data.append (float(val))
        print (len (data))
        fig, ax = plt.subplots ()
        n, bins, patches = ax.hist (data, num_bins, facecolor = color)
        fig.tight_layout ()

    plt.title (title)


hist ("x_sobel.txt", 500, 'blue', 'X-sobel after counting')
plt.show ()
