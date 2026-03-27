$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
cd $PSScriptRoot
cmake --build ".\output\build"
.\output\build\FlappyBird.exe
