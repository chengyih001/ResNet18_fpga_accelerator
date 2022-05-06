// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
/*
#-  (c) Copyright 2011-2020 Xilinx, Inc. All rights reserved.
#-
#-  This file contains confidential and proprietary information
#-  of Xilinx, Inc. and is protected under U.S. and
#-  international copyright and other intellectual property
#-  laws.
#-
#-  DISCLAIMER
#-  This disclaimer is not a license and does not grant any
#-  rights to the materials distributed herewith. Except as
#-  otherwise provided in a valid license issued to you by
#-  Xilinx, and to the maximum extent permitted by applicable
#-  law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
#-  WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
#-  AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
#-  BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
#-  INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
#-  (2) Xilinx shall not be liable (whether in contract or tort,
#-  including negligence, or under any other theory of
#-  liability) for any loss or damage of any kind or nature
#-  related to, arising under or in connection with these
#-  materials, including for any direct, or any indirect,
#-  special, incidental, or consequential loss or damage
#-  (including loss of data, profits, goodwill, or any type of
#-  loss or damage suffered as a result of any action brought
#-  by a third party) even if such damage or loss was
#-  reasonably foreseeable or Xilinx had been advised of the
#-  possibility of the same.
#-
#-  CRITICAL APPLICATIONS
#-  Xilinx products are not designed or intended to be fail-
#-  safe, or for use in any application requiring fail-safe
#-  performance, such as life-support or safety devices or
#-  systems, Class III medical devices, nuclear facilities,
#-  applications related to the deployment of airbags, or any
#-  other applications that could lead to death, personal
#-  injury, or severe property or environmental damage
#-  (individually and collectively, "Critical
#-  Applications"). Customer assumes the sole risk and
#-  liability of any use of Xilinx products in Critical
#-  Applications, subject only to applicable laws and
#-  regulations governing limitations on product liability.
#-
#-  THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
#-  PART OF THIS FILE AT ALL TIMES. 
#- ************************************************************************


   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef X_HLS_STREAM_THREAD_UNSAFE_SIM_H
#define X_HLS_STREAM_THREAD_UNSAFE_SIM_H

/*
 * This file contains a C++ model of hls::stream.
 * It defines C simulation model.
 */
#ifndef __cplusplus

#error C++ is required to include this header file

#else

//////////////////////////////////////////////
// C level simulation models for hls::stream
//////////////////////////////////////////////
#include <queue>
#include <iostream>
#include <typeinfo>
#include <string>
#include <sstream>
#include <unordered_map>
#include <cstring>

#ifndef _MSC_VER
#include <cxxabi.h>
#include <stdlib.h>
#endif

namespace hls {

static int instance_count;
static int max_size;
static void stream_handler() {
  std::cout << "The maximum depth reached by any of the " << instance_count 
    << " hls::stream() instances in the design is " << max_size << std::endl;
}

template<size_t SIZE>
class stream_map {
  protected:
    static std::unordered_map<void*, std::deque<std::array<char, SIZE>>>& deque_map();
    static std::unordered_map<void*, std::string>& name_map();
};

template<size_t SIZE>
std::unordered_map<void*, std::deque<std::array<char, SIZE>>>& stream_map<SIZE>::deque_map() {
  static auto* deque_map = new std::unordered_map<void*, std::deque<std::array<char, SIZE>>>();
  return *deque_map;
};

template<size_t SIZE>
std::unordered_map<void*, std::string>& stream_map<SIZE>::name_map() {
  static auto* name_map = new std::unordered_map<void*, std::string>();
  return *name_map;
};

template<typename __STREAM_T__, int DEPTH=0>
class stream;
template<typename __STREAM_T__>
class stream<__STREAM_T__, 0> : public stream_map<sizeof(__STREAM_T__)>
{
  using stream_map<sizeof(__STREAM_T__)>::deque_map;
  using stream_map<sizeof(__STREAM_T__)>::name_map;
  protected:
    __STREAM_T__ _data;
    
  protected:
    void register_handler() {
      if(instance_count == 0) 
        const int handler = std::atexit(stream_handler);
      instance_count++;
    } 

  public:
    /// Constructors
    // Keep consistent with the synthesis model's constructors
    stream() {
        static unsigned _counter = 1;
        register_handler();
        std::stringstream ss;
        name_map().erase(&_data);
        deque_map().erase(&_data);
#ifndef _MSC_VER
        char* _demangle_name = abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
        if (_demangle_name) {
            
            name_map()[&_data] = _demangle_name;
            free(_demangle_name);
        }
        else {
            name_map()[&_data] = "hls_stream";
        }
#else
        name_map()[&_data] = typeid(*this).name();
#endif

        ss << _counter++;
        name_map()[&_data] += "." + ss.str();
    }

