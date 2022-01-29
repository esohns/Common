@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: initialize_ACE.bat
@rem //
@rem // arguments [1]platform: linux | macos | solaris | windows
@rem // arguments [2]bitness : 32 | {64}
@rem // History:
@rem //   Date   |Name | Description of modification
@rem // ---------|-----|-------------------------------------------------------------
@rem // 20/02/06 | soh | Creation.
@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////

@echo off
set RC=0
setlocal enabledelayedexpansion
pushd . >NUL 2>&1
goto Begin

:Print_Usage
echo usage: %~n0 ^[linux ^| macos ^| solaris ^| windows^] ^[32 ^| ^{64^}^]
goto Clean_Up

:Begin
@rem prerequisites
@rem if NOT exist "%LIB_ROOT%" (
@rem  echo invalid directory LIB_ROOT ^(was: "%LIB_ROOT%"^)^, exiting
@rem  goto Failed
@rem )
set PROJECTS_ROOT_DIR_DEFAULT=D:\projects
set PROJECTS_ROOT_DIR="%PRJ_ROOT%"
if NOT exist "%PROJECTS_ROOT_DIR%" (
 set PROJECTS_ROOT_DIR="%PROJECTS_ROOT_DIR_DEFAULT%"
)
if NOT exist "%PROJECTS_ROOT_DIR%" (
 echo invalid projects directory ^(was: "%PROJECTS_ROOT_DIR%"^)^, exiting
 goto Failed
)

@rem handle options
if "%1."=="." (
 echo invalid argument^, exiting
 goto Print_Usage
)
set BITS=%2
if "%BITS%."=="." (
 echo setting to 64 bits
 set BITS=64
)
if NOT "%1"=="linux" if NOT "%1"=="macos" if NOT "%1"=="solaris" if NOT "%1"=="windows" (
 echo invalid argument ^(was: "%1"^)^, exiting
 goto Print_Usage
)
echo platform: "%1"
if %BITS% NEQ 32 if %BITS% NEQ 64 (
 echo invalid argument ^(was: "%BITS%"^)^, exiting
 goto Print_Usage
)

@rem copy config.h
echo processing config.h...
set ACE_ROOT_DEFAULT=%LIB_ROOT%\ACE_TAO\ACE
if "%ACE_ROOT%" == "" (
 echo "ACE_ROOT" not set^, default to ^"%ACE_ROOT_DEFAULT%^"
 set ACE_ROOT=%ACE_ROOT_DEFAULT%
) else (
@rem echo "%%ACE_ROOT%%" set to ^"%ACE_ROOT%^"...
)
if NOT exist "%ACE_ROOT%" (
 echo invalid directory ^(was: "%ACE_ROOT%"^)^, exiting
 goto Failed
)
set TARGET_DIRECTORY=%ACE_ROOT%\ace
if NOT exist "%TARGET_DIRECTORY%" (
 echo invalid directory ^(was: ^"%TARGET_DIRECTORY%^"^)^, exiting
 goto Failed
)
set TARGET_FILE=%TARGET_DIRECTORY%\config.h
set SOURCE_DIRECTORY=%PROJECTS_ROOT_DIR%\Common\3rd_party\ACE_wrappers\ace
if NOT exist "%SOURCE_DIRECTORY%" (
 echo invalid directory ^(was: "%SOURCE_DIRECTORY%"^)^, exiting
 goto Failed
)
set SOURCE_FILE=%SOURCE_DIRECTORY%\config-%1.h
if NOT exist "%SOURCE_FILE%" (
 echo invalid file ^(was: "%SOURCE_FILE%"^)^, exiting
 goto Failed
)

@copy /Y "%SOURCE_FILE%" "%SOURCE_DIRECTORY%\config.h" >NUL
if %ERRORLEVEL% NEQ 0 (
 echo failed to copy ^"%SOURCE_FILE%^" file, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@move /Y "%SOURCE_DIRECTORY%\config.h" "%TARGET_FILE%" >NUL
if %ERRORLEVEL% NEQ 0 (
 echo failed to move ^"%SOURCE_DIRECTORY%\config.h^" file, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
echo copied ^"%SOURCE_FILE%^" to ^"%TARGET_FILE%^"...
echo processing config.h...DONE

@rem copy platform_macros.GNU
echo processing platform_macros.GNU...
set TARGET_DIRECTORY=%ACE_ROOT%\include\makeinclude
if NOT exist "%TARGET_DIRECTORY%" (
 echo invalid directory ^(was: ^"%TARGET_DIRECTORY%^"^)^, exiting
 goto Failed
)
set TARGET_FILE=%TARGET_DIRECTORY%\platform_macros.GNU
set SOURCE_DIRECTORY=%PROJECTS_ROOT_DIR%\Common\3rd_party\ACE_wrappers\include\makeinclude
if NOT exist "%SOURCE_DIRECTORY%" (
 echo invalid directory ^(was: ^"%SOURCE_DIRECTORY%^"^)^, exiting
 goto Failed
)
if "%1"=="linux" (
 set SOURCE_FILE=%SOURCE_DIRECTORY%\platform_macros_linux.GNU
) else if "%1"=="macos" (
  set SOURCE_FILE=%SOURCE_DIRECTORY%\platform_macros_macos.GNU
) else if "%1"=="solaris" (
  set SOURCE_FILE=%SOURCE_DIRECTORY%\platform_macros_solaris.GNU
) else if "%1"=="windows" (
  set SOURCE_FILE=%SOURCE_DIRECTORY%\platform_macros_windows_msvc.GNU
)
if NOT exist "%SOURCE_FILE%" (
 echo invalid file ^(was: ^"%SOURCE_FILE%^"^)^, exiting
 goto Failed
)

@copy /Y "%SOURCE_FILE%" "%SOURCE_DIRECTORY%\platform_macros.GNU" >NUL
if %ERRORLEVEL% NEQ 0 (
 echo failed to copy ^"%SOURCE_FILE%^" file, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
if %BITS% NEQ 32 (
 sed.exe -e "s/buildbits=32/buildbits=%BITS%/g" -i -s "%SOURCE_DIRECTORY%\platform_macros.GNU" >NUL
 if %ERRORLEVEL% NEQ 0 (
  echo failed to sed ^"platform_macros.GNU^" file, exiting
  set RC=%ERRORLEVEL%
  goto Failed
 )
)
@move /Y "%SOURCE_DIRECTORY%\platform_macros.GNU" "%TARGET_FILE%" >NUL
if %ERRORLEVEL% NEQ 0 (
 echo failed to move ^"platform_macros.GNU^" file, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
echo copied ^"%SOURCE_FILE%^" to ^"%TARGET_FILE%^"...
echo processing platform_macros.GNU...DONE

timeout /T 2 /NOBREAK >NUL

goto Clean_Up

:Failed
echo processing...FAILED

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
