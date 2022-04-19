/*
 *         Copyright (C) <2020-2022>  DUHAMEL Erwan
 *
 *                      BSD-2 License
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *      * Redistributions of source code must retain the above copyright notice,
 *        this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright notice,
 *        this list of conditions and the following disclaimer in the documentation
 *        and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef STE_timer_HPP
#define STE_timer_HPP

#include <atomic>

#include <chrono>

#include <mutex>

#include <ostream>

#include <thread>

#include <type_traits>

namespace ste
{

/**
                                ste::timer

    @short This class provides a thread-based C++ timer that calls a user-defined function.

    @details
    Features:
        • Can call any function.
        • Single shot or loop execution editable at runtime.
        • Function change while running (provided the two decltype()s are identical).

     @copyright     Copyright (C) <2020-2022>  DUHAMEL Erwan

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

template<typename function_t,
         typename delay_t,
         typename interval_t,
         typename            = std::enable_if_t<std::is_same_v<delay_t, std::chrono::nanoseconds>  ||
                                                std::is_same_v<delay_t, std::chrono::microseconds> ||
                                                std::is_same_v<delay_t, std::chrono::milliseconds> ||
                                                std::is_same_v<delay_t, std::chrono::seconds>      ||
                                                std::is_same_v<delay_t, std::chrono::minutes>      ||
                                                std::is_same_v<delay_t, std::chrono::hours>
                                                >,
         typename            = std::enable_if_t<std::is_same_v<interval_t, std::chrono::nanoseconds>  ||
                                                std::is_same_v<interval_t, std::chrono::microseconds> ||
                                                std::is_same_v<interval_t, std::chrono::milliseconds> ||
                                                std::is_same_v<interval_t, std::chrono::seconds>      ||
                                                std::is_same_v<interval_t, std::chrono::minutes>      ||
                                                std::is_same_v<interval_t, std::chrono::hours>
                                                >
         >
class timer
{
    static_assert (std::is_invocable<function_t>::value, "ste::timer can only be initialized with an invocable template parameter." );

private:

    /*********************************************************************/
    /*                             Attributes                            */
    /*********************************************************************/

    /// Current state of the timer.
    std::atomic<bool> _stopped;

    /// Timer mode (continuous calls to the function or single call after the delay)
    std::atomic<bool> _single_shot;

    /// Interval between two function calls.
    std::atomic<interval_t> _interval;

    /// Delay before the timer stops.
    std::atomic<delay_t> _delay;

    /// Function to call
    function_t _function;

    /// Mutex used to protect read / write access when changing function.
    std::mutex _write_mutex;

