#include "timer.hpp" //Do not forget to put the correct path to the header here.

#include <chrono>

#include <functional>

#include <iostream>


int main(){


    int a = 0;

    std::function<void(void)> a_fun = [&a]{std::cout << ++a << std::endl;};
    std::function<void(void)> b_fun = [&a]{std::cout << (a+=2) << std::endl;};

    ste::Timer t(a_fun , std::chrono::milliseconds(1000) , false);
    t.start();


    std::this_thread::sleep_for(std::chrono::milliseconds(2100));

    t.function() = b_fun;

    std::this_thread::sleep_for(std::chrono::milliseconds(2100));



    return 0;
}
