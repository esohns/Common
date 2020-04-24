@echo off
set RC=0
::setlocal enabledelayedexpansion
::pushd . >NUL 2>&1
::goto Begin

::Begin:
echo "wwow"
set LOCATION=%PRJ_ROOT%\\Common\\src\\parser\\etc\\antlr-4.7.1-complete.jar
::call java -jar %LOCATION% -Dlanguage=Cpp -listener -visitor -o generated/ -package antlrcpptest lexer_class.g4 parser_class.g4
::java -jar %LOCATION% -Dlanguage=Cpp -listener -visitor -o generated/ -package antlrcpptest -XdbgST TLexer.g4 TParser.g4
::java -jar %LOCATION% -Dlanguage=Java -listener -visitor -o generated/ -package antlrcpptest TLexer.g4 TParser.g4
if %ERRORLEVEL% NEQ 0 (
  echo failed to generate parser file^(s^)^, exiting
  set RC=%ERRORLEVEL%
  goto Failed
)

echo %~dp0\\generated
for /R "%~dp0\generated" %%f in (*.h) do (
  echo %%f
  cp /Y "%%f" "~dp0\.." >NUL
  if %ERRORLEVEL% NEQ 0 (
    echo failed to cp file^(s^)^, exiting
    set RC=%ERRORLEVEL%
    goto Failed
  )
  echo copied "%%f"...
)
for /R "%~dp0\generated" %%f in (*.cpp) do (
  cp /Y "%%f" "~dp0\.." >NUL
  if %ERRORLEVEL% NEQ 0 (
    echo failed to cp file^(s^)^, exiting
    set RC=%ERRORLEVEL%
    goto Failed
  )
  echo copied "%%f"...
)
goto Clean_Up

:Failed
echo processing parser...FAILED

:Clean_Up
popd
::endlocal & set RC=%ERRORLEVEL%
endlocal & set RC=%RC%
goto Error_Level

:Exit_Code
:: echo %ERRORLEVEL% %1 *WORKAROUND*
exit /b %1

:Error_Level
call :Exit_Code %RC%