    stream(const char *name) {
    // default constructor,
    // capacity set to predefined maximum
        name_map()[&_data] = name;
        register_handler();
    }

  /// Make copy constructor and assignment operator private
  private:
    stream(const stream< __STREAM_T__ >& chn):
        _data(chn._data) {
        name_map()[&_data] = name_map()[&chn._data];
        deque_map()[&_data] = deque_map()[&chn._data];
    }

    stream& operator = (const stream< __STREAM_T__ >& chn) {
        name_map()[&_data] = name_map()[&chn._data];
        deque_map()[&_data] = deque_map()[&chn._data];
        return *this;
    }

  public:
    /// Overload >> and << operators to implement read() and write()
    void operator >> (__STREAM_T__& rdata) {
        read(rdata);
    }

    void operator << (const __STREAM_T__& wdata) {
        write(wdata);
    }


  public:
    /// Destructor
    /// Check status of the queue
    ~stream() {
      if (!deque_map()[&_data].empty())
      {
          std::cout << "WARNING: Hls::stream '" 
                    << name_map()[&_data]
                    << "' contains leftover data,"
                    << " which may result in RTL simulation hanging."
                    << std::endl;
      }
    }
    bool exist() {
      return deque_map().count(&_data);
    }

    /// Status of the queue
    bool empty() {

        return deque_map()[&_data].empty();
    }    

    bool full() const { return false; }

    /// Blocking read
    void read(__STREAM_T__& head) {
        head = read();
    }

    /// Blocking read
    bool read_dep(__STREAM_T__& head, volatile bool flag) {
        head = read();
        return flag;
    }

    __STREAM_T__ read() {
        __STREAM_T__ elem;
#ifdef HLS_STREAM_WAIT_FOR_DATA_IN_BLOCKING_READ
        while(deque_map()[&_data].empty())
          ;
#endif
        if (deque_map()[&_data].empty()) {
            std::cout << "WARNING: Hls::stream '"
                      << name_map()[&_data] 
                      << "' is read while empty,"
                      << " which may result in RTL simulation hanging."
                      << std::endl;
            elem = __STREAM_T__();
        } else {
            std::array<char, sizeof(__STREAM_T__)> &elem_data = deque_map()[&_data].front();
            memcpy(&elem, elem_data.data(), sizeof(__STREAM_T__));
            deque_map()[&_data].pop_front();
        }
        return elem;
    }

    /// Blocking write
    void write(const __STREAM_T__& tail) { 
        std::array<char, sizeof(__STREAM_T__)> elem_data;
        memcpy(elem_data.data(), &tail, sizeof(__STREAM_T__));
        deque_map()[&_data].push_back(elem_data);
        if(max_size < size()) {
          max_size = size();
        }
    }

    /// Blocking write
    bool write_dep(const __STREAM_T__& tail, volatile bool flag) { 
        std::array<char, sizeof(__STREAM_T__)> elem_data;
        memcpy(elem_data.data(), &tail, sizeof(__STREAM_T__));
        deque_map()[&_data].push_back(elem_data);
        
        if(max_size < size()) {
          max_size = size();
        }
        return flag;
    }

    /// Nonblocking read
    bool read_nb(__STREAM_T__& head) {
        bool is_empty = deque_map()[&_data].empty();
        if (is_empty) {
            head = __STREAM_T__();
        } else {
            __STREAM_T__ elem;
            std::array<char, sizeof(__STREAM_T__)> &elem_data = deque_map()[&_data].front();
            memcpy(&elem, elem_data.data(), sizeof(__STREAM_T__));
            deque_map()[&_data].pop_front();
            head = elem;
        }
        return !is_empty;
    }

    /// Nonblocking write
    bool write_nb(const __STREAM_T__& tail) {
        bool is_full = full();
        write(tail);
        return !is_full;
    }

    /// Fifo size
    size_t size() {
        return deque_map()[&_data].size();
    }
};

template<typename __STREAM_T__, int DEPTH>
class stream : public stream<__STREAM_T__, 0> {
public:
  stream() {}
  stream(const char* name) : stream<__STREAM_T__, 0>(name) {}
};

} // namespace hls

#endif // __cplusplus
#endif  // X_HLS_STREAM_THREAD_UNSAFE_SIM_H


