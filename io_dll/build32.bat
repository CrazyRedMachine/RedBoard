cd /d %~dp0
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64_x86
meson setup -Dc_args=-DSHMEM=1 build32 --werror --buildtype=release
meson configure build32
ninja -C build32
mkdir bin
copy build32\src\chuniio\chuniio.dll bin\chusanio_x86.dll
copy build32\src\divaio\divaio.dll bin\divaio_x86.dll
pause