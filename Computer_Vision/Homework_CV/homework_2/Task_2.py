import numpy as np
import matplotlib.pyplot as plt
import cv2
from numpy.lib.stride_tricks import sliding_window_view

def zero_mean_cross_correlation(img, temp):
    
    temp_height, temp_width = temp.shape

    #Вычитаем среднее значение из шаблона
    temp_zero_mean = temp.astype(np.float32) - np.mean(temp.astype(np.float32))

    #Считаем отступ
    pad_height = temp_height // 2
    pad_width = temp_width // 2

    #Добавляем отступ
    img_padded = np.pad(img.astype(np.float32), ((pad_height, pad_height), (pad_width, pad_width)))

    windows = sliding_window_view(img_padded, (temp_height, temp_width))
    result_img = np.sum(windows * temp_zero_mean, axis=(2, 3))    

    return result_img