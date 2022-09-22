cd /d %~dp0
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
meson setup -Dc_args=-DSHMEM=1 build64 --buildtype=release
meson configure build64
ninja -C build64
mkdir bin
copy build64\src\chuniio\chuniio.dll bin\chusanio_x64.dll
copy build64\src\divaio\divaio.dll bin\divaio_x64.dll
copy build64\src\aimeio\aimeio.dll bin\aimeio_x64.dll
pause