#!/bin/bash

locally="false"
libraries_directory="$PWD/libraries/"
cmake_arguments="-DCMAKE_BUILD_TYPE=Debug"
sudo="sudo"
# Select the boost version to download
boost_major=1
boost_minor=62
boost_patch=0

if [[ $# -eq 1 &&  ($1 = "-l" || $1 = "--locally") ]]
then
	locally="true"
fi

if [[ $locally = "true" ]]
then
	cmake_arguments="$cmake_arguments -DCMAKE_INSTALL_PREFIX='$libraries_directory'"
	sudo=""
fi

function install_github()
{
	username=$1
	repo_name=$2
	additional_cmake_arguments=$3
	base_path=`pwd`

	echo "Downloading $username/$repo_name"

	# Download and extract the source
	wget https://github.com/$username/$repo_name/archive/master.zip
	unzip -qq master.zip
	rm master.zip # Must be removed for the next library

	echo "Installing $username/$repo_name"
	# Box2D have different directories structure than usual libraries on GitHub
	if [[ $repo_name = "Box2D" ]]
	then
		cd $repo_name-master/Box2D/Build
	else
		mkdir $repo_name-master/build
		cd $repo_name-master/build
	fi
	cmake $cmake_arguments $additional_cmake_arguments  ..
	make --ignore-errors --quiet --jobs=4
	$sudo make install --ignore-errors --quiet --jobs=4
	# Don't forget to restart from the base path for the next library
	cd $base_path 
}

# Create the libraries directory if not already present
[ -d "$libraries_directory" ] || mkdir "$libraries_directory"
cd "$libraries_directory"

install_github SFML SFML
install_github texus TGUI
install_github alecthomas entityx "-DENTITYX_BUILD_TESTING=False"
install_github erincatto Box2D "-DBOX2D_BUILD_EXAMPLES=False -DBOX2D_BUILD_SHARED=True -DBOX2D_BUILD_STATIC=False -DCMAKE_CXX_FLAGS='-std=c++11'"

# Install Boost from sources
boost_under=${boost_major}_${boost_minor}_${boost_patch}
boost_dot=${boost_major}.${boost_minor}.${boost_patch}

wget http://downloads.sourceforge.net/project/boost/boost/$boost_dot/boost_$boost_under.zip
unzip -qq boost_$boost_under.zip
cd boost_$boost_under
./bootstrap.sh --with-libraries=test --prefix="$libraries_directory"
./b2 -d0 -j4 install

