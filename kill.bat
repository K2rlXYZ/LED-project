tasklist /fi "ImageName eq LEDaudio.exe" /fo csv 2>NUL | find /I "LEDaudio.exe">NUL
if "%ERRORLEVEL%"=="0" taskkill /f /im LEDaudio.exe
tasklist /fi "ImageName eq LEDkeys.exe" /fo csv 2>NUL | find /I "LEDkeys.exe">NUL
if "%ERRORLEVEL%"=="0" taskkill /f /im LEDkeys.exe