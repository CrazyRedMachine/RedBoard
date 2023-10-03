cd /d %~dp0
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64_x86
meson configure build_fufubot
meson --reconfigure build_fufubot
meson setup -Dc_args=-DFUFUBOT=1 build_fufubot --werror --buildtype=release
ninja -C build_fufubot
mkdir bin
copy build_fufubot\src\chuniio\chuniio.dll bin\redboard.dll
copy build_fufubot\src\aimeio\aimeio.dll bin\aimeio.dll
copy build_fufubot\src\chuniio\chunitest.exe bin\
copy build_fufubot\src\aimeio\aimetest.exe bin\
pause