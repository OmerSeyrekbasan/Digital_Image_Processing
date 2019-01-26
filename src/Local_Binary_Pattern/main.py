"""
author: Ömer Seyrekbasan
contact: seyrekbasanomer@gmail.com
ENV: Windows 10 64 bit Python 3.6.5
"""
import numpy as np
import utils
import os

HISTOGRAM_SAVE_PATH = "histograms/"
LBP_SAVE_PATH = "LBP/"

# hsv histogram generation
for root, dirs, files in os.walk("CBIR", topdown=False):
    for name in files:
        print(os.path.join(root, name))
        histogram = utils.histogram_operation(os.path.join(root, name))
        np.save(os.path.join(HISTOGRAM_SAVE_PATH, name), histogram)
        print(histogram)
print("Completed")

# LBP histogram generation
for root, dirs, files in os.walk("CBIR", topdown=False):
    for name in files:
        print(os.path.join(root, name))
        histogram = utils.LBP_operation(os.path.join(root, name))
        np.save(os.path.join(LBP_SAVE_PATH, name), histogram)
        print(histogram)
print("Completed")

#Get LBP histograms from files
LBP_list = []
filenames = []
for histogram in os.listdir(LBP_SAVE_PATH):
    arr = np.load(os.path.join(LBP_SAVE_PATH, histogram))
    filenames.append(histogram[:-4])
    LBP_list.append(arr)

# Get HSV histograms from files
histogram_list = []
for histogram in os.listdir(HISTOGRAM_SAVE_PATH):
    arr = np.load(os.path.join(HISTOGRAM_SAVE_PATH, histogram))
    histogram_list.append(arr)

utils.test_operation("Test/ice10.jpg", histogram_list, LBP_list, filenames)

print("Completed!")