QTerm --- BBS client based on Qt
============

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
- Qt 4.6+ or Qt 5.0+
- CMake 2.8.6+
- OpenSSL (Optional)
- DBus (Optional)
- Phonon (Optional)
- KWallet (Optional)

To build,

::

    # Using Qt4
    cmake .
    # Using Qt5
    cmake . -DQT5=YES
    make

Development
-----------
Repository is at https://github.com/qterm/qterm


License
-------
QTerm is released under GNU General Public License (refer to COPYING).


Credits
-------
Kudos to our users for their bug reports and suggestions.