public:

    /*********************************************************************/
    /*             Construction / destruction / assignment               */
    /*********************************************************************/

    /**
     *  @brief Constructor.
     *  @param function Function to call.
     *  @param interval Duration between calls.
     *  @param delay (optional) Duration to wait before the timer starts its call loop. Default is {}.
     *  @param single_shot (optional) Indicates if timer must execute its function
     *                                until lifetime expires or stop() is called.
     *                                Default is true.
     *  @param start (optional) Indicates if the timer must be started immediately. Default is false.
     */
    inline timer(function_t function,
                 const interval_t interval,
                 const delay_t delay    = {},
                 const bool single_shot = true,
                 const bool start       = false)
        :
          _stopped(!start),               //ste::timer is stopped when declared.
          _single_shot(single_shot),
          _delay(delay),
          _interval(interval),
          _function(function)
    {
        if(start)
        {
            this->start();
        }
    }

    /**
     *  @brief Constructor.
     *  @param function Function to call.
     *  @param interval Duration between calls.
     *  @param delay (optional) Duration to wait before the timer starts its call loop. Default is 0.
     *  @param single_shot (optional) Indicates if timer must execute its function
     *                                until lifetime expires or stop() is called.
     *                                Default is true.
     *  @param start (optional) Indicates if the timer must be started immediately. Default is false.
     */
    inline timer(function_t function,
                 const std::uint64_t interval,
                 const std::uint64_t delay    = 0,
                 const bool single_shot       = true,
                 const bool start             = false)
        : timer(function,
                static_cast<interval_t>(interval),
                static_cast<delay_t>(delay),
                single_shot,
                start)
    {}

    timer(const timer&)            = delete;
    timer(timer&&)                 = delete;
    timer& operator=(const timer&) = delete;
    timer& operator=(timer&&)      = delete;

    /*********************************************************************/
    /*                         Timer management                          */
    /*********************************************************************/

    /// Starts the timer. Nothing happens if the timer is already started.
    inline void start()
    {
        if(_stopped)
        {
            const auto on_run = [this]() -> void
            {
                const auto delay = this->delay();

                if(delay.count() != 0)
                {
                    std::this_thread::sleep_for(_delay.load());
                }

                do
                {
                    const interval_t interval = this->interval();

                    if(_stopped)
                    {
                        return;
                    }

                    if(interval.count() != 0)
                    {
                        std::this_thread::sleep_for(interval);
                    }

                    if(_stopped)
                    {
                        return;
                    }

                    _function();
                }
                while(!_single_shot); //Also return if set to single shot mode while in the loop

                _stopped = true;
            };

            std::thread t(on_run);
            t.detach();

            _stopped = false;
        }
    }

    /// Stops the timer.
    inline void stop()
    {
        _stopped = true;
    }

    /*********************************************************************/
    /*                       Accessors / Mutators                        */
    /*********************************************************************/

    /// Returns 'true' if the timer is running, 'false' otherwise.
    inline bool running() const
    {
        return !stopped();
    }

    /// Returns 'true' if the timer is stopped, 'false' otherwise.
    inline bool stopped() const
    {
        return _stopped.load();
    }

    /// Sets the timer to single shot or continugous mode.
    inline void set_single_shot(const bool single_shot)
    {
        _single_shot = single_shot;
    }

    /// Returns 'true' if the timer is single shot (
    inline bool single_shot() const
    {
        return _single_shot;
    }

    /// Sets timer delay (duration before first call).
    inline void set_delay(const delay_t delay)
    {
        _delay = delay;
    }

    /// Sets timer delay (duration before first call).
    inline void set_delay(const std::uint64_t interval)
    {
        set_delay(static_cast<delay_t>(interval));
    }

    /// Returns the duration to wait before starting the timer loop.
    inline delay_t delay() const
    {
        return _delay.load();
    }

    /// Sets timer interval (duration between calls).
    inline void set_interval(const interval_t interval)
    {
        _interval = interval;
    }

    /// Sets timer interval (duration between calls).
    inline void set_interval(const std::uint64_t interval)
    {
        set_interval(static_cast<interval_t>(interval));
    }

    /// Returns the duration between two function calls.
    inline interval_t interval() const
    {
        return _interval.load();
    }

    /**
     *  Sets the function called by the timer.
     *  @note std::lock_guard protects access from timer thread.
     */
    inline void set_function(function_t func)
    {
        std::lock_guard lock (_write_mutex);
        _function = func;
    }

    /// Returns a copy of the function called by the timer.
    inline function_t function() const
    {
        return _function;
    }

    /*********************************************************************/
    /*                            Operators                              */
    /*********************************************************************/

    inline friend std::ostream& operator<<(std::ostream& out, const timer& t)
    {
        // TODO: replace by templated lambda when C++20 support becomes wider.

        constexpr auto delay_unit = []() constexpr -> std::string_view
        {
            if constexpr(std::is_same_v<delay_t, std::chrono::nanoseconds>)
            {
                return "ns";
            }
            else if constexpr(std::is_same_v<delay_t, std::chrono::microseconds>)
            {
                return "us";
            }
            else if constexpr(std::is_same_v<delay_t, std::chrono::milliseconds>)
            {
                return "ms";
            }
            else if constexpr(std::is_same_v<delay_t, std::chrono::seconds>)
            {
                return "s";
            }
            else if constexpr(std::is_same_v<delay_t, std::chrono::minutes>)
            {
                return "min";
            }
            else if constexpr(std::is_same_v<delay_t, std::chrono::hours>)
            {
                return "h";
            }
            else
            {
                return "?";
            }
        }();

        constexpr auto interval_unit = []() constexpr -> std::string_view
        {
            if constexpr(std::is_same_v<interval_t, std::chrono::nanoseconds>)
            {
                return "ns";
            }
            else if constexpr(std::is_same_v<interval_t, std::chrono::microseconds>)
            {
                return "us";
            }
            else if constexpr(std::is_same_v<interval_t, std::chrono::milliseconds>)
            {
                return "ms";
            }
            else if constexpr(std::is_same_v<interval_t, std::chrono::seconds>)
            {
                return "s";
            }
            else if constexpr(std::is_same_v<interval_t, std::chrono::minutes>)
            {
                return "min";
            }
            else if constexpr(std::is_same_v<interval_t, std::chrono::hours>)
            {
                return "h";
            }
            else
            {
                return "?";
            }
        }();

        return out << "ste::timer:\n"
                   << "    Interval: " << t.interval().count() << interval_unit << "\n"
                   << "    Delay: " << t.delay().count() << delay_unit << "\n"
                   << "    Single-shot: " << t.single_shot();
    }
};

/// Partial specialisation of ste::timer. Durations are given in nanoseconds.
template<typename function_t>
using ns_timer = timer<function_t, std::chrono::nanoseconds, std::chrono::nanoseconds>;

/// Partial specialisation of ste::timer. Durations are given in microseconds.
template<typename function_t>
using us_timer = timer<function_t, std::chrono::microseconds, std::chrono::microseconds>;

/// Partial specialisation of ste::timer. Durations are given in milliseconds.
template<typename function_t>
using ms_timer = timer<function_t, std::chrono::milliseconds, std::chrono::milliseconds>;

/// Partial specialisation of ste::timer. Durations are given in seconds.
template<typename function_t>
using sec_timer = timer<function_t, std::chrono::seconds, std::chrono::seconds>;

/// Partial specialisation of ste::timer. Durations are given in minutes.
template<typename function_t>
using min_timer = timer<function_t, std::chrono::seconds, std::chrono::seconds>;

/// Partial specialisation of ste::timer. Durations are given in hours.
template<typename function_t>
using hour_timer = timer<function_t, std::chrono::hours, std::chrono::hours>;

} //namespace ste
#endif //STE_timer_HPP
