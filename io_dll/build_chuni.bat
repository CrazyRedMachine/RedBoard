cd /d %~dp0
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64_x86
meson setup build_chuni --werror --buildtype=release
meson configure build_chuni
ninja -C build_chuni
mkdir bin
copy build_chuni\src\chuniio\chuniio.dll bin\chuniio.dll
copy build_chuni\src\aimeio\aimeio.dll bin\aimeio.dll
copy build_chuni\src\chuniio\chunitest.exe bin\
copy build_chuni\src\aimeio\aimetest.exe bin\
pause