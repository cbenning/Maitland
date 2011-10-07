@echo // Copyright (C) 1996-2003 Markus F.X.J. Oberhumer
@echo //
@echo //   Windows 32-bit
@echo //   Intel C/C++
@echo //
@call b\prepare.bat
@if "%BECHO%"=="n" echo off


set CC=icl -nologo -MD
set CF=-O2 -GF -W3 %CFI% %CFASM%
REM set CF=-O2 -GF -W4 -Qwd171,181,193,279,593,810,981,1418 %CFI% %CFASM%
set LF=%BLIB% setargv.obj

%CC% %CF% -c @b\src.rsp
@if errorlevel 1 goto error
lib -nologo -out:%BLIB% @b\win32\vc.rsp
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
