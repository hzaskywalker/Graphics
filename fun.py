from threading import Thread
import numpy as np
import cv2
import os
haha = '3.png'
os.system('make VERBOSE=1')

def name(i):
    return '{}_{}'.format(i, haha)

def work(i):
    os.system('./main 200 {} {} 1024 1024'.format(name(i), i))
    

num = 40
a = []
for i in range(num):
    a.append(Thread(group = None, target = work, args = (i,)))
for i in a:
    i.start()
for i in a:
    i.join()
imgs = []
for i in range(num):
    img = np.loadtxt(name(i) + '.txt')
    img = img.reshape(img.shape[0], -1, 3)
    print(img.shape)
    imgs.append(img.transpose(1, 0, 2))
    os.system('rm {}'.format(name(i)))
    #os.system('rm {}.txt'.format(name(i)))
img = np.mean(imgs, axis = 0)
cv2.imwrite(haha, np.minimum(img, 255).astype(np.uint8))
