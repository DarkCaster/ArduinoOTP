@echo off
REM Use random variable name, so calling another bat file will not spoil it.
set aafc2aac361f=%~dp0

echo Before building dependencies for the first time run 'git submodule update --init --recursive' from git console

echo Removing custom NuGet.Config
del "%aafc2aac361f%\NuGet.Config"

echo Removing custom NuGet.Config
del "%aafc2aac361f%\DotNetBlocks\NuGet.Config"

echo Starting up build system (this may take some time)

REM build all external deps for DotNetBlocks

call "%aafc2aac361f%\DotNetBlocks\External\DotNetBuildTools\prepare-and-build.bat" "%aafc2aac361f%\DotNetBlocks\External"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

copy /Y "%aafc2aac361f%\DotNetBlocks\External\DotNetBuildTools\dist\extra\NuGet.Config" "%aafc2aac361f%\DotNetBlocks\NuGet.Config"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

REM use DotNetBuildTools sub-submodule from DotNetBlocks submodule and build all external deps for this project

call "%aafc2aac361f%\DotNetBlocks\External\DotNetBuildTools\prepare-and-build.bat" "%aafc2aac361f%\External"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

copy /Y "%aafc2aac361f%\External\NuGet.Config" "%aafc2aac361f%\NuGet.Config"
IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

