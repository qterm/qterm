QTerm --- BBS client based on Qt
================================

QTerm is a BBS (Bulletin Board System) client program. It runs on GNU/Linux,
OS X and Windows thanks to the cross-platform framework Qt.


History
-------
Back in year 2000, as a desktop alternative, Linux started to get popular 
among enthusiasts. People need the software they used to have on Windows. 
A BBS client was one of those that university students wanted most.

In Nov.2 2000, kafa and smartfish released miniTerm at smth (now newsmth) BBS.
It was an absolutly exciting experience to view BBS from its mini window.

In 2001, having different vision from being "mini", QTerm forked the code to
become a fully functional BBS client such as S-Term and CTerm.


Build
-----

The following dependencies must be met,

- A basic development environment

  - On Debian, the package is
    ::

      sudo aptitude install build-essentials

  - On Fedora, the package is
    ::

      sudo dnf install @"C Development Tools and Libraries"

- Qt 5 or 6

  - On Debian, the Qt5 packages are
    ::

      sudo apt install qt5-defaults qttools5-dev qttools5-dev-tools qtscript5-dev qtmultimedia5-dev

    the Qt6 packages are
    ::

      sudo apt install qt6-base-dev qt6-multimedia-dev qt6-tools-dev qt6-documentation-tools

  - On Fedora, the Qt5 packages are
    ::

      sudo dnf install qt5-devel

  - The installer from Qt is simple for macOS and Windows.

- CMake 3.11+

  - On Linux and macOS, the package is most probably *cmake*.

- OpenSSL 1.0+ (Optional)

  - On Debian, the package is *libssl-dev*.
  - On Fedora, the package is *openssl-devel*.
  - On macOS, install *openssl* via MacPorts or HomeBrew
  - On Windows, get the installer from https://slproweb.com/products/Win32OpenSSL.html

- DBus (Optional)
- Phonon (Optional)
- KWallet (Optional)

To build on Linux or macOS, ::

    # Create the build directory side by side to qterm source directory
    mkdir -p qterm-build && cd qterm-build
    # If your Qt is under non-standard location, specify the qmake program
    cmake ../qterm -DCMAKE_PREFIX_PATH=<qt_cmake_dir>
    # Build
    make
    # Optionally create a binary package
    make package

To build on Windows, open the Visual Studio Developer Command Prompt, ::

    # Set PATH to include Qt binary directory
    set PATH=C:\Qt\5.7\msvc2015\bin;%PATH%
    # Create the build directory side by side to qterm source directory
    mdkir qterm-build
    cmake ..\qterm -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles"
    # Build
    nmake
    # Optionally create an installer if NSIS is installed
    nmake package

Development
-----------
Repository is at https://github.com/qterm/qterm


License
-------
QTerm is released under GNU General Public License (refer to COPYRIGHT).


Credits
-------
Kudos to our users for their bug reports and suggestions.
