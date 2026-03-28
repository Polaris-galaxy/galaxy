// 简单的学生信息管理系统
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib> // 用于 system("pause") 和 system("cls")

using namespace std;

// 定义学生信息结构体
struct Student {
    string name;    // 姓名
    string id;      // 学号
    string gender;  // 性别
    string major;   // 专业
};

// 定义链表节点结构体
struct Node {
    Student data;
    Node* next;
};

// 定义学生管理系统类
class StudentManager {
private:
    Node* head; // 链表头指针

    // 辅助函数：释放链表内存
    void clearList() {
        Node* current = head;
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = NULL;
    }

    // 辅助函数：根据学号检查是否存在
    bool isIdExist(const string& id) {
        Node* p = head;
        while (p != NULL) {
            if (p->data.id == id) return true;
            p = p->next;
        }
        return false;
    }

public:
    // 构造函数
    StudentManager() {
        head = NULL;
    }

    // 析构函数
    ~StudentManager() {
        clearList();
    }

    // (1) & (2) 添加功能：添加学生信息
    void addStudent() {
        Student newStu;
        cout << "\n--- 添加新学生 ---" << endl;
        
        while (true) {
            cout << "请输入学号 (必填): ";
            cin >> newStu.id;
            if (isIdExist(newStu.id)) {
                cout << "错误：该学号已存在，请重新输入！" << endl;
            } else {
                break;
            }
        }

        cout << "请输入姓名 (必填): ";
        cin >> newStu.name;
        cout << "请输入性别: ";
        cin >> newStu.gender;
        cout << "请输入专业: ";
        cin >> newStu.major;

        // 创建新节点
        Node* newNode = new Node;
        newNode->data = newStu;
        newNode->next = NULL;

        // 追加到链表末尾
        if (head == NULL) {
            head = newNode;
        } else {
            Node* p = head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = newNode;
        }
        cout << ">> 添加成功！" << endl;
    }

    // (3) 删除功能：按姓名删除 (处理重名)
    void deleteByName() {
        if (head == NULL) {
            cout << ">> 记录为空，无法删除。" << endl;
            return;
        }

        string targetName;
        cout << "\n请输入要删除的学生姓名: ";
        cin >> targetName;

        // 第一步：统计重名数量并记录
        int count = 0;
        Node* p = head;
        while (p != NULL) {
            if (p->data.name == targetName) {
                count++;
            }
            p = p->next;
        }

        if (count == 0) {
            cout << ">> 未找到姓名为 \"" << targetName << "\" 的学生。" << endl;
            return;
        }

        string targetId; // 最终确定要删除的学号

        // 如果有重名，列出所有同名学生供选择
        if (count > 1) {
            cout << ">> 发现 " << count << " 位同名学生，请确认要删除哪一位：" << endl;
            printHeader();
            p = head;
            while (p != NULL) {
                if (p->data.name == targetName) {
                    printRow(p->data);
                }
                p = p->next;
            }
            cout << "请输入要删除的学生的【学号】以确认: ";
            cin >> targetId;
        } else {
            // 只有一个，直接获取其ID用于删除逻辑
            p = head;
            while (p != NULL) {
                if (p->data.name == targetName) {
                    targetId = p->data.id;
                    break;
                }
                p = p->next;
            }
        }

        // 执行删除操作 (根据确定的ID删除)
        Node* current = head;
        Node* prev = NULL;
        bool deleted = false;

        while (current != NULL) {
            if (current->data.id == targetId && current->data.name == targetName) {
                // 找到了
                if (prev == NULL) {
                    // 删除头节点
                    head = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                deleted = true;
                cout << ">> 删除成功！" << endl;
                break;
            }
            prev = current;
            current = current->next;
        }

        if (!deleted) {
            cout << ">> 删除失败，未找到匹配的学号/姓名组合。" << endl;
        }
    }

    // (4) 查找功能
    void findStudent() {
        if (head == NULL) {
            cout << ">> 记录为空。" << endl;
            return;
        }

        int choice;
        cout << "\n1. 按姓名查找\n2. 按学号查找\n请选择: ";
        cin >> choice;

        if (choice == 1) {
            string name;
            cout << "请输入姓名: ";
            cin >> name;
            bool found = false;
            Node* p = head;
            int matches = 0;
            
            while (p != NULL) {
                if (p->data.name == name) {
                    if (!found) {
                        printHeader();
                        found = true;
                    }
                    printRow(p->data);
                    matches++;
                }
                p = p->next;
            }
            if (!found) cout << ">> 未找到该姓名的学生。" << endl;
            else if (matches > 1) cout << ">> 共找到 " << matches << " 条重名记录。" << endl;

        } else if (choice == 2) {
            string id;
            cout << "请输入学号: ";
            cin >> id;
            Node* p = head;
            while (p != NULL) {
                if (p->data.id == id) {
                    printHeader();
                    printRow(p->data);
                    return;
                }
                p = p->next;
            }
            cout << ">> 未找到该学号的学生。" << endl;
        } else {
            cout << ">> 输入无效。" << endl;
        }
    }

    // (5) 显示功能：显示所有记录
    void showAll() {
        if (head == NULL) {
            cout << ">> 当前没有学生记录。" << endl;
            return;
        }
        printHeader();
        Node* p = head;
        while (p != NULL) {
            printRow(p->data);
            p = p->next;
        }
    }

    // (6) 保存数据：排序并保存为 Excel 格式
    void saveToFile() {
        if (head == NULL) {
            cout << ">> 没有数据可保存。" << endl;
            return;
        }

        // 先按学号排序 (冒泡排序算法应用于链表数据)
        sortListById();

        string filename;
        cout << "请输入保存文件名 (例如 data.csv): ";
        cin >> filename;

        ofstream outFile(filename.c_str());
        if (!outFile) {
            cout << ">> 文件打开失败！" << endl;
            return;
        }

        // 写入Excel表头 (CSV格式)
        outFile << "姓名,学号,性别,专业" << endl;

        Node* p = head;
        while (p != NULL) {
            outFile << p->data.name << "," 
                    << p->data.id << "," 
                    << p->data.gender << "," 
                    << p->data.major << endl;
            p = p->next;
        }

        outFile.close();
        cout << ">> 数据已保存至 " << filename << " (已按学号排序)" << endl;
    }

    // (7) 加载数据
    void loadFromFile() {
        string filename;
        cout << "请输入要加载的文件名: ";
        cin >> filename;

        ifstream inFile(filename.c_str());
        if (!inFile) {
            cout << ">> 找不到文件或无法打开！" << endl;
            return;
        }

        // 清空当前链表，防止重复追加
        clearList();

        string line;
        // 跳过第一行表头
        getline(inFile, line);

        int count = 0;
        while (getline(inFile, line)) {
            if (line.empty()) continue;

            Student stu;
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                stu.name = line.substr(0, pos1);
                stu.id = line.substr(pos1 + 1, pos2 - pos1 - 1);
                stu.gender = line.substr(pos2 + 1, pos3 - pos2 - 1);
                stu.major = line.substr(pos3 + 1);

                // 添加到链表
                Node* newNode = new Node;
                newNode->data = stu;
                newNode->next = NULL;

                if (head == NULL) {
                    head = newNode;
                } else {
                    Node* p = head;
                    while (p->next != NULL) p = p->next;
                    p->next = newNode;
                }
                count++;
            }
        }
        inFile.close();
        cout << ">> 成功加载 " << count << " 条记录。" << endl;
    }

private:
    // 辅助函数：打印表头
    void printHeader() {
        cout << setiosflags(ios::left) 
             << setw(12) << "姓名" 
             << setw(15) << "学号" 
             << setw(8) << "性别" 
             << setw(20) << "专业" << endl;
        cout << string(55, '-') << endl;
    }

