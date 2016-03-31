#!/bin/bash

function install_github()
{
	username=$1
	repo_name=$2
	install_box2d=$3

	wget https://github.com/$username/$repo_name/archive/master.zip
	unzip -qq master.zip
	rm master.zip
	if [ $install_box2d = "true" ]
	then
		cd $repo_name-master/Box2D/Build
	else
		mkdir $repo_name-master/build
		cd $repo_name-master/build
	fi
	cmake ..
	make -i --quiet
	sudo make install -i --quiet
}

install_github SFML SFML
install_github texus TGUI
install_github alecthomas entityx
install_github erincatto Box2D "true"

# Install Boost from sources
wget http://downloads.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.zip
unzip -qq boost_1_60_0.zip
cd boost_1_60_0
./bootstrap.sh --with-libraries=filesystem,system,test
sudo ./b2 -d0 install
