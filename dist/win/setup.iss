[Setup]
AppName=Track-o-Bot
AppVersion=0.2.0
AppPublisher=spidy.ch
AppPublisherURL=http://spidy.ch
DefaultDirName={pf}\Track-o-Bot
DefaultGroupName=Track-o-Bot
UninstallDisplayIcon={app}\Track-o-Bot.exe

[Files]
Source: "../../build/Track-o-Bot.exe"; DestDir: "{app}"
Source: "../../../WinSparkle/Release/WinSparkle.dll"; DestDir: "{app}"  

; Required for QtNetworkManager to work with HTTPS
Source: "../../../OpenSSL-Win32/bin/libeay32.dll"; DestDir: "{app}"
Source: "../../../OpenSSL-Win32/bin/ssleay32.dll"; DestDir: "{app}"

[Run]
Filename: "{app}\Track-o-Bot.exe"; Description: "Launch Track-o-Bot"; Flags: postinstall nowait

[Icons]
Name: "{group}\Track-o-Bot"; Filename: "{app}\Track-o-Bot.exe"

