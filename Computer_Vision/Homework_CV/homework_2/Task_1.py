import numpy as np
import matplotlib.pyplot as plt
from time import time
import cv2

def conv_nested(image, kernel):
    
    image = np.clip(image, 0, 255).astype(np.uint8)
    height, weight = image.shape[:2]    

    new_kernel = np.fliplr(np.flipud(kernel))
    kernel_size = new_kernel.shape[0]
    reserve = kernel_size // 2

    result_image = np.zeros_like(image).astype(float)   

    for i in range(reserve, height - reserve):
        for j in range(reserve, weight - reserve):        
            for m in range(kernel_size):
                for n in range(kernel_size):
                    pixel_value = image[i + m - reserve, j + n - reserve]
                    result_image[i, j] += new_kernel[m, n] * pixel_value      

    return result_image

