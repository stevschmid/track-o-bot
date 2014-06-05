call "%vs100comntools%vsvars32.bat
cd ..\..\
qmake track-o-bot.pro
nmake clean
nmake release

iscc dist/win/setup.iss
