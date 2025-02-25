import cv2
import numpy as np
import matplotlib.pyplot as plt


image = cv2.imread('robots.jpg')
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

gray_image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)


lower_red1 = np.array([0, 100, 100])    
upper_red1 = np.array([10, 255, 255]) 
lower_red2 = np.array([160, 100, 100])  
upper_red2 = np.array([180, 255, 255])

img_hsv = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)
red_mask1 = cv2.inRange(img_hsv, lower_red1, upper_red1)
red_mask2 = cv2.inRange(img_hsv, lower_red2, upper_red2)

red_mask = red_mask1 | red_mask2
red_result = cv2.bitwise_and(image, image, mask = red_mask)

fig, axs = plt.subplots(1, 2, figsize = (12, 8))
ax1, ax2 = axs

ax1.set_title('Изображение 1')
ax1.axis('off')
ax1.imshow(image)

ax2.set_title('Изображение 2')
ax2.axis('off')
ax2.imshow(red_result)

plt.show()