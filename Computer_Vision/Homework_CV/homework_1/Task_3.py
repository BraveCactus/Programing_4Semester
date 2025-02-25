import cv2
import numpy as np
import matplotlib.pyplot as plt

def rotate(image, angle):
    height, width, _ = image.shape 

    center_x = width // 2
    center_y = height // 2    
   
    rot_mat = cv2.getRotationMatrix2D((center_x, center_y), angle, scale=1)
    
    rot_image = cv2.warpAffine(image, rot_mat, (width, height)) 
   
    cos = abs(rot_mat[0, 0])
    sin = abs(rot_mat[0, 1])

    new_width = int((height * sin) + (width * cos))
    new_height = int((height * cos) + (width * sin))
    
    rot_mat[0, 2] += new_width / 2 - center_x
    rot_mat[1, 2] += new_height / 2 - center_y
    
    resized_rot_image = cv2.warpAffine(image, rot_mat, (new_width, new_height))
    
    fig, m_axs = plt.subplots(1, 2, figsize=(20, 8))
    ax1, ax2 = m_axs

    ax1.imshow(image)
    ax1.set_title('Original Image')
    ax1.axis('off')

    ax2.imshow(resized_rot_image)
    ax2.set_title('Rotated Image')
    ax2.axis('off') 

    plt.show() 

def doc_scanning(image):
    
    height, width = image.shape[:2]    
    image_hsv = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)
    
    low_red = (0, 0, 0)
    high_red = (5, 200, 200)
    
    notebook_mask = cv2.inRange(image_hsv, low_red, high_red)
    
    corners = [None, None, None, None]
    new_corners = np.float32([(0, height), (0, 0), (width, 0), (width, height)])

    for y in range(height):
        for x in range(width):
            if corners[1] is not None and corners[3] is not None:
                break
            
            if notebook_mask[y][x] != 0:
                corners[1] = (x, y) 
            
            if notebook_mask[height - y - 1][x] != 0:
                corners[3] = (x, height - y - 1) 
    
    for x in range(width):
        for y in range(height):
            if corners[0] is not None and corners[2] is not None:
                break
            
            if notebook_mask[y][x] != 0:
                corners[0] = (x, y)
            
            if notebook_mask[y][width - x - 1] != 0:
                corners[2] = (width - x - 1, y)   
    

    M = cv2.getPerspectiveTransform(np.float32(corners), new_corners)

    return cv2.warpPerspective(image, M, (width, height))


image = cv2.imread('task_3/lk.png')
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

image = cv2.imread('task_3/notebook.jpg')
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

notebook_image = doc_scanning(image_rgb)

fig, axs = plt.subplots(1, 2, figsize=(12, 8))
ax1, ax2 = axs

ax1.axis('off')
ax1.imshow(image_rgb)
ax1.set_title('Оригинальное изображение')

ax2.axis('off')
ax2.imshow(notebook_image)
ax2.set_title('Обработанное изображение')

plt.show()

rotate(image, 97)
