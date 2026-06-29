@echo off
if not exist gm82gex.py curl https://raw.githubusercontent.com/GM82Project/gm82gex/main/gm82gex.py -o gm82gex.py

cmake -A Win32 -B build && cmake --build build --config Release

pause