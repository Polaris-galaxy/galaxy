#include<iostream>

using namespace std;

class cube {
    private:
    double length;
    double width;
    double height;

    public:
    void cube_get();
    void cube_side();
};

int main(){
    cube c[3];
    for (int i = 0; i < 3; i++){
        c[i].cube_get();
    }

    for(int i = 0; i < 3; i++){
        c[i].cube_side();
    }
}

void cube::cube_get(){
    cout << "请输入长宽高：" << endl;
    cin >> length >> width >> height;
}

void cube::cube_side(){
    double side;
    side = length * width * height;
    cout << "体积为：" << side << endl;
}