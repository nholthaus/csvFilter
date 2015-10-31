csvFilter - by Nic Holthaus
--------------------------------------------
The MIT License (MIT)

Copyright (c) 2014 Nic Holthaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------
1. INSTALLING
--------------------------------------------
1. Double Click the included installer "csvInstaller-Installer.exe"
2. Follow the prompts

--------------------------------------------
3. BUILDING
--------------------------------------------
To build the software, you'll need Qt 5.4 or later (used for image processing/GUI),
CMAKE to build the makefile/visual studio project, and NSIS if you want to build the windows
installer package. As of writing this, the project is only really set up for windows. If that's not the case, the clock code itself should
be a trivial port since cmake/qt are natively cross-platform.
