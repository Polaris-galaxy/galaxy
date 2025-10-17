#include <graphics.h>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// 常量定义
const double PI = acos(-1.0);
const double EPS = 1e-6;

// 点类
class Point {
private:
    double x, y;
    static int count;  // 对象计数器

public:
    // 构造函数
    Point(double x = 0, double y = 0) : x(x), y(y) { count++; }
    Point(const Point& p) : x(p.x), y(p.y) { count++; }

    // 析构函数
    ~Point() { count--; }

    // 访问器
    double getX() const { return x; }
    double getY() const { return y; }
    void setX(double x) { this->x = x; }
    void setY(double y) { this->y = y; }

    // 静态方法：获取对象数量
    static int getCount() { return count; }

    // 面积和周长（点没有面积和周长）
    double area() const { return 0; }
    double perimeter() const { return 0; }

    // 移动
    void move(double dx, double dy) {
        x += dx;
        y += dy;
    }

    // 缩放（以原点为中心）
    void scale(double s) {
        x *= s;
        y *= s;
    }

    // 旋转（以原点为中心）
    void rotate(double angle) {  // 角度制
        double rad = angle * PI / 180.0;
        double cosA = cos(rad);
        double sinA = sin(rad);
        double nx = x * cosA - y * sinA;
        double ny = x * sinA + y * cosA;
        x = nx;
        y = ny;
    }

    // 镜像（关于x轴）
    void mirrorX() {
        y = -y;
    }

    // 镜像（关于y轴）
    void mirrorY() {
        x = -x;
    }

    // 绘制点
    void draw() const {
        putpixel((int)(x + 0.5), (int)(y + 0.5), RED);
    }

    // 输出点信息
    void print() const {
        cout << "Point(" << x << ", " << y << ")";
    }
};

int Point::count = 0;

// 线段类
class Line {
private:
    Point p1, p2;
    static int count;  // 对象计数器

public:
    // 构造函数
    Line(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0) : p1(x1, y1), p2(x2, y2) { count++; }
    Line(const Point& p1, const Point& p2) : p1(p1), p2(p2) { count++; }
    Line(const Line& line) : p1(line.p1), p2(line.p2) { count++; }

    // 析构函数
    ~Line() { count--; }

    // 访问器
    Point getP1() const { return p1; }
    Point getP2() const { return p2; }
    void setP1(const Point& p) { p1 = p; }
    void setP2(const Point& p) { p2 = p; }

    // 静态方法：获取对象数量
    static int getCount() { return count; }

    // 长度（线段没有面积）
    double length() const {
        double dx = p2.getX() - p1.getX();
        double dy = p2.getY() - p1.getY();
        return sqrt(dx * dx + dy * dy);
    }

    // 面积（线段面积为0）
    double area() const { return 0; }

    // 周长（线段周长为其长度）
    double perimeter() const { return length(); }

    // 移动
    void move(double dx, double dy) {
        p1.move(dx, dy);
        p2.move(dx, dy);
    }

    // 缩放（以原点为中心）
    void scale(double s) {
        p1.scale(s);
        p2.scale(s);
    }

    // 旋转（以原点为中心）
    void rotate(double angle) {  // 角度制
        p1.rotate(angle);
        p2.rotate(angle);
    }

    // 镜像（关于x轴）
    void mirrorX() {
        p1.mirrorX();
        p2.mirrorX();
    }

    // 镜像（关于y轴）
    void mirrorY() {
        p1.mirrorY();
        p2.mirrorY();
    }

    // 绘制线段
    void draw(int color = GREEN) const {
        setcolor(color);
        line((int)(p1.getX() + 0.5), (int)(p1.getY() + 0.5),
            (int)(p2.getX() + 0.5), (int)(p2.getY() + 0.5));
    }

    // 输出线段信息
    void print() const {
        cout << "Line: ";
        p1.print();
        cout << " to ";
        p2.print();
        cout << ", Length: " << length() << endl;
    }
};

int Line::count = 0;

// 圆类
class Circle {
private:
    Point center;
    double radius;
    static int count;  // 对象计数器

public:
    // 构造函数
    Circle(double x = 0, double y = 0, double r = 0) : center(x, y), radius(r) {
        if (radius < 0) radius = 0;
        count++;
    }
    Circle(const Point& c, double r = 0) : center(c), radius(r) {
        if (radius < 0) radius = 0;
        count++;
    }
    Circle(const Circle& circle) : center(circle.center), radius(circle.radius) { count++; }

