import cv2
import numpy as np
import matplotlib.pyplot as plt

def road_counter(image, gray_area):   
    #Возвращает количество дорог
    road_number = 0
    height, width = gray_area.shape

    for i in range(1, width - 1):   
        if gray_area[406, i] == 255 and gray_area[406, i + 1] == 0:
            road_number += 1
    return road_number

def road_center(image, gray_area):   
    #Возвращает список из центров дорог
    height, width = gray_area.shape

    center_arr = list()

    for i in range(width - 1):
        if gray_area[406, i] == 0 and gray_area[406, i + 1] == 255:
            begin = i
            road_width = 0
            while (i < width and gray_area[406, i] != 0):
                road_width += 1
                i += 1
            center = 100 + (begin + (begin + road_width)) // 2
            center_arr.append(center)    
    return center_arr

def open_road(image, gray_area):
    #Возвращает список True/False: True - нет препятствий на i дороге, иначе - False
    road_number = road_counter(image, gray_area)
    center_arr = road_center(image, gray_area)

    height, width = gray_area.shape
    
    open_road_arr = [True] * min(5, len(center_arr))

    for i in range(len(open_road_arr)):
        for j in range(height - 1):                       
            if (gray_area[j][center_arr[i]] == 0):
                open_road_arr[i] = False
                break               
   
    return open_road_arr

def road_change(image, gray_area):
    #Возвращает 0, если машине не нужно перестраиваться, номер нужной дороги, если требуется 
    lower_red = np.array([0, 0, 200])
    upper_red = np.array([255, 200, 255])

    car_area = cv2.inRange(image, lower_red, upper_red)     
    open_roads_arr = open_road(image, gray_area)
    roads_center_arr = road_center(image, gray_area)

    height, width = car_area.shape

    for i in range(len(open_roads_arr)):
        if(open_roads_arr[i] == False):
            continue
        for j in range(height - 1):                              
            if (car_area[j][roads_center_arr[i]] != 0):
                #Машине не нужно перестраиваться                
                return -1
                             
    return open_roads_arr.index(True)  


def find_road_number(image):
    pix = 256
    low_gray = (0.9 * 213, 0.9 * 213, 0.9 * 213)
    high_gray = (1.1 * pix, 1.1 * pix, 1.1 * pix)

    gray_area = cv2.inRange(image, low_gray, high_gray)

    if(road_change(image, gray_area) == -1):
        print(f"Машине не нужно перестраиваться")
    else:
        print(f"Машине нужно перестроиться на {road_change(image, gray_area)} дорогу")

image = cv2.imread('task_2/road1.jpg')
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)



find_road_number(image)

