/**
                                ste::Timer

    @short This is an example file for this class.

     @copyright     Copyright (C) <2020-2021>  DUHAMEL Erwan

                        BSD-2 License

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

        * Redistributions of source code must retain the above copyright notice,
          this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright notice,
          this list of conditions and the following disclaimer in the documentation
          and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

     @author DUHAMEL Erwan (erwanduhamel@outlook.com)
*/

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
