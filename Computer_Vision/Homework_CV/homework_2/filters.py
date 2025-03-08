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
    
    height, width = image.shape

    result_height = 2*pad_height + height 
    result_width = 2*pad_width + width 

    result = np.zeros((result_height, result_width))    

    for i in range(pad_height, pad_height + height):
        for j in range(pad_width, pad_width + width):
            image_x = i-pad_height
            image_y = j-pad_width
            result[i, j] = image[image_x, image_y]

    return result

def conv_fast(image, kernel):    

    new_kernel = np.fliplr(np.flipud(kernel)) # Отображаем ядро по горизонтали и вертикали

    kernel_height, kernel_width = kernel.shape[:2]

    padding_x = kernel_height//2
    padding_y = kernel_width//2
    
    result_image = zero_pad(image, padding_x, padding_y) #Добавляем padding

    windows = sliding_window_view(result_image, (kernel_height, kernel_width)) #Скользящее окно

    result_image = np.tensordot(windows, new_kernel, axes=([2, 3], [0, 1])) #Суммируем соседние пиксели

    return result_image[1:-1][1:-1] #Возвращаем результат без отступов

def zero_mean_cross_correlation(img, template):  
    template_height, template_width = template.shape[:2]

    #Вычитаем среднее значение из шаблона
    template_zero_mean = template.astype(np.float32) - np.mean(template.astype(np.float32))

    #Считаем отступ
    pad_height = template_height // 2
    pad_width = template_width // 2

    #Добавляем отступ
    img_padded = zero_pad(img, pad_height, pad_width)  

    windows = sliding_window_view(img_padded, (template_height, template_width))
    result_img = np.sum(windows * template_zero_mean, axis=(2, 3))    

    return result_img

def normalized_cross_correlation(img, template):    
    template_height, template_width = template.shape[:2]

    #Вычитаем среднее значение из шаблона
    template_zero_mean = template.astype(np.float32) - np.mean(template.astype(np.float32))

    #Считаем отступ
    pad_height = template_height // 2
    pad_width = template_width // 2    

    #Добавляем отступ
    img_padded = zero_pad(img, pad_height, pad_width)

    windows = sliding_window_view(img_padded, (template_height, template_width))

    win_stand_dev = np.std(windows, axis=(2, 3), keepdims=True)
    windows_norm = (windows - np.mean(windows, axis=(2, 3), keepdims=True)) / win_stand_dev

    temp_stand_dev = np.std(template_zero_mean)
    template_norm = (template_zero_mean - np.mean(template)) / temp_stand_dev
    
    result_img = np.sum(windows_norm * template_norm, axis=(2, 3))   

    return result_img

