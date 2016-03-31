#!/bin/bash

function install_github()
{
	username=$1
	repo_name=$2
	install_box2d=$3

	wget https://github.com/$username/$repo_name/archive/master.zip
	unzip master.zip
	rm master.zip
	if [ $install_box2d = "true" ]
	then
		cd $repo_name-master/Box2D/Build
	else
		mkdir $repo_name-master/build
		cd $repo_name-master/build
	fi
	cmake ..
	make -j2 -i
	sudo make install -i
}

install_github SFML SFML
install_github texus TGUI
install_github alecthomas entityx
install_github erincatto Box2D "true"

