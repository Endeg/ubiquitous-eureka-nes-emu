@REM TODO: Create build files for tcc, gcc and clang
@REM set cc=zig cc -target i386-windows-gnu
@REM set cc=zig cc -target x86_64-windows-gnu
@REM set cc=gcc
@REM set cc=tcc
set cc=cl
set Includes=-Iinclude -Iexternal
set DebugFlags=-Zi
@REM set Libraries=-lwinmm -lgdi32
set Libraries=
set ProjectName=emulator
set Code=src/main.c
set FullCode=%Code%
