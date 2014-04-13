# The Emerging Plasmoid

Emerging plasmoid shows information about a ongoing emerge job:
- Which package it is building
- The current package build progress
- How many packages are on the build queue

It looks like this:

![Emerging Plasmoid showing kdevelop build](http://kde-look.org/CONTENT/content-pre1/147414-1.png)


## Build instructions

### Gentoo tree:
As root, type:

    emerge emerging-plasmoid

### Manual:
Download the latest source for Emerging Plasmoid

    cd /where/you/put/the/source
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix`
    make 
    make install

If you want to install system-wide, use `kde4-config --prefix` instead of `--localprefix`

Run `kbuildsycoca4` to make it available for you. You might have to restart your session or
the `plasma-desktop` process.