#!/bin/bash

function install_github()
{
	username=$1
	repo_name=$2
	cmake_options=$3
	base_path=`pwd`

	# Download and extract the source
	wget https://github.com/$username/$repo_name/archive/master.zip
	unzip -qq master.zip
	rm master.zip # Must be removed for the next library

	# Box2D have different directories structure than usual libraries on GitHub
	if [ $repo_name = "Box2D" ]
	then
		cd $repo_name-master/Box2D/Build
	else
		mkdir $repo_name-master/build
		cd $repo_name-master/build
	fi

	cmake $cmake_options ..
	make -i --quiet
	sudo make install -i --quiet
	cd $base_path # Don't forget to restart from the base path for the next library
}

install_github SFML SFML
install_github texus TGUI
install_github alecthomas entityx "-DENTITYX_BUILD_TESTING=False"
install_github erincatto Box2D "-DBOX2D_BUILD_EXAMPLES=False -DBOX2D_BUILD_SHARED=True -DBOX2D_BUILD_STATIC=False"

# Install Boost from sources
wget http://downloads.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.zip
unzip -qq boost_1_60_0.zip
cd boost_1_60_0
./bootstrap.sh --with-libraries=filesystem,system,test
sudo ./b2 -d0 install
