#include <iostream>
#include <cmath>
#define M_PI 3.14159265358979323846 // 定义圆周率常量
using namespace std;

// 抽象基类（虚基类）：Shape
class Shape {
protected:
    string name; // 图形名称，保护成员供派生类访问
public:
    // 构造函数
    Shape(string n) : name(n) {}
    // 析构函数（虚析构，确保派生类对象正确释放）
    virtual ~Shape() {}
    // 纯虚函数（强制派生类重写，体现多态）
    virtual double getArea() const = 0;   // 计算面积
    virtual double getPerimeter() const = 0; // 计算周长
    virtual void draw() const = 0;        // 绘制图形（模拟）

    void get_name() const {
        cout << "图形名称：" << name << endl;
    }
};

// 派生类：圆形（公有继承Shape）
class Circle : public Shape {
private:
    double radius; // 半径（私有成员，外部不可直接访问）
public:
    // 构造函数
    Circle(double r) : Shape("圆形"), radius(r) {}
    // 重写纯虚函数
    double getArea() const override {
        return M_PI * radius * radius;
    }
    double getPerimeter() const override {
        return 2 * M_PI * radius;
    }
    void draw() const override {
        cout << "绘制" << name << "：半径=" << radius << endl;
    }
};

// 派生类：正三角形（公有继承Shape）
class EquilateralTriangle : public Shape {
private:
    double side; // 边长
public:
    EquilateralTriangle(double s) : Shape("正三角形"), side(s) {}
    double getArea() const override {
        return (sqrt(3) / 4) * side * side;
    }
    double getPerimeter() const override {
        return 3 * side;
    }
    void draw() const override {
        cout << "绘制" << name << "：边长=" << side << endl;
    }
};

// 派生类：平行四边形（公有继承Shape）
class Parallelogram : public Shape {
protected:
    double base;   // 底边长（保护成员，供子类访问）
    double height; // 高
    double side;   // 侧边长度
public:
    Parallelogram(double b, double h, double s) : Shape("平行四边形"), base(b), height(h), side(s) {}
    double getArea() const override {
        return base * height;
    }
    double getPerimeter() const override {
        return 2 * (base + side);
    }
    void draw() const override {
        cout << "绘制" << name << "：底=" << base << "，高=" << height << "，侧边=" << side << endl;
    }
};

// 派生类：正方形（公有继承Parallelogram，体现特殊与一般关系）
class Square : public Parallelogram {
public:
    // 构造函数：正方形的底=高=边长，侧边=边长
    Square(double s) : Parallelogram(s, s, s) {
        name = "正方形"; // 重写图形名称
    }
    // 重写面积计算（可复用父类，此处显式写出便于理解）
    double getArea() const override {
        return base * base;
    }
    // 重写绘制方法
    void draw() const override {
        cout << "绘制" << name << "：边长=" << base << endl;
    }
};

// 派生类：正六边形（公有继承Shape）
class RegularHexagon : public Shape {
private:
    double side; // 边长
public:
    RegularHexagon(double s) : Shape("正六边形"), side(s) {}
    double getArea() const override {
        return (3 * sqrt(3) / 2) * side * side;
    }
    double getPerimeter() const override {
        return 6 * side;
    }
    void draw() const override {
        cout << "绘制" << name << "：边长=" << side << endl;
    }
};

// 主函数：测试多态性（使用抽象基类指针）
int main() {
    // 创建抽象基类指针数组，指向不同派生类对象
    Shape* shapes[] = {
        new Circle(5),          // 圆形，半径5
        new EquilateralTriangle(4), // 正三角形，边长4
        new Parallelogram(6, 3, 4), // 平行四边形，底6、高3、侧边4
        new Square(5),          // 正方形，边长5
        new RegularHexagon(3)   // 正六边形，边长3
    };

    // 遍历数组，调用成员函数（体现多态：同一接口，不同实现）
    for (int i = 0; i < 5; i++) {
        shapes[i]->get_name();
        cout << "面积：" << shapes[i]->getArea() << endl;
        cout << "周长：" << shapes[i]->getPerimeter() << endl;
        shapes[i]->draw();
        delete shapes[i]; // 释放对象（虚析构确保正确调用派生类析构）
    }

    return 0;
}