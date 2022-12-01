@echo off
call _prepare-build.bat
call %cc% %FullCode% -DCHECKS=1 %Includes% -o %ProjectName%.exe %Libraries%
call %ProjectName%.exe
