import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset
from torchvision import transforms, datasets
import matplotlib.pyplot as plt
from tqdm import tqdm
import numpy as np
import random
import os

# ==================== 超参数设置 ====================
config = {
    "seed": 42,         # 随机种子
    "batch_size": 32,   # 批大小
    "num_epochs": 10,   # 训练轮次
    "learning_rate": 1e-3,  # 学习率
    "num_workers": 4,   # 数据加载线程数
    "device": "cuda" if torch.cuda.is_available() else "cpu",  # 自动选择设备
    "save_dir": "./checkpoints"  # 模型保存路径
}

# 固定随机种子保证可重复性
def set_seed(seed):
    torch.manual_seed(seed)
    np.random.seed(seed)
    random.seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)

set_seed(config["seed"])

# ==================== 数据加载 ====================
# 自定义数据集类（示例使用MNIST）
class CustomDataset(Dataset):
    def __init__(self, train=True):
        self.transform = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize((0.1307,), (0.3081,))
        ])
        self.dataset = datasets.MNIST(
            root="./data",
            train=train,
            download=True,
            transform=self.transform
        )
    
    def __len__(self):
        return len(self.dataset)
    
    def __getitem__(self, idx):
        return self.dataset[idx]

# 创建数据集和数据加载器
train_dataset = CustomDataset(train=True)
val_dataset = CustomDataset(train=False)

train_loader = DataLoader(
    train_dataset,
    batch_size=config["batch_size"],
    shuffle=True,
    num_workers=config["num_workers"]
)

val_loader = DataLoader(
    val_dataset,
    batch_size=config["batch_size"],
    shuffle=False,
    num_workers=config["num_workers"]
)

# ==================== 模型定义 ====================
class SimpleCNN(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv_layers = nn.Sequential(
            nn.Conv2d(1, 16, 3, padding=1),
            nn.ReLU(),
            nn.MaxPool2d(2),
            nn.Conv2d(16, 32, 3, padding=1),
            nn.ReLU(),
            nn.MaxPool2d(2)
        )
        self.fc_layers = nn.Sequential(
            nn.Linear(32*7*7, 128),
            nn.ReLU(),
            nn.Dropout(0.5),
            nn.Linear(128, 10)
        )
    
    def forward(self, x):
        x = self.conv_layers(x)
        x = x.view(x.size(0), -1)
        return self.fc_layers(x)

model = SimpleCNN().to(config["device"])

# ==================== 损失函数与优化器 ====================
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=config["learning_rate"])

# ==================== 训练与验证 ====================
def train_epoch(model, loader):
    model.train()
    total_loss = 0.0
    correct = 0
    for inputs, targets in tqdm(loader, desc="Training"):
        inputs, targets = inputs.to(config["device"]), targets.to(config["device"])
        
        optimizer.zero_grad()
        outputs = model(inputs)
        loss = criterion(outputs, targets)
        loss.backward()
        optimizer.step()

        total_loss += loss.item() * inputs.size(0)
        _, predicted = outputs.max(1)
        correct += predicted.eq(targets).sum().item()
    
    avg_loss = total_loss / len(loader.dataset)
    accuracy = 100. * correct / len(loader.dataset)
    return avg_loss, accuracy

def validate(model, loader):
    model.eval()
    total_loss = 0.0
    correct = 0
    with torch.no_grad():
        for inputs, targets in tqdm(loader, desc="Validating"):
            inputs, targets = inputs.to(config["device"]), targets.to(config["device"])
            
            outputs = model(inputs)
            loss = criterion(outputs, targets)
            
            total_loss += loss.item() * inputs.size(0)
            _, predicted = outputs.max(1)
            correct += predicted.eq(targets).sum().item()
    
    avg_loss = total_loss / len(loader.dataset)
    accuracy = 100. * correct / len(loader.dataset)
    return avg_loss, accuracy

# ==================== 主训练循环 ====================
best_val_acc = 0.0
train_losses, val_losses = [], []
train_accs, val_accs = [], []

for epoch in range(config["num_epochs"]):
    print(f"\nEpoch {epoch+1}/{config['num_epochs']}")
    
    # 训练阶段
    train_loss, train_acc = train_epoch(model, train_loader)
    train_losses.append(train_loss)
    train_accs.append(train_acc)
    
    # 验证阶段
    val_loss, val_acc = validate(model, val_loader)
    val_losses.append(val_loss)
    val_accs.append(val_acc)
    
    # 保存最佳模型
    if val_acc > best_val_acc:
        best_val_acc = val_acc
        os.makedirs(config["save_dir"], exist_ok=True)
        torch.save(model.state_dict(), os.path.join(config["save_dir"], "best_model.pth"))
    
    print(f"Train Loss: {train_loss:.4f} | Acc: {train_acc:.2f}%")
    print(f"Val Loss: {val_loss:.4f} | Acc: {val_acc:.2f}%")

# ==================== 结果可视化 ====================
plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(train_losses, label='Train')
plt.plot(val_losses, label='Validation')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(train_accs, label='Train')
plt.plot(val_accs, label='Validation')
plt.xlabel('Epoch')
plt.ylabel('Accuracy (%)')
plt.legend()
plt.show()

# ==================== 测试与推理 ====================
def test(model, loader):
    model.eval()
    correct = 0
    with torch.no_grad():
        for inputs, targets in loader:
            inputs, targets = inputs.to(config["device"]), targets.to(config["device"])
            outputs = model(inputs)
            _, predicted = outputs.max(1)
            correct += predicted.eq(targets).sum().item()
    accuracy = 100. * correct / len(loader.dataset)
    print(f"Test Accuracy: {accuracy:.2f}%")

# 加载最佳模型进行测试
model.load_state_dict(torch.load(os.path.join(config["save_dir"], "best_model.pth")))
test(model, val_loader)