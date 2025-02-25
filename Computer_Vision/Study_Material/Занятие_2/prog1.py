import cv2
import matplotlib.pyplot as plt
import numpy as np

SAMPLE_RATE = 44100  # Hertz
DURATION = 5  # Seconds

def generate_sine_wave(freq, sample_rate, duration):
    x = np.linspace(0, duration, sample_rate * duration, endpoint=False)
    frequencies = x * freq
    # 2pi because np.sin takes radians
    y = np.sin((2 * np.pi) * frequencies)
    return x, y

# Generate a 2 hertz sine wave that lasts for 5 seconds
x, y = generate_sine_wave(2, SAMPLE_RATE, DURATION)
plt.plot(x, y)
plt.show()

# image = cv2.imrad('robots2.jpg')
# image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# fig, ax = plt.subplots(1, 1, figsize = (12, 8))
# ax.imshow(image)
plt.show()