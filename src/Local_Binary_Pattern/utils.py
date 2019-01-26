import numpy as np
from PIL import Image



#convert a rgb value to the corresponding hsv value
def convert_to_hsv(r, g, b):
    r, g, b = r/255.0, g/255.0, b/255.0
    mx = max(r, g, b)
    mn = min(r, g, b)
    df = mx-mn
    if mx == mn:
        h = 0
    elif mx == r:
        h = (60 * ((g-b)/df) + 360) % 360
    elif mx == g:
        h = (60 * ((b-r)/df) + 120) % 360
    elif mx == b:
        h = (60 * ((r-g)/df) + 240) % 360
    if mx == 0:
        s = 0
    else:
        s = df/mx
    v = mx
    return h, s, v


#convert rgb image to hsv image
def convert_image_hsv(img):
    new_image = np.zeros(shape=(500,500,3))
    for i in range(500):
        for j in range(500):
            h, s, v = convert_to_hsv(img[i][j][0], img[i][j][1], img[i][j][2])
            new_image[i][j] = h,s,v
    return new_image

#read image from path and convert to yuv format. Returns numpy matrix
def read_image_yuv(path):
    im = Image.open(path)
    im = im.resize([500,500])
    img_yuv = im.convert('YCbCr')
    img = np.asarray(img_yuv)
    return img



# calculate hsv histogram of an image
def calculate_histogram(img):
    histogram = np.zeros(shape=(360,), dtype=float)
    for i in range(500):
        for j in range(500):
            histogram[int(img[i][j][0])] += 1
    div = 500*500
    i = 0
    while i < 360:
        histogram[i] /= div
        i += 1
    return histogram

#read and resize image
def read_image(path):
    im = Image.open(path)
    im = im.resize([500,500])
    img = np.asarray(im)
    return img

#generate hue histogram from a given path. Uses functions defined above
def histogram_operation(path):
    img = read_image(path)
    cv = convert_image_hsv(img)
    histogram = calculate_histogram(cv)
    return histogram

#calculate L1 distance between two image histograms.
def calc_distance(his1, his2):
    i = 0
    total = 0.0
    while(i < len(his1)):
        total += abs(his1[i] - his2[i]) 
        i += 1
    return total

#generates mappings for 58 uniform LBP values. 
def generate_LBP_dictionary():
    LBP_DICTIONARY = dict()
    position = 0
    for t in range(0,256):
        cur_val = t
        bitlist = np.zeros(shape=(8))
        div = 2
        for i in range(0,8):
            if ( cur_val % div == 1):
                bitlist[i] = 1
            else:
                bitlist[i]  = 0
            cur_val = int(cur_val/div)
        flag = bitlist[0]
        counter = 0
        for i in range(1,8):
            if bitlist[i] != flag:
                flag = bitlist[i]
                counter += 1
        if (counter <= 2):
            LBP_DICTIONARY[t] = position
            position += 1

    # print(LBP_DICTIONARY.keys())
    return LBP_DICTIONARY

#generates lbp matrix from an yuv image using the Y value
def calc_LBP(img):
    lbp_matrix = np.zeros(shape=(500,500))
    for i in range(1,499):
        bitlist = np.zeros(shape=(8))
        for j in range(1,499):
            cur_val = img[i][j][2]
            bitlist[0] = 1 if img[i-1][j-1][0] > cur_val else 0
            bitlist[1] = 1 if img[i-1][j][0] > cur_val else 0
            bitlist[2] = 1 if img[i-1][j+1][0] > cur_val else 0
            bitlist[3] = 1 if img[i][j-1][0] > cur_val else 0
            bitlist[4] = 1 if img[i][j+1][0] > cur_val else 0
            bitlist[5] = 1 if img[i+1][j-1][0] > cur_val else 0
            bitlist[6] = 1 if img[i+1][j][0] > cur_val else 0
            bitlist[7] = 1 if img[i+1][j+1][0] > cur_val else 0

            val = 0
            factor = 128
            for k in range(8):
                val += factor*bitlist[k]
                factor /= 2

            lbp_matrix[i][j] = val

    return lbp_matrix


#Given a lbp matrix, this method generates lbp histogram
def generate_LBP_histogram(img, dictionary):
    histogram = np.zeros(shape=(59))
    for i in range(1,499):
        for j in range(1,499):
            cur_val = int(img[i][j])

            if cur_val in dictionary.keys():
                pos = dictionary[cur_val]
                histogram[pos] += 1  
            else:
                histogram[58] += 1    
    i = 0
    div = 500*500
    while i < 59:
        histogram[i] /= div
        i += 1         
    return histogram
            


#Using methods defined above, return lbp histogram from an image path
def LBP_operation(path):
    cv = read_image_yuv(path)
    lbp_pic = calc_LBP(cv)
    LBP_DICTIONARY = generate_LBP_dictionary()
    histogram = generate_LBP_histogram(lbp_pic, LBP_DICTIONARY)
    return histogram

#find 5 most similar histograms for a test image, using either hue histogram or lbp
def get_similars(test, training_set):
    similarity_list = np.zeros(shape=(len(training_set)))

    i = 0
    while(i < len(training_set)):
        dist = calc_distance(test, training_set[i])
        similarity_list[i] = dist
        i += 1
    return np.argsort(similarity_list)[0:5]

#find 5 most similar histograms for a test image usign both hue and lbp histograms
def get_similars2(test1,test2, training_set1, training_set2):
    similarity_list = np.zeros(shape=(len(training_set1)))
    i = 0
    while(i < len(training_set1)):
        dist1 = calc_distance(test1, training_set1[i])
        dist2 = calc_distance(test2, training_set2[i])
        dist = dist1 + dist2
        similarity_list[i] = dist
        i += 1
    return np.argsort(similarity_list)[0:5]

#given a path, this method finds the 5 most similar training images for the each test scenario.
def test_operation(path, h1, h2, f):
    hsv = histogram_operation(path)
    lbp = LBP_operation(path)

    a = get_similars(hsv,h1)
    b = get_similars(lbp,h2)
    c = get_similars2(hsv,lbp, h1, h2)

    print("Original Image =", str(path))
    for i in a:
        print(f[i])
    print("\n")

    print("Original Image =", str(path))
    for i in b:
        print(f[i])
    print("\n")

    print("Original Image =", str(path))
    for i in c:
        print(f[i])
    print("\n")