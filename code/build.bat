@echo off

REM Tool paths
set CLANG=			C:\dev\wasm\clang
set WASMLD=			C:\dev\wasm\wasm-ld

REM Global compiler flags
set CCFLAGS=		-Ofast -std=c99

REM Global compiler flags for Wasm targeting
set CLANGFLAGS=		--target=wasm32 -nostdlib
set CLANGFLAGS=		%CLANGFLAGS% -fvisibility=hidden -fno-builtin -fno-exceptions -fno-threadsafe-statics

REM Flags for wasm-ld
::set WASMLDFLAGS=	--export-all --no-entry -allow-undefined -export=main
set WASMLDFLAGS=	-Wl,--export-all -Wl,--no-entry -Wl,-allow-undefined -Wl,--lto-O3
:: --import-memory

REM Compile and link the source files
::%CLANG% %CCFLAGS% %CLANGFLAGS% -o ..\build\main.o -c main.c
REM Link the object files
::%WASMLD% %WASMLDFLAGS% -o ..\build\game.wasm ..\build\main.o

%CLANG% -DDEBUG --target=wasm32 -O3 -flto -nostdlib %WASMLDFLAGS% -o ..\build\game.wasm main.c
REM can add -g flag above for debug info, but NOTE that it greatly increases the output file size

REM Copy the loader files
copy	index.js				..\build\index.js
copy	index.html				..\build\index.html
copy	..\data\wojtek.ico		..\build\wojtek.ico
::del		..\build\main.o
