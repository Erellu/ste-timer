#include "timer.hpp" //Do not forget to put the correct path to the header here.

#include <functional>

#include <iostream>


int main(){


    std::function<void(void)> a_fun = []{std::cout << "A" << std::endl;};
    std::function<void(void)> b_fun = []{std::cout << "B" << std::endl;};

    ste::Timer t(a_fun , std::chrono::milliseconds(1000) , false); //Declare the timer to call 'a_fun' each 1000 milliseconds indefinitely.

    t.start(); //Start the timer

    std::this_thread::sleep_for(std::chrono::milliseconds(2100)); //Wait for 2100ms

    t.function() = b_fun; //Change the function called by 't'

    std::this_thread::sleep_for(std::chrono::milliseconds(2100)); //Wait for 2100ms

    return 0;
}