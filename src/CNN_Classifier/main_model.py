"""
author: Ömer Seyrekbasan
Env: Windows 10 64 bit. Python 3.6.5. Visual Studio Code
contact: seyrekbasanomer@gmail.com
"""

from tensorflow.keras import layers
from tensorflow.keras import models
from load_dataset import load_data
from tensorflow.keras import optimizers
import numpy as np
from tensorflow.keras.utils import to_categorical
import matplotlib.pyplot as plt
from keras.models import load_model
from sklearn.metrics import confusion_matrix


#başarıyı hesaplamak için kullanılan fonksiyon
def calc_accuracy(prediction, actual):
    counter = 0
    index = 0
    while(index < len(prediction)):
        if (prediction[index] == actual[index]):
            counter += 1
        index += 1

    return counter/len(prediction)

#veriseti elde edilir
x_train, x_test, train_label, test_labels = load_data()

#sınıflar one-hot-encoding'e çevrilir
train_label = to_categorical(train_label, 10)
test_labels = to_categorical(test_labels, 10)

print("Data is ready...")
"""
Modelin tanımlandığı yer.
Bizim ilk girişi ve çıkışı vermemiz yeterli. Ara adımlar keras tarafından hesaplanır.
"""
model = models.Sequential()
#64 filtre, relu aktivasyon fonksiyonu, 3x3'lük filtreler. Padding yok ve stride = 1
model.add(layers.Conv2D(64, (3, 3),activation='relu',input_shape=(200, 200, 3)))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(64, (3, 3), activation='relu'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(128, (3, 3), activation='relu'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(128, (3, 3), activation='relu'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Flatten())
model.add(layers.Dense(512, activation='relu'))
#overfittingin önlenmesi için
model.add(layers.Dropout(0.2))
model.add(layers.Dense(512, activation='relu'))
model.add(layers.Dropout(0.2))
#Çıkışın alındığı yer. 10 sınıf olduğu için 10 adet nöron
model.add(layers.Dense(10, activation='sigmoid'))

#Bu fonksiyon modeling özetini çıkartır
model.summary()

#loss ve optimizasyon fonksiyonlari model kurulur.
model.compile(loss='categorical_crossentropy',
optimizer='adam',
metrics=['accuracy'])

#model burada eğitilir. Eğitim setinin %10'nunu validation için ayırdıö
loss = model.fit(x_train,train_label,validation_split = 0.1,batch_size=32, epochs=20,verbose = 1)

#model böyle kaydedilebilir.
model.save('my_model.h5')

print(loss.history.keys())
#  "Accuracy" burada çizilir
plt.plot(loss.history['acc'])
plt.plot(loss.history['val_acc'])
plt.title('model accuracy')
plt.ylabel('accuracy')
plt.xlabel('epoch')
plt.legend(['train', 'validation'], loc='upper left')
plt.show()
# "Loss" burada çizilir
plt.plot(loss.history['loss'])
plt.plot(loss.history['val_loss'])
plt.title('model loss')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.legend(['train', 'validation'], loc='upper left')
plt.show()

#modelden test setimiz için tahminde bulunmasını istiyoruz. Bize test seti boyutunda bir dizi döner.
PREDICTED_CLASSES = model.predict_classes(x_test, batch_size=32, verbose=1)

#onehot encoding ters hale getirilir.
test_labels = test_labels.argmax(axis=-1)
train_label = train_label.argmax(axis=-1)

#accuracy hesaplanır
acc = calc_accuracy(PREDICTED_CLASSES, test_labels)
print(acc)

#bu döngüyü test setinde hangi index'te hangi sınıftan var görmek için kullandım. Program açısından bir önemi yok
for i in range(0,len(test_labels)):
    print(i, "  ", test_labels[i])

#istenilen indiste bulunan bir resim hem çizilir hem de figüre yapılan tahmin basılır
z = 283
imgplot = plt.imshow(x_test[z])
plt.title("Actual Class:" + str(test_labels[z]) + "   Predicted Class:" +str(PREDICTED_CLASSES[z]))
plt.show()

#modelin yaptığı tahminler sonucu elde edilen karmaşıklık matrisi
matrix = confusion_matrix(test_labels, PREDICTED_CLASSES)
print(matrix)

#burasının bir önemi yok. Her sınıftan kaç eğitim bilgisi var görmek için kullandın.
matrix = confusion_matrix(train_label, train_label)
print(matrix)

