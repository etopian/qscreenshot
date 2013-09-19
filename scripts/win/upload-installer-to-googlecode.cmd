@echo off
@echo Uploading qScreenshot Windows Installer to Google Code
call googlecode_upload.py -p "qscreenshot" -s "qScreenshot 0.7 Windows Installer || Qt 4.8.5" -l "Featured,Windows,Installer" "setup\qscreenshot-0.7-win32-setup.exe"
@echo Completed
pause & pause
