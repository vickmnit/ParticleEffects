@echo off
::set PATH=.\third_party\llvm-mingw-20220209-ucrt-i686\bin;%PATH%

cmake -H./ -B./build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=mingw32-make
cd build
mingw32-make -j 4
xcopy "../third_party/pvrframe/Windows_x86_64/libGLESv2.dll" "./" /I /R /K /Y /Q /F /D /S
xcopy "../res" "./res" /I /R /K /Y /Q /F /D /S
::cd .. 

::cmake -H./ -B./build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=mingw32-make
::cd build
::mingw32-make -j 4
xcopy "../third_party/pvrframe/Windows_x86_64/libEGL.dll" "./" /I /R /K /Y /Q /F /D /S
xcopy "../res" "./res" /I /R /K /Y /Q /F /D /S

xcopy "../third_party/glfw/lib/glfw3.dll" "./" /I /R /K /Y /Q /F /D /S
xcopy "../res" "./res" /I /R /K /Y /Q /F /D /S
cd .. 

cmd /k