    // 析构函数
    ~Circle() { count--; }

    // 访问器
    Point getCenter() const { return center; }
    double getRadius() const { return radius; }
    void setCenter(const Point& c) { center = c; }
    void setRadius(double r) { radius = (r >= 0) ? r : 0; }

    // 静态方法：获取对象数量
    static int getCount() { return count; }

    // 面积
    double area() const { return PI * radius * radius; }

    // 周长
    double perimeter() const { return 2 * PI * radius; }

    // 移动
    void move(double dx, double dy) {
        center.move(dx, dy);
    }

    // 缩放
    void scale(double s) {
        if (s > 0) {
            radius *= s;
            center.scale(s);
        }
    }

    // 旋转（圆旋转不改变形状）
    void rotate(double angle) {  // 角度制
        center.rotate(angle);
    }

    // 镜像（关于x轴）
    void mirrorX() {
        center.mirrorX();
    }

    // 镜像（关于y轴）
    void mirrorY() {
        center.mirrorY();
    }

    // 绘制圆
    void draw(int color = BLUE) const {
        setcolor(color);
        circle((int)(center.getX() + 0.5), (int)(center.getY() + 0.5),
            (int)(radius + 0.5));
    }

    // 输出圆信息
    void print() const {
        cout << "Circle: Center ";
        center.print();
        cout << ", Radius: " << radius << ", Area: " << area()
            << ", Perimeter: " << perimeter() << endl;
    }
};

int Circle::count = 0;

// 矩形类
class Rectangle {
private:
    Point topLeft;  // 左上角点
    double width, height;
    static int count;  // 对象计数器

public:
    // 构造函数
    Rectangle(double x = 0, double y = 0, double w = 0, double h = 0)
        : topLeft(x, y), width(w), height(h) {
        if (width < 0) width = 0;
        if (height < 0) height = 0;
        count++;
    }
    Rectangle(const Point& tl, double w = 0, double h = 0)
        : topLeft(tl), width(w), height(h) {
        if (width < 0) width = 0;
        if (height < 0) height = 0;
        count++;
    }
    Rectangle(const Rectangle& rect)
        : topLeft(rect.topLeft), width(rect.width), height(rect.height) {
        count++;
    }

    // 析构函数
    ~Rectangle() { count--; }

    // 访问器
    Point getTopLeft() const { return topLeft; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    void setTopLeft(const Point& tl) { topLeft = tl; }
    void setWidth(double w) { width = (w >= 0) ? w : 0; }
    void setHeight(double h) { height = (h >= 0) ? h : 0; }

    // 获取右下角点
    Point getBottomRight() const {
        return Point(topLeft.getX() + width, topLeft.getY() + height);
    }

    // 静态方法：获取对象数量
    static int getCount() { return count; }

    // 面积
    double area() const { return width * height; }

    // 周长
    double perimeter() const { return 2 * (width + height); }

    // 移动
    void move(double dx, double dy) {
        topLeft.move(dx, dy);
    }

    // 缩放（以原点为中心）
    void scale(double s) {
        if (s > 0) {
            topLeft.scale(s);
            width *= s;
            height *= s;
        }
    }

    // 旋转（以原点为中心）
    void rotate(double angle) {  // 角度制
        // 旋转矩形的四个顶点
        Point p1 = topLeft;
        Point p2(topLeft.getX() + width, topLeft.getY());
        Point p3 = getBottomRight();
        Point p4(topLeft.getX(), topLeft.getY() + height);

        p1.rotate(angle);
        p2.rotate(angle);
        p3.rotate(angle);
        p4.rotate(angle);

        // 计算旋转后矩形的边界
        double minX = min(min(p1.getX(), p2.getX()), min(p3.getX(), p4.getX()));
        double maxX = max(max(p1.getX(), p2.getX()), max(p3.getX(), p4.getX()));
        double minY = min(min(p1.getY(), p2.getY()), min(p3.getY(), p4.getY()));
        double maxY = max(max(p1.getY(), p2.getY()), max(p3.getY(), p4.getY()));

        topLeft = Point(minX, minY);
        width = maxX - minX;
        height = maxY - minY;
    }

    // 镜像（关于x轴）
    void mirrorX() {
        topLeft.mirrorX();
        topLeft.setY(topLeft.getY() - height);
    }

    // 镜像（关于y轴）
    void mirrorY() {
        topLeft.mirrorY();
        topLeft.setX(topLeft.getX() - width);
    }

    // 绘制矩形
    void draw(int color = CYAN) const {
        setcolor(color);
        rectangle((int)(topLeft.getX() + 0.5), (int)(topLeft.getY() + 0.5),
            (int)(topLeft.getX() + width + 0.5), (int)(topLeft.getY() + height + 0.5));
    }

    // 输出矩形信息
    void print() const {
        cout << "Rectangle: TopLeft ";
        topLeft.print();
        cout << ", Width: " << width << ", Height: " << height
            << ", Area: " << area() << ", Perimeter: " << perimeter() << endl;
    }
};

int Rectangle::count = 0;

// 三角形类
class Triangle {
private:
    Point p1, p2, p3;
    static int count;  // 对象计数器

public:
    // 构造函数
    Triangle(double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0, double x3 = 0, double y3 = 0)
        : p1(x1, y1), p2(x2, y2), p3(x3, y3) {
        count++;
    }
    Triangle(const Point& a, const Point& b, const Point& c) : p1(a), p2(b), p3(c) { count++; }
    Triangle(const Triangle& tri) : p1(tri.p1), p2(tri.p2), p3(tri.p3) { count++; }

