# The Lost Girl {#mainpage}

[![Build Status](https://travis-ci.org/TheoVerhelst/TheLostGirl.svg?branch=master)](https://travis-ci.org/TheoVerhelst/TheLostGirl)
[![codecov.io](https://codecov.io/github/TheoVerhelst/TheLostGirl/coverage.svg?branch=master)](https://codecov.io/github/TheoVerhelst/TheLostGirl?branch=master)

## Synopsis
The Lost Girl is an archery role-playing game represented in scrolling 2D.<br/>
You took place in a huge world full of possibilities and you can play up to 4 differents characters. Alot of amazing and well constructed quests are proposed to you, but you are totally free of your actions. You want to kill the most important character of the whole country? No problem, you'll just have to assume yourself.

## Installation

Run the following commands in a terminal in the main directory:<br/>

    ./install_deps.sh
    mkdir build
    cd build
    cmake ..
    make

And then go have a coffee.<br/>
The executable is now available by running `./TheLostGirl`.<br/>
For now, the compilation was tested only under Linux.<br/>
If you have problems with steps above, send me an email.

## API Reference

Run the following commands in a terminal once you are in the main directory:<br/>

    cd doxygen
    doxygen
    sensible-browser html/index.html

For now this was tested only under Linux.

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
