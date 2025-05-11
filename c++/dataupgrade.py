import albumentations as A
import cv2
import numpy as np
import os
os.environ['NO_ALBUMENTATIONS_UPDATE'] = '1'

transform = A.Compose([
    A.HorizontalFlip(p = 0.5),
    A.RandomBrightnessContrast(p = 0.2),
    A.Rotate(limit = 30 , p = 0.5),
])

image_path = "/home/polaris/桌面/4.png"
image = cv2.imread(image_path)
image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

if(image is None):
    print("Image not found or unable to load.")
    exit(1)

if not isinstance(image, np.ndarray):
    print(f"错误: 图像不是NumPy数组，而是 {type(image)}")
    exit(1)

augmented = transform(image = image)
augmented_image = augmented['image']