
for /f "delims=" %%x in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property InstallationPath') do set VSPATH=%%x
call "%VSPATH%\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.35
