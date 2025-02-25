import cv2
import skimage.io
import numpy as np
import matplotlib.pyplot as plt

image = cv2.imread('img/robots3.jpg')
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

blurred_image = cv2.GaussianBlur(image.copy(), (5, 5), 0)

fig, axs = plt.subplots(1, 2, figsize=(12, 8))
ax1, ax2 = axs

ax2.axis('off')
ax2.imshow(blurred_image)

ax1.axis('off')
ax1.imshow(image)
plt.show()
