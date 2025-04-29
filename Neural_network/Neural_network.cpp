#include<iostream>
#include<string>
#include<cstdlib>

using namespace std;

class Math{
    private:
        double m_number_one;
        double m_number_two;
        char m_operator;

    public:
        Math(double number_one, double number_two, char operator_) : m_number_one(number_one), m_number_two(number_two), m_operator(operator_) {};

    
};

int main(){
    cout << "Hello world!'" << endl;

    return 0;
}

