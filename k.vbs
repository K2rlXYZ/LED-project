Set oShell = CreateObject ("Wscript.Shell") 
Dim strArgs
strArgs = "cmd /c numkeys.bat"
oShell.Run strArgs, 0, false