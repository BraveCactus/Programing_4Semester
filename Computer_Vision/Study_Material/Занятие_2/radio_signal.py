import numpy as np
import matplotlib.pyplot as plt
import cv2

# Генерация тестового радиолокационного сигнала с помехами
fs = 1000  # Частота дискретизации
t = np.linspace(0, 1, fs, endpoint=False)  # Временная ось
f_signal = 50  # Частота сигнала
f_noise = 200  # Частота помехи

# Создание чистого сигнала и добавление шумов
signal = np.sin(2 * np.pi * f_signal * t) + 0.5 * np.sin(2 * np.pi * f_noise * t) + np.random.normal(0, 0.5, fs)

# Применение БПФ
spectrum = np.fft.fft(signal)
frequencies = np.fft.fftfreq(len(spectrum), 1/fs)

# Извлечение модуля спектра и нахождение главной частоты
magnitude = np.abs(spectrum)
peak_frequency = frequencies[np.argmax(magnitude)]

# Визуализация
plt.figure(figsize=(12, 6))

# Визуализация временного сигнала
plt.subplot(2, 1, 1)
plt.plot(t, signal)
plt.title('Временной сигнал с помехами')
plt.xlabel('Время (с)')
plt.ylabel('Амплитуда')

# Визуализация спектра
plt.subplot(2, 1, 2)
plt.plot(frequencies[:fs//2], magnitude[:fs//2])  # Показать только положительные частоты
plt.title('Спектр сигнала')
plt.xlabel('Частота (Гц)')
plt.ylabel('Амплитуда')
plt.axvline(x=peak_frequency, color='r', linestyle='--', label=f'Главная частота: {peak_frequency:.2f} Гц')
plt.legend()

plt.tight_layout()
plt.show()

print(f'Главная частота: {peak_frequency:.2f} Гц')
