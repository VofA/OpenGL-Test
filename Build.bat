@echo off

cd sources

@echo off

:do
  cls
  cmake ./
  set /p input=

goto do