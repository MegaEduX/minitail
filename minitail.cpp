//
//  minitail.cpp
//  minitail
//
//  Copyright (c) 2015, Eduardo Almeida
//  All rights reserved.
//
//  Redistribution and use in source and binary forms,
//  with or without modification, are permitted provided
//  that the following conditions are met:
//
//  1. Redistributions of source code must retain the above
//  copyright notice, this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following
//  disclaimer in the documentation and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its
//  contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#include "minitail.h"

#include <fstream>
#include <future>
#include <thread>

void mt(std::string path, std::function<void (std::string)> delegate, bool *stop);
void mt_async(std::string path, std::function<void (std::string)> delegate, bool *stop);

minitail::minitail(std::string path, std::function<void (std::string)> delegate) {
    _path = path;
    _delegate = delegate;
    
    _running = false;
    
    _stopController = new bool;
}

void minitail::start() {
    if (_running)
        return;
    
    _running = true;
    
    *_stopController = false;
    
    mt_async(_path, _delegate, _stopController);
}

void minitail::stop() {
    if (!_running)
        return;
    
    _running = false;
    
    *_stopController = true;
}

void mt(std::string path, std::function<void (std::string)> delegate, bool *stop) {
    //  Adapted from http://stackoverflow.com/questions/11757304/how-to-read-a-growing-text-file-in-c
    
    std::ifstream ifs(path, std::ios::ate);
    
    if (!ifs.is_open()) {
        std::cerr << "ERROR opening file: " << path << '\n';
        
        return;
    }
    
    std::ios::streampos gpos = ifs.tellg();
    
    std::string line;
    
    while (!(* stop)) {
        if (!std::getline(ifs, line) || ifs.eof()) {
            ifs.clear();
            ifs.seekg(gpos);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            continue;
        }
        
        gpos = ifs.tellg();
        
        delegate(line);
    }
    
    std::cout << "Bye-bye!" << std::endl;
}

void mt_async(std::string path, std::function<void (std::string)> delegate, bool *stop) {
    std::thread t(mt, path, delegate, stop);
    
    t.detach();
}
