import cv2
import numpy as np
import matplotlib.pyplot as plt
from time import time
from collections import deque

np.set_printoptions(threshold=np.inf)

def simplify(image: np.ndarray, block_side: int) -> np.ndarray:    
    height, width = image.shape[0:2]

    indices = []
    for k in range(height // block_side):
        indices.append(block_side * k) 
        indices.append((block_side - 1) + block_side * k) 
    indices.append(image.shape[0] - 1)
    indices = sorted(indices)
    res_image = image[indices, :]
    
    indices = []
    for k in range(width // block_side):
        indices.append(block_side * k)  
        indices.append((block_side - 1) + block_side * k)  
    indices.append(image.shape[1] - 1)
    indices = sorted(indices)
    res_image = res_image[:, indices]
    
    return res_image

def find_opening_on_side(image: np.ndarray, side: int) -> tuple:    
    height, width = image.shape[0:2]
    
    if side == 1 or side == 3: 
        if side == 1:  
            for i in range(width):
                if image[0][i] == 255:
                    return 0, i
        else:  
            for i in range(width):
                if image[height - 1][i] == 255:
                    return height - 1, i
    else:  
        if side == 2:  
            for i in range(height):
                if image[i][0] == 255:
                    return i, 0
        else:  
            for i in range(height):
                if image[i][width - 1] == 255:
                    return i, width - 1

def find_path(matrix: np.ndarray, start: tuple, end: tuple) -> list:    
    n = len(matrix)
    visited = [[False for _ in range(n)] for _ in range(n)]
    parent = {}
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    queue = deque()
    queue.append(start)
    visited[start[0]][start[1]] = True

    while queue:
        current = queue.popleft()
        if current == end:
            break

        for dx, dy in directions:
            x = current[0] + dx
            y = current[1] + dy
            if 0 <= x < n and 0 <= y < n:
                if matrix[x][y] == 255 and not visited[x][y]:
                    visited[x][y] = True
                    parent[(x, y)] = current
                    queue.append((x, y))

    path = []
    current = end
    while current != start:
        path.append(current)
        current = parent[current]
    path.append(start)
    path.reverse()

    return path

def plot_maze_path(image):
    block_side = 16
    res_image = simplify(image, block_side)
    height, width = res_image.shape[0:2]

    start = find_opening_on_side(res_image, 1)
    end = find_opening_on_side(res_image, 3)

    path = find_path(res_image, start, end)
    for point in path:
        res_image[point[0]][point[1]] = 150 
    
    fig, axs = plt.subplots(1, 2, figsize=(12, 9))
    ax1, ax2 = axs

    ax1.set_title('Исходное изображение', fontsize=15)
    ax1.imshow(image, cmap='gray')
    ax2.set_title('Путь от входа до выхода', fontsize=15)
    ax2.imshow(res_image, cmap='gray')

    plt.show()



image = cv2.imread('task_1/maze1.png')
image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

plot_maze_path(image)

