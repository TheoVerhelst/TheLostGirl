# The Lost Girl {#mainpage}

[![Build Status](https://travis-ci.org/TheoVerhelst/TheLostGirl.svg?branch=master)](https://travis-ci.org/TheoVerhelst/TheLostGirl)
[![codecov.io](https://codecov.io/github/TheoVerhelst/TheLostGirl/coverage.svg?branch=master)](https://codecov.io/github/TheoVerhelst/TheLostGirl?branch=master)

## Synopsis
The Lost Girl is an archery role-playing game represented in scrolling 2D.<br/>
You are in a huge world full of possibilities and you can play up to 4
differents characters. Alot of amazing and well constructed quests are proposed
to you, but you are totally free to do anything you want. You want to kill the
most important character of the whole country? No problem, you'll just have to
assume yourself.

## Installation

Run the following commands in a terminal in the main directory:<br/>

    ./install_deps.sh
    mkdir build
    cd build
    cmake ..
    make

And then go have a coffee.<br/>
The executable is now available by running `./TheLostGirl`.<br/>
The CMake commands should be cross-plateform, but the depencencies script is a
shell script that works only under Linux.

## API Reference

In order to build the documentation, just set `TLG_BUILD_DOC` to true when
running `cmake`:<br/>

    cmake -DTLG_BUILD_DOC=True ..

instead of

    cmake ..

Once the compilation is done (with `make`), the documentation is available with
the following command:

    sensible-browser doxygen/html/index.html

You'll need Doxygen to be installed on your computer in order to build the
documentation.

## Contributors

Code: Théo Verhelst (<Theo.Verhelst@ulb.ac.be>)<br/>
Graphics: Jeremy Wimberg

## External libraries

* SFML: <http://www.sfml-dev.org/><br/>
* TGUI: <https://www.tgui.eu/><br/>
* Box2D: <http://www.box2d.org/><br/>
* Entityx: <https://github.com/alecthomas/entityx/><br/>
* Json-cpp: <https://github.com/open-source-parsers/jsoncpp><br/>
* Boost: <http://www.boost.org/><br/>
