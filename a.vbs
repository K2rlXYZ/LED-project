Set oShell = CreateObject ("Wscript.Shell") 
Dim strArgs
strArgs = "cmd /c audio.bat"
oShell.Run strArgs, 0, false