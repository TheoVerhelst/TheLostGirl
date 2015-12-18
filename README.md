## Synopsis

The Lost Girl is an archery role-playing game represented in scrolling 2D.  
You took place in a huge world full of possibilities and you can play up to 4 differents characters. Alot of amazing and well constructed quests are proposed to you, but you are totally free of your actions. You want to kill the most important character of the whole country? No problem, you'll just have to assume yourself. 

## Installation
In order to compile The Lost Girl from sources, you have to install the following libraries:
* SFML version 2.3
* TGUI version 0.7-alpha2
* Box2D version 2.3
* Entityx version 1.0.0alpha1 or higher
* Boost version 1.57 or higher  
See below for download link of these librairies.
Json-cpp is already included.

Once you have installed the librairies, run the following commands in a terminal in the main directory:
```
cd bin
cmake ..
make
```
And then go have a coffee.  
The executable is now available by running `./TheLostGirl`.
For now, the compilation was tested only under Linux.  
If you have problems with steps above, send me an email.

## API Reference

Run the following commands in a terminal once you are in the main directory:  
```
cd doxygen
doxygen
sensible-browser html/index.html
```
For now this was tested only under Linux.

## Contributors

Code: Théo Verhelst (Theo.Verhelst@ulb.ac.be)  
Graphics: Jeremy Wimberg

## External libraries

SFML: http://www.sfml-dev.org/  
TGUI: https://www.tgui.eu/  
Box2D: http://www.box2d.org/  
Entityx: https://github.com/alecthomas/entityx/  
Json-cpp: https://github.com/open-source-parsers/jsoncpp  
Boost: http://www.boost.org/  

