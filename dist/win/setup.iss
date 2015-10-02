#define MainApp '../../build/Track-o-Bot.exe'
#define Major
#define Minor
#define Rev
#define Build
#expr ParseVersion(MainApp, Major, Minor, Rev, Build)
#define Version Str(Major)+"."+Str(Minor)+"."+Str(Rev)

[Setup]
AppName=Track-o-Bot
AppVersion={#Version}
AppPublisher=spidy.ch
AppPublisherURL=http://spidy.ch
DefaultDirName={pf}\Track-o-Bot
DefaultGroupName=Track-o-Bot
UninstallDisplayIcon={app}\Track-o-Bot.exe
OutputBaseFilename=Track-o-Bot_{#Version}

[InstallDelete]
Type: Files; Name: "{app}/msvcp100.dll"
Type: Files; Name: "{app}/msvcr100.dll"
Type: Files; Name: "{app}/QtCore4.dll" 
Type: Files; Name: "{app}/QtGui4.dll"
Type: Files; Name: "{app}/QtNetwork4.dll"
Type: Files; Name: "{app}/QtSvg4.dll"
Type: Files; Name: "{app}/imageformats/qgif4.dll"
Type: Files; Name: "{app}/imageformats/qico4.dll"
Type: Files; Name: "{app}/imageformats/qjpeg4.dll"
Type: Files; Name: "{app}/imageformats/qmng4.dll"
Type: Files; Name: "{app}/imageformats/qsvg4.dll"
Type: Files; Name: "{app}/imageformats/qtga4.dll"
Type: Files; Name: "{app}/imageformats/qtiff4.dll"

[Files]
Source: "../../build/Track-o-Bot.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "../../../WinSparkle/Release/WinSparkle.dll"; DestDir: "{app}"; Flags: ignoreversion  

; Required for QtNetworkManager to work with HTTPS
Source: "../../../OpenSSL-Win32/bin/libeay32.dll"; DestDir: "{app}"
Source: "../../../OpenSSL-Win32/bin/ssleay32.dll"; DestDir: "{app}"

; Qt
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/bin/Qt5Core.dll"; DestDir: "{app}"   
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/bin/Qt5Widgets.dll"; DestDir: "{app}"    
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/bin/Qt5WinExtras.dll"; DestDir: "{app}"   
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/bin/Qt5Gui.dll"; DestDir: "{app}"                      
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/bin/Qt5Network.dll"; DestDir: "{app}"                      
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/plugins/imageformats/*.dll"; Excludes: "*d.dll"; DestDir: "{app}/imageformats" 
Source: "../../../../Qt/Qt5.5.0/5.5/msvc2012/plugins/platforms/*.dll"; Excludes: "*d.dll"; DestDir: "{app}/platforms" 

; C/C++ Runtime
Source: "../../../../Program Files (x86)/Microsoft Visual Studio 11.0/VC/redist/x86/Microsoft.VC110.CRT/msvcp110.dll"; DestDir: "{app}"    
Source: "../../../../Program Files (x86)/Microsoft Visual Studio 11.0/VC/redist/x86/Microsoft.VC110.CRT/msvcr110.dll"; DestDir: "{app}" 

[Run]
Filename: "{app}\Track-o-Bot.exe"; Description: "Launch Track-o-Bot"; Flags: postinstall nowait

[Icons]
Name: "{group}\Track-o-Bot"; Filename: "{app}\Track-o-Bot.exe"

; This is temporary workaround for people upgrading from 0.6.0
; 0.6.0 is affected by QTBUG-35986
; This code can be removed once 0.6.0 is outdated
[Code]
function InitializeSetup(): Boolean;
	var ErrorCode: Integer;
begin
	ShellExec('open', 'taskkill.exe', '/f /im Track-o-Bot.exe', '', SW_HIDE, ewNoWait, ErrorCode);
	result := True;
end;

function InitializeUninstall(): Boolean;
	var ErrorCode: Integer;
begin
	ShellExec('open', 'taskkill.exe', '/f /im Track-o-Bot.exe', '', SW_HIDE, ewNoWait, ErrorCode);
	result := True;
end;

