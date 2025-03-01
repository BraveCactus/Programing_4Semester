import numpy as np
import matplotlib.pyplot as plt
from time import time
from numpy.lib.stride_tricks import sliding_window_view
import cv2

def conv_nested(image, kernel):
    
    image = np.clip(image, 0, 255).astype(np.uint8) #переводим изображение в беззнаковый тип
    height, weight = image.shape[:2]    

    new_kernel = np.fliplr(np.flipud(kernel))
    kernel_size = new_kernel.shape[0]
    reserve = kernel_size // 2 #Считаем отступ

    result_image = np.zeros_like(image).astype(float) #переводим изображение к виду дробного числа с плавающей точкой

    for i in range(reserve, height - reserve):
        for j in range(reserve, weight - reserve):        
            for m in range(kernel_size):
                for n in range(kernel_size):
                    pixel_value = image[i + m - reserve, j + n - reserve]
                    result_image[i, j] += new_kernel[m, n] * pixel_value      

    return result_image

def zero_pad(image, pad_height, pad_width):    
    padded_img = np.pad(image,((pad_height, pad_height), (pad_width, pad_width)), mode = 'constant')    
    return padded_img

def conv_fast(image, kernel):
    kernel_height, kernel_width = kernel.shape[:2]

    new_kernel = np.fliplr(np.flipud(kernel)) # Отображаем ядро по горизонтали и вертикали
    result_image = zero_pad(image, kernel_height//2, kernel_width//2) #Добавляем padding

    windows = sliding_window_view(result_image, (kernel_height, kernel_width)) #Скользящее окно

    result_image = np.tensordot(windows, new_kernel, axes=([2, 3], [0, 1])) #Суммируем соседние пиксели

    print(result_image.astype(np.uint8).shape)
    print(image.shape)

    return result_image[1:-1][1:-1] #Возвращаем результат без отступов
