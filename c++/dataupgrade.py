import warnings
warnings.filterwarnings("ignore", message="A new version of Albumentations is available")
import albumentations as A
import cv2
import numpy as np
import matplotlib.pyplot as plt

# 定义数据增强变换
transform = A.Compose([
    A.HorizontalFlip(p=0.5),
    A.RandomBrightnessContrast(p=0.2),
    A.Rotate(limit=30, p=0.5),
])

image_path = "D:\Galaxy\其他\桌面\Polaris_vscode\c++\\4.png"  # 使用原始字符串
image = cv2.imread(image_path)

# 检查图像是否成功加载
if image is None:
    print("Image not found or unable to load.")
    exit(1)

# 将BGR转换为RGB（OpenCV默认读取为BGR，但显示时需要RGB）
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# 应用数据增强
augmented = transform(image=image_rgb)
augmented_image = augmented['image']

# 显示原始图像和增强后的图像
plt.figure(figsize=(10, 5))

# 显示原始图像
plt.subplot(1, 2, 1)
plt.imshow(image_rgb)
plt.title('Original Image')
plt.axis('off')

# 显示增强后的图像
plt.subplot(1, 2, 2)
plt.imshow(augmented_image)
plt.title('Augmented Image')
plt.axis('off')

plt.tight_layout()
plt.show()

# 使用OpenCV显示图像（需要转换回BGR格式）
# augmented_image_bgr = cv2.cvtColor(augmented_image, cv2.COLOR_RGB2BGR)
# cv2.imshow('Original Image', image)
# cv2.imshow('Augmented Image', augmented_image_bgr)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

# 保存增强后的图像
# output_path = r"D:\Galaxy\其他\桌面\Polaris_vscode\c++\4_augmented.png"
# cv2.imwrite(output_path, cv2.cvtColor(augmented_image, cv2.COLOR_RGB2BGR))