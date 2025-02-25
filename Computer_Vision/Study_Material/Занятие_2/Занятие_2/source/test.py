import cv2
import numpy as np
import matplotlib.pyplot as plt

kernel = (1/3) * np.array([ [1, 0, 0],
                            [0, 0, 1],
                            [0, 1, 0]])


image = cv2.imread('img/dog.jpg')
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)


blurred_image = cv2.GaussianBlur(image, (11, 11), 0)

kernel1 = np.array([[2,0],[0,0]], dtype=np.float32)
dst = cv2.filter2D(image.copy(), -1, kernel)

added_image = cv2.add(image, blurred_image)
sub_image = cv2.subtract(image, blurred_image)

adsub_image = cv2.add(added_image, sub_image)


plt.figure(figsize=(12, 8))

plt.subplot(1, 4, 1)
plt.title('Оригинальное изображение')
plt.imshow(image)
plt.axis('off')

plt.subplot(1, 4, 2)
plt.title('Размытие Гаусса')
plt.imshow(blurred_image)
plt.axis('off')

plt.subplot(1, 4, 3)
plt.title('1 + 2')
plt.imshow(added_image)
plt.axis('off')

plt.subplot(1, 4, 4)
plt.title('1 - 2')
plt.imshow(sub_image)
plt.axis('off')


plt.show()
