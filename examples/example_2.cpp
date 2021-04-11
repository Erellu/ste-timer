#include "timer.hpp" //Do not forget to put the correct path to the header here.

#include <chrono>

#include <functional>

#include <iostream>


class Foo{

public:
    Foo() : _a(0){}

    void bar(){
        std::cout << "Foo::bar " << ++_a << std::endl;
    }

    void zog(){
        std::cout << "Foo::zog" << std::endl;
    }

private:

    int _a;

};

/*********************************************/

int main(){

    Foo f;

    std::function<void(void)> a_fun = [&f]{f.bar();};
    std::function<void(void)> b_fun = [&f]{f.zog();};

    ste::Timer t(a_fun , std::chrono::milliseconds(1000) , false);
    t.start();


    std::this_thread::sleep_for(std::chrono::milliseconds(2100));

    t.function() = b_fun;

    std::this_thread::sleep_for(std::chrono::milliseconds(2100));



    return 0;
}
