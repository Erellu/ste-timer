/*
                        ste::timer example 1

                 This example demonstrates how to
                 call change the function called by a ste::timer
                 from another ste::timer.

     Copyright (C) <2020-2022>  DUHAMEL Erwan (erwanduhamel@outlook.com)

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
*/


#include "../../include/timer.hpp"

#include <chrono>

#include <functional>

#include <iostream>

int main()
{
    const std::function<void(void)> f1 = []()
    {
        std::cout << "f1" << std::endl;
    };

    ste::ms_timer<std::function<void(void)>> t1(f1, 500, 0, false);

    const std::function<void(void)> f2 = []()
    {
        std::cout << "f2" << std::endl;
    };

    const std::function f3 = [f1, f2, &t1]()
    {
        static bool b = false;

        t1.set_function(!b ? f2 : f1);
        b = !b;
    };

    ste::ms_timer<std::function<void(void)>> t2(f3, 1000, 0, false);

    {
        std::cout << t1 << std::endl;
        t1.start();

        std::cout << t2 << std::endl;
        t2.start();

        std::this_thread::sleep_for(t1.interval() * 10);
    }

    return 0;
}
