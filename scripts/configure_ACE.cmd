@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: configure_ACE.bat
@rem //
@rem // Arguments: - platform [win32] {win32}
@rem //            - mwc.pl '-type' parameter {vs2017}
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
echo usage: %~n0 ^[win32^] ^[vs2022^]
goto Clean_Up

:Begin
set argC=0
for %%x in (%*) do set /A argC+=1
set OR_=
@rem if (%argC% lss 2) set OR_=true
if (%argC% gtr 2) set OR_=true
if "%OR_%"=="true" (
 echo invalid #argument^(s^) ^(was: %argC%^)^, exiting
 goto Print_Usage
)
set PLATFORM_TYPE=%1
set PROJECT_TYPE=%2

set DEFAULT_PLATFORM_TYPE=win32
call :switch_case_%PLATFORM_TYPE% 2>nul
if %ERRORLEVEL% NEQ 0 (
 set PLATFORM_TYPE=%DEFAULT_PLATFORM_TYPE%
 goto :switch_case_end
)
:switch_case_win32
 goto :switch_case_end
:switch_case_end
echo platform: "!PLATFORM_TYPE!"...

set DEFAULT_PROJECT_TYPE=vs2022
call :switch_case_2_%PROJECT_TYPE% 2>nul
if %ERRORLEVEL% NEQ 0 (
 set PROJECT_TYPE=%DEFAULT_PROJECT_TYPE%
 goto :switch_case_2_end
)
:switch_case_2_vc10
 goto switch_case_2_end
:switch_case_2_vc15
 goto switch_case_2_end
:switch_case_2_vs2017
 goto switch_case_2_end
:switch_case_2_vs2019
 goto switch_case_2_end
:switch_case_2_vs2022
 goto switch_case_2_end
:switch_case_2_end
echo project: "!PROJECT_TYPE!"...

set DEFAULT_PROJECT_DIRECTORY=%~dp0..\..
set PROJECT_DIRECTORY=%DEFAULT_PROJECT_DIRECTORY%
if NOT exist "%PROJECT_DIRECTORY%" (
 echo invalid directory ^(was: "%PROJECT_DIRECTORY%"^)^, exiting
 goto Failed
)

set ACE_ROOT=%ACE_ROOT%
if NOT exist "%ACE_ROOT%" (
 echo invalid directory ^(was: "%ACE_ROOT%"^)^, falling back
 set ACE_ROOT=%LIB_ROOT%\ACE_TAO\ACE
 if NOT exist "!ACE_ROOT!" (
  echo invalid directory ^(was: "!ACE_ROOT!"^)^, exiting
  goto Failed
 )
)

set MPC_ROOT=%MPC_ROOT%
if NOT exist "%MPC_ROOT%" (
 echo invalid directory ^(was: "%MPC_ROOT%"^)^, falling back
 set MPC_ROOT=%LIB_ROOT%\MPC
 if NOT exist "!MPC_ROOT!" (
  echo invalid directory ^(was: "!MPC_ROOT!"^)^, exiting
  goto Failed
 )
)

@rem step1: apply patches
set PATCH_EXE=%BIN_ROOT%\patch.exe
if NOT exist "%PATCH_EXE%" (
 echo invalid executable ^(was: "%PATCH_EXE%"^)^, exiting
 goto Failed
)
cd !ACE_ROOT!
if %ERRORLEVEL% NEQ 0 (
 echo failed to cd ^(was: "!ACE_ROOT!"^)^, exiting
 goto Failed
)
set PROJECTS=Common ACEStream ACENetwork
for %%a in (%PROJECTS%) do (
 echo INFO: processing project "%%a"...
 set PATCH_DIRECTORY=%PROJECT_DIRECTORY%\%%a\3rd_party\ACE_wrappers\patches
 if NOT exist "!PATCH_DIRECTORY!" (
  echo invalid directory ^(was: "!PATCH_DIRECTORY!"^)^, exiting
  goto Failed
 )
 for %%b in (!PATCH_DIRECTORY!\*.patch) do (
  echo INFO: applying patch "%%b"...
  %PATCH_EXE% -f -i%%b -p4 -s -u
  if %ERRORLEVEL% NEQ 0 (
   echo failed to apply patch ^(was: "%%b"^)^, exiting
   goto Failed
  )
 )
 echo INFO: processing project "%%a"...DONE
)

