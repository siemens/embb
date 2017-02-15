:: Copyright (c) 2014-2017, Siemens AG. All rights reserved.
:: 
:: Redistribution and use in source and binary forms, with or without
:: modification, are permitted provided that the following conditions are met:
:: 
:: 1. Redistributions of source code must retain the above copyright notice,
:: this list of conditions and the following disclaimer.
:: 
:: 2. Redistributions in binary form must reproduce the above copyright notice,
:: this list of conditions and the following disclaimer in the documentation
:: and/or other materials provided with the distribution.
:: 
:: THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
:: AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
:: IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
:: ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
:: LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
:: CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
:: SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
:: INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
:: CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
:: ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
:: POSSIBILITY OF SUCH DAMAGE.

:: Needs to be located in the folder containing the tests!!
:: Is copied automatically there when generating build files with cmake.
@echo off
setlocal EnableDelayedExpansion 
SET NUM_ERRORS=0
SET DIR=%~dp0 
SET EMBB_EXECUTABLES=@EXPECTED_EMBB_TEST_EXECUTABLES@

call :parse "%EMBB_EXECUTABLES%"
goto :end

:parse
set list=%1
set list=%list:"=%
FOR /f "tokens=1* delims=;" %%a IN ("%list%") DO (
  if not "%%a" == "" call :sub %%a
  if not "%%b" == "" call :parse "%%b"
)
exit /b

:sub
call "%DIR:~0,-1%\%1.exe"
if not !ERRORLEVEL! ==0 set /a NUM_ERRORS=!NUM_ERRORS!+1
exit /b


:end

if not !NUM_ERRORS! ==0 (
 echo.
 SET ERRORLEVEL=1
 echo "Number of failed tests: !NUM_ERRORS!"
 exit /b !NUM_ERRORS!
)