    // 辅助函数：打印一行数据
    void printRow(const Student& s) {
        cout << setiosflags(ios::left) 
             << setw(12) << s.name 
             << setw(15) << s.id 
             << setw(8) << s.gender 
             << setw(20) << s.major << endl;
    }

    // 辅助函数：链表冒泡排序 (交换数据域)
    void sortListById() {
        if (head == NULL || head->next == NULL) return;

        bool swapped;
        Node* ptr1;
        Node* lptr = NULL;

        do {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr) {
                // 按学号从小到大排序
                // 简单的字符串比较，如果需要数值比较可用 atol 等转换
                if (ptr1->data.id > ptr1->next->data.id) { 
                    Student temp = ptr1->data;
                    ptr1->data = ptr1->next->data;
                    ptr1->next->data = temp;
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }
};

// 菜单显示
void showMenu() {
    cout << "\n===============================" << endl;
    cout << "   简单的学生信息管理系统" << endl;
    cout << "===============================" << endl;
    cout << "  1. 添加学生记录" << endl;
    cout << "  2. 删除学生记录" << endl;
    cout << "  3. 查找学生记录" << endl;
    cout << "  4. 显示所有记录" << endl;
    cout << "  5. 保存数据 (Excel格式)" << endl;
    cout << "  6. 加载数据文件" << endl;
    cout << "  0. 退出程序" << endl;
    cout << "===============================" << endl;
    cout << "请选择操作 (0-6): ";
}

int main() {
    StudentManager sys;
    int choice;

    while (true) {
        showMenu();
        cin >> choice;

        // 简单的输入错误处理
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << ">> 输入无效，请输入数字！" << endl;
            continue;
        }

        switch (choice) {
            case 1:
                sys.addStudent();
                break;
            case 2:
                sys.deleteByName();
                break;
            case 3:
                sys.findStudent();
                break;
            case 4:
                sys.showAll();
                break;
            case 5:
                sys.saveToFile();
                break;
            case 6:
                sys.loadFromFile();
                break;
            case 0:
                cout << ">> 感谢使用，再见！" << endl;
                return 0;
            default:
                cout << ">> 无效的选项，请重新选择。" << endl;
        }
        
        // 暂停以便用户查看结果 (Windows特有，如果是Linux可用 cin.get())
        // system("pause"); 
        // system("cls"); 
    }
    return 0;
}