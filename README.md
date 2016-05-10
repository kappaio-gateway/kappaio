# kappaio

This is the main package that performs the core tasks of the web-based ZigBee gateway. 

For a quick try on the Linux ZigBee gateway with pre-built Raspberry Pi images, go to [https://kappa.io/quickstart](https://kappa.io/quickstart).

kappaio is written mostly in C/C++ with plans to interface with languages including but not limited to php, javascript, lua, go, depending on the user-base.

Supported hardware:
- ZigBee Module: 
 - TI CC2531 USB dongle with Z-Stack 2.5.1a; for firmware images, go to [https://kappa.io/download#cc2531-dongle](https://kappa.io/download#cc2531-dongle)

##Build and install
You need cmake for the compilation:
```
sudo apt-get install cmake
```
Now build the package
```
git clone https://github.com/kappaIO-Dev/kappaio.git
cd kappaio/cmake-build
cmake . ; make package
dpkg -i kappaio-1.2.0-linux.deb
```

For questions : dev@kapparock.com

##License
The MIT License (MIT)

Copyright (c) 2016 kappaIO

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
