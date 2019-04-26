import matplotlib
import numpy as np
import matplotlib.pyplot as plt


def hist(file, num_bins, color, title):
    data = []
    with open(file) as f:
        for val in f.read().split():
            data.append(int(val))
    print(len(data))
    fig, ax = plt.subplots()
    n, bins, patches = ax.hist(data, num_bins, facecolor=color)
    fig.tight_layout()
    plt.title(title)





hist("/home/elena/workspaces/c++/StereoSLAM/res/x_sobel.txt", 1000, 'blue', 'X-sobel after counting')

hist("/home/elena/workspaces/c++/StereoSLAM/res/x_sobel_dsh.txt", 500, 'blue', 'X-sobel after division and shifting')

hist("/home/elena/workspaces/c++/StereoSLAM/res/x_sobel_conv.txt", 1000, 'blue', 'X-sobel after converting')

hist("/home/elena/workspaces/c++/StereoSLAM/res/y_sobel.txt", 1000, 'green', 'Y-sobel after counting')

hist("/home/elena/workspaces/c++/StereoSLAM/res/y_sobel_dsh.txt", 500, 'green', 'Y-sobel after counting')

hist("/home/elena/workspaces/c++/StereoSLAM/res/y_sobel_conv.txt", 500, 'green', 'Y-sobel after counting')

plt.show()






