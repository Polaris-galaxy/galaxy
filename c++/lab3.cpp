#include <iostream>
#include <new>
#include <cstdlib>

using namespace std;

// 定义一个较大的结构体用于测试
struct LargeStruct {
    int data[1000000]; 
};

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        cerr << "用法: " << argv[0] << " <times>" << endl;
        return 1;
    }
    int times = atoi(argv[1]);  // 将命令行参数转换为整数

    try {
        // 外层 try 块，用于捕获内层未处理的异常
        for (int i = 1; i <= times; ++i) {
            try {
                // 内层 try 块：尝试分配内存
                LargeStruct *p = new(nothrow) LargeStruct;
                if (p == nullptr) {
                    // 内存分配失败，抛出异常
                    throw "Memory allocation failed";
                }
                // 分配成功，输出提示并释放内存
                cout << "第 " << i << " 次: 内存分配成功" << endl;
                delete p;
            } catch (const char* msg) {
                // 捕获分配失败时抛出的异常，输出警告信息并继续下一次循环
                cerr << "警告: " << msg << " (在第 " << i << " 次尝试分配时失败)" << endl;
                // 跳过本次并继续循环
            }
        }
    } catch (const exception& e) {
        // 外层 catch 块：捕获其他可能的异常
        cerr << "外层异常捕获: " << e.what() << endl;
    } catch (...) {
        cerr << "外层捕获到未知异常" << endl;
    }

    return 0;
}
