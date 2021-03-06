@echo // Copyright (C) 1996-2003 Markus F.X.J. Oberhumer
@echo //
@echo //   DOS 16-bit
@echo //   Watcom C/C++
@echo //
@call b\prepare.bat
@if "%BECHO%"=="n" echo off


set CC=wcl -zq -ml -bt#dos -l#dos
set CF=-ox -w5 %CFI%
set LF=%BLIB%

%CC% %CF% -c src\*.c
@if errorlevel 1 goto error
wlib -q -b -n -t %BLIB% @b\dos16\wc.rsp
@if errorlevel 1 goto error

%CC% %CF% examples\simple.c %LF%
@if errorlevel 1 goto error
%CC% %CF% examples\uclpack.c %LF%
@if errorlevel 1 goto error


@call b\done.bat
@goto end
:error
@echo ERROR during build!
:end
@call b\unset.bat