    // 析构函数
    ~Triangle() { count--; }

    // 访问器
    Point getP1() const { return p1; }
    Point getP2() const { return p2; }
    Point getP3() const { return p3; }
    void setP1(const Point& p) { p1 = p; }
    void setP2(const Point& p) { p2 = p; }
    void setP3(const Point& p) { p3 = p; }

    // 静态方法：获取对象数量
    static int getCount() { return count; }

    // 计算三边长
    double length12() const {
        double dx = p2.getX() - p1.getX();
        double dy = p2.getY() - p1.getY();
        return sqrt(dx * dx + dy * dy);
    }
    double length23() const {
        double dx = p3.getX() - p2.getX();
        double dy = p3.getY() - p2.getY();
        return sqrt(dx * dx + dy * dy);
    }
    double length31() const {
        double dx = p1.getX() - p3.getX();
        double dy = p1.getY() - p3.getY();
        return sqrt(dx * dx + dy * dy);
    }

    // 面积（海伦公式）
    double area() const {
        double a = length23();
        double b = length31();
        double c = length12();
        double s = (a + b + c) / 2;
        return sqrt(s * (s - a) * (s - b) * (s - c));
    }

    // 周长
    double perimeter() const {
        return length12() + length23() + length31();
    }

    // 移动
    void move(double dx, double dy) {
        p1.move(dx, dy);
        p2.move(dx, dy);
        p3.move(dx, dy);
    }

    // 缩放（以原点为中心）
    void scale(double s) {
        p1.scale(s);
        p2.scale(s);
        p3.scale(s);
    }

    // 旋转（以原点为中心）
    void rotate(double angle) {  // 角度制
        p1.rotate(angle);
        p2.rotate(angle);
        p3.rotate(angle);
    }

    // 镜像（关于x轴）
    void mirrorX() {
        p1.mirrorX();
        p2.mirrorX();
        p3.mirrorX();
    }

    // 镜像（关于y轴）
    void mirrorY() {
        p1.mirrorY();
        p2.mirrorY();
        p3.mirrorY();
    }

    // 绘制三角形
    void draw(int color = MAGENTA) const {
        setcolor(color);
        line((int)(p1.getX() + 0.5), (int)(p1.getY() + 0.5),
            (int)(p2.getX() + 0.5), (int)(p2.getY() + 0.5));
        line((int)(p2.getX() + 0.5), (int)(p2.getY() + 0.5),
            (int)(p3.getX() + 0.5), (int)(p3.getY() + 0.5));
        line((int)(p3.getX() + 0.5), (int)(p3.getY() + 0.5),
            (int)(p1.getX() + 0.5), (int)(p1.getY() + 0.5));
    }