@rem step2: verify build directories
set DEFAULT_ACE_BUILD_ROOT_DIRECTORY=%ACE_ROOT%\build
set ACE_BUILD_ROOT_DIRECTORY=%DEFAULT_ACE_BUILD_ROOT_DIRECTORY%
if NOT exist "%ACE_BUILD_ROOT_DIRECTORY%" (
 mkdir "%ACE_BUILD_ROOT_DIRECTORY%"
 if %ERRORLEVEL% NEQ 0 (
  echo failed to mkdir ^(was: "%ACE_BUILD_ROOT_DIRECTORY%"^)^, exiting
  goto Failed
 )
 echo created directory "%ACE_BUILD_ROOT_DIRECTORY%"
)
set DEFAULT_ACE_BUILD_DIRECTORY=%ACE_BUILD_ROOT_DIRECTORY%\%PLATFORM_TYPE%
set ACE_BUILD_DIRECTORY=%DEFAULT_ACE_BUILD_DIRECTORY%
call :switch_case_3_%PLATFORM_TYPE% 2>nul
if %ERRORLEVEL% NEQ 0 (
 @rem set ACE_BUILD_DIRECTORY=%ACE_BUILD_ROOT_DIRECTORY%\%PLATFORM_TYPE%
 goto :switch_case_3_end
)
:switch_case_3_win32
 set ACE_BUILD_DIRECTORY=%ACE_BUILD_ROOT_DIRECTORY%\%PLATFORM_TYPE%
 goto :switch_case_3_end
:switch_case_3_end
if NOT exist "%ACE_BUILD_DIRECTORY%" (
 mkdir "%ACE_BUILD_DIRECTORY%"
 if %ERRORLEVEL% NEQ 0 (
  echo failed to mkdir ^(was: "%ACE_BUILD_DIRECTORY%"^)^, exiting
  goto Failed
 )
 echo created directory "%ACE_BUILD_DIRECTORY%"
)
@rem set CREATE_ACE_BUILD=%ACE_ROOT%\bin\create_ace_build.pl
@rem if NOT exist "%CREATE_ACE_BUILD%" (
@rem  echo invalid directory ^(was: "%CREATE_ACE_BUILD%"^)^, exiting
@rem  goto Failed
@rem )
set PERL_EXE=C:\Perl64\bin\perl.exe
if NOT exist "%PERL_EXE%" (
 echo invalid executable ^(was: "%PERL_EXE%"^)^, exiting
 goto Failed
)
@rem %PERL_EXE% "%CREATE_ACE_BUILD%" -a -v %PROJECT_TYPE% 2>nul
@rem if %ERRORLEVEL% NEQ 0 (
@rem  echo failed to run script ^(was: "%CREATE_ACE_BUILD%"^)^, exiting
@rem  goto Failed
@rem )

@rem step3: generate Makefiles
set FEATURES_FILE_DIRECTORY=%PROJECT_DIRECTORY%\Common\3rd_party\ACE_wrappers
if NOT exist "%FEATURES_FILE_DIRECTORY%" (
 echo invalid directory ^(was: "%FEATURES_FILE_DIRECTORY%"^)^, exiting
 goto Failed
)
set LOCAL_FEATURES_FILE=%FEATURES_FILE_DIRECTORY%\local.features
if NOT exist "%LOCAL_FEATURES_FILE%" (
 echo invalid directory ^(was: "%LOCAL_FEATURES_FILE%"^)^, exiting
 goto Failed
)
echo INFO: feature file is: "%LOCAL_FEATURES_FILE%"
set ACE_MWC_FILE=!ACE_ROOT!\ACE.mwc
if NOT exist "%ACE_MWC_FILE%" (
 echo invalid file ^(was: "%ACE_MWC_FILE%"^)^, exiting
 goto Failed
)

set MWC_PL=!ACE_ROOT!\bin\mwc.pl
if NOT exist "%MWC_PL%" (
 echo invalid script ^(was: "%MWC_PL%"^)^, exiting
 goto Failed
)
set MWC_PL_OPTIONS=
@rem if [ "${PLATFORM}" = "linux" ]
@rem then
@rem MWC_PL_OPTIONS="-name_modifier *_gnu"
@rem fi
cd "%ACE_BUILD_DIRECTORY%"
%PERL_EXE% "%MWC_PL%" -feature_file "%LOCAL_FEATURES_FILE%" %MWC_PL_OPTIONS% -type %PROJECT_TYPE% "%ACE_MWC_FILE%"
if %ERRORLEVEL% NEQ 0 (
 echo failed to run script ^(was: "%ACE_MWC_FILE%"^)^, exiting
 goto Failed
)
echo processing "%ACE_MWC_FILE%"...DONE

@rem step4: make
@rem set MAKE_EXE=D:\temp\bin\make.exe
@rem if NOT exist "%MAKE_EXE%" (
@rem echo invalid executable ^(was: "%MAKE_EXE%"^)^, exiting
@rem  goto Failed
@rem )
@rem set MAKE_OPTIONS=-j4
@rem %MAKE_EXE% %MAKE_OPTIONS%
@rem if %ERRORLEVEL% NEQ 0 (
@rem  echo "failed to make: $?, exiting
@rem  goto Failed
@rem )

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
::echo %ERRORLEVEL% %1 *WORKAROUND*
exit /b %1

:Error_Level
call :Exit_Code %RC%

