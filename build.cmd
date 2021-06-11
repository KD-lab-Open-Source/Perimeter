mkdir build
cd build

cmake -G "Visual Studio 16 2019" -A "Win32" -Thost=x64 ..
cmake --build .
