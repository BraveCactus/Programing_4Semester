import cv2
import skimage.io
import numpy as np
import matplotlib.pyplot as plt


dog_image = cv2.imread('img/dog.jpg')
dog_image = cv2.cvtColor(dog_image, cv2.COLOR_BGR2RGB)

con_dog_image = cv2.imread('img/convoluted_dog.jpg')
con_dog_image = cv2.cvtColor(con_dog_image, cv2.COLOR_BGR2RGB)

blurred_image = cv2.blur(dog_image.copy(), (9, 9))

fig, axs = plt.subplots(1, 3, figsize=(12, 8))
ax1, ax2, ax3 = axs

ax1.axis('off')
ax1.imshow(dog_image)

ax2.axis('off')
ax2.imshow(blurred_image)

ax3.axis('off')
ax3.imshow(con_dog_image)

plt.show()
