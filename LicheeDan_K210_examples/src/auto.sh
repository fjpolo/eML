#!/bin/sh
set -e
for dir in `ls .`
do
	if [ -d $dir ]
	then
		echo $dir
		rm -f $dir/$dir.bin

		if [ ! -d $dir/build/ ]
		then
			mkdir $dir/build
		fi
		cd $dir/build/ && cmake ../../../ && make && cp $dir.bin ../ && cd -
		if [ ! -f $dir/$dir.bin ]
		then
			echo ERROR: Unable to build $dir
			exit
		fi
	fi
done
set +e