    // 输出三角形信息
    void print() const {
        cout << "Triangle: ";
        p1.print();
        cout << ", ";
        p2.print();
        cout << ", ";
        p3.print();
        cout << ", Area: " << area() << ", Perimeter: " << perimeter() << endl;
    }
};

int Triangle::count = 0;

// 绘制坐标系
void drawCoordinateSystem(int width, int height) {
    setcolor(LIGHTGRAY);
    // 绘制网格
    for (int x = 0; x < width; x += 50) {
        line(x, 0, x, height);
    }
    for (int y = 0; y < height; y += 50) {
        line(0, y, width, y);
    }

    // 绘制坐标轴
    setcolor(BLACK);
    setlinestyle(SOLID_LINE, 2);
    line(width / 2, 0, width / 2, height);  // Y轴
    line(0, height / 2, width, height / 2); // X轴

    // 绘制原点
    setcolor(RED);
    circle(width / 2, height / 2, 5);
    floodfill(width / 2, height / 2, RED);
}

// 主函数
int main() {
    // 初始化图形窗口（graphics.h方式）
    int width = 1000;
    int height = 800;
    int gdriver = DETECT, gmode;
    initgraph(&gdriver, &gmode, "");  // 初始化图形模式
    setbkcolor(WHITE);
    cleardevice();

    // 创建各种几何图形对象
    Point p1(50, 50);
    Point p2(150, 50);
    Point p3(100, 150);

    Line line1(p1, p2);

    Circle circle1(200, 100, 50);

    Rectangle rect1(300, 50, 100, 80);

    Triangle tri1(p1, p2, p3);

    // 显示各类对象数量
    cout << "对象数量统计：" << endl;
    cout << "点(Point)对象数量：" << Point::getCount() << endl;
    cout << "线段(Line)对象数量：" << Line::getCount() << endl;
    cout << "圆(Circle)对象数量：" << Circle::getCount() << endl;
    cout << "矩形(Rectangle)对象数量：" << Rectangle::getCount() << endl;
    cout << "三角形(Triangle)对象数量：" << Triangle::getCount() << endl << endl;

    // 显示各对象信息
    cout << "对象信息：" << endl;
    line1.print();
    circle1.print();
    rect1.print();
    tri1.print();
    cout << endl;

    // 绘制原始图形（左侧）
    drawCoordinateSystem(width, height);
    setlinestyle(SOLID_LINE, 1);

    // 转换到左侧区域绘制
    int offsetX = -width / 4;

    // 保存原始对象用于变换
    Line line2 = line1;
    Circle circle2 = circle1;
    Rectangle rect2 = rect1;
    Triangle tri2 = tri1;

    // 移动原始对象到左侧区域
    line1.move(offsetX, 0);
    circle1.move(offsetX, 0);
    rect1.move(offsetX, 0);
    tri1.move(offsetX, 0);

    // 绘制原始图形
    setcolor(BLACK);
    outtextxy(50, 30, "原始图形");
    line1.draw(GREEN);
    circle1.draw(BLUE);
    rect1.draw(CYAN);
    tri1.draw(MAGENTA);

    // 右侧绘制变换后的图形
    // 1. 移动变换
    Line lineMove = line2;
    lineMove.move(width / 4, 0);
    lineMove.draw(YELLOW);

    // 2. 缩放变换
    Circle circleScale = circle2;
    circleScale.scale(0.7);
    circleScale.move(width / 4, 0);
    circleScale.draw(YELLOW);

    // 3. 旋转变换
    Rectangle rectRotate = rect2;
    rectRotate.rotate(45);
    rectRotate.move(width / 4, 0);
    rectRotate.draw(YELLOW);

    // 4. 镜像变换
    Triangle triMirror = tri2;
    triMirror.mirrorY();
    triMirror.move(width / 4, 0);
    triMirror.draw(YELLOW);

    // 显示变换说明
    setcolor(BLACK);
    outtextxy(width / 2 + 50, 30, "变换后图形");
    outtextxy(width / 2 + 50, 100, "线段：移动");
    outtextxy(width / 2 + 50, 200, "圆：缩放(0.7倍)");
    outtextxy(width / 2 + 50, 300, "矩形：旋转(45度)");
    outtextxy(width / 2 + 50, 400, "三角形：Y轴镜像");

    // 等待用户操作
    cout << "按任意键退出..." << endl;
    getchar();
    closegraph();  // 关闭图形模式

    return 0;
}