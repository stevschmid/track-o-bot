# Build Dependencies

* Qt 5.5
* (OS X) Sparkle
* (Windows) WinSparkle

## Mac OS X

```
brew install qt5
brew link qt5 --force
```

* Install [Sparkle](http://sparkle.andymatuschak.org/) 
 * Move _Sparkle.framework_ to ``/Library/Frameworks``.

## Windows

* Install [Qt](http://qt-project.org/downloads) 
 * I use the Qt libraries 5.5.0 for Windows VS 2010.
* Install [WinSparkle](https://github.com/vslavik/winsparkle) 
 * Clone the repository and build the library, for example with VS 2010. The precompiled releases are ancient.

# Build Instructions

```
qmake
make
```

(Use ``nmake`` instead of ``make`` on Windows)

The resulting binary can be found in the ``build`` subfolder.

# Contributing

Feel free to submit pull requests, suggest new ideas and discuss issues. Track-o-Bot is about simplicity and usability. Only features which benefit all users will be considered. 

# License

GNU Lesser General Public License (LGPL) Version 2.1.

See [LICENSE](LICENSE).

