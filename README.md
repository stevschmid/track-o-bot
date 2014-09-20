# Build Dependencies

* Qt 4.8
* (OS X) Sparkle
* (Windows) WinSparkle

## Mac OS X

* Install Qt
  ```
  brew install qt
  ```

* Install [Sparkle](http://sparkle.andymatuschak.org/) 
 * Move _Sparkle.framework_ to ``/Library/Frameworks``.
 * **Mavericks only:** Clang doesn't look in the usual framework directories, so you need to copy _Sparkle.framework_ where clang can find it.
 
        ```
sudo cp -fR ~/Downloads/Sparkle-1.8.0/Sparkle.framework/  
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/
        ```

## Windows

* Install [Qt](http://qt-project.org/downloads) 
 * I use the Qt libraries 4.8.6 for Windows VS 2010.
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

Feel free to submit pull requests, suggest new ideas and discuss issues.

# License

GNU Lesser General Public License (LGPL) Version 2.1.

See [LICENSE](LICENSE).
