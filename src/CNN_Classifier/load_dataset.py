import numpy as np
import os 
from skimage.io import imread
import random
import time
from skimage.transform import resize


"""
Bu fonksiyonda veriseti gezilir ve model tarafından kullanılacak hale getirilir.
@return eğitim tensorü. Tüm resimlerin %80'i. Shape (None, 200, 200, 3)
@return test tensorü. Tüm resimlerin %20'si. Shape (None, 200, 200, 3)
@return eğitim sınıfları vektörü . 
@return test sınıfları vektörü.
"""
def load_data():
    path = "Dataset/"
    print(os.listdir(path))
    labels = []
    images = []
    #Her resim okunduktan hemen sonra 200x200 formatına getirilir.
    class_no = 0
    for p in os.listdir(path):
        for i in os.listdir(path + "/" +p):
            img = imread(path + "/" +p  + "/"+ i)
            img = resize(img, [200,200,3])
            images.append(img)
            labels.append(class_no)
        class_no += 1
    
    """
    Bu döngü aslında çok gerekli değil. Sadece test'te her sınıftan 5'ten fazla
    olmasının sağlanması için yaptım.
    """
    flag = False
    count = int(0.8 * len(images))

    while (flag == False):
        combined = list(zip(images, labels))
        random.shuffle(combined)
        images[:], labels[:] = zip(*combined)
        cnt = 0
        for i in range(0,10):
            if labels[count:].count(i) > 5:
                cnt += 1
            else:
                break
        if (cnt ==  10):
            flag = True

    #numpy dizisi olarak döndürülür.
    images = np.asarray(images)
    return images[0:count], images[count:],labels[0:count],labels[count:]