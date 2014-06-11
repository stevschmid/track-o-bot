call "%vs100comntools%vsvars32.bat
cd ..\..\
qmake track-o-bot.pro
nmake clean
del build\Track-o-Bot.exe
nmake release

iscc dist/win/setup.iss
