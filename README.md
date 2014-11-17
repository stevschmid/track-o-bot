# Build Dependencies

* Qt 4.8
* (OS X) Sparkle
* (Windows) WinSparkle

## Mac OS X

* Install of the Qt [High DPI 4.8 backport](https://bugreports.qt-project.org/browse/QTBUG-23870) [patch](https://codereview.qt-project.org/#/c/54636/) is required
```
brew edit qt
```
Add the following patch
```
patch do
  url "https://gist.githubusercontent.com/stevschmid/91a564b7a3f562e6a213/raw/ccfe0dcf2c5479356906fdfcdf229c2acb1bee39/qt48-high-dpi-backport-fixed.patch"
  sha1 "8460eac1c2dfcff9dfbe240472ef6b66e58eb9f9"
end
```
Build from source
```
brew reinstall qt --build-from-source
```

* Install [Sparkle](http://sparkle.andymatuschak.org/) 
 * Move _Sparkle.framework_ to ``/Library/Frameworks``.

## Windows

* Install [Qt](http://qt-project.org/downloads) 
 * I use the Qt libraries 4.8.6 for Windows VS 2010.
* Install [WinSparkle](https://github.com/vslavik/winsparkle) 
 * Clone the repository and build the library, for example with VS 2010. The precompiled releases are ancient.

## GNU/Linux

* Install Qt4.8 with your distribution package manager (apt, etc...)
* Create folder .Hearthstone in your home directory
* Create symlinks of the Hearthstone's output_log and log.config in the newly created folder

# Build Instructions

```
qmake
make
```

(Use ``nmake`` instead of ``make`` on Windows)

On Linux, you might have to do the following if you have qt5 installed

```
qtmake-qt4
make
```

The resulting binary can be found in the ``build`` subfolder.

# Install Instructions

```
sudo make install
```

Default install prefix is /usr/local. You can change it by adding PREFIX argument to qmake-qt4 command eg:

```
qmake-qt4 PREFIX=/usr
```

# Contributing

Feel free to submit pull requests, suggest new ideas and discuss issues. Track-o-Bot is about simplicity and usability. Only features which benefit all users will be considered. 

# License

GNU Lesser General Public License (LGPL) Version 2.1.

See [LICENSE](LICENSE).

