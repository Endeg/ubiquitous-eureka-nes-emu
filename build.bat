@echo off
call _prepare-build.bat
call %cc% %FullCode% %DebugFlags% -DCHECKS=1 %Includes% -Fe%ProjectName%.exe %Libraries%
