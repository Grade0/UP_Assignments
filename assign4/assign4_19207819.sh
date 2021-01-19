#!/bin/bash
#Name: Chen Davide
#Student number: 19207819
#email: davide.chen@ucdconnect.ie
#notes: the script creates a new directory named as input and uses a recursive function for 
#interating through the source directory, duplicating subdirectories, 
#copying and converting *.png files

if [ ! $# -eq 2 ]; then 
    echo -e "\nERROR: Script needs 2 arguments:\n$0 source/directory/ target/directory/\n"
    exit
fi

function recursiveDuplication {
    for file in `ls $1`; do
        if [ -d $1/$file ] && [ ! -d $2/$file ]; then
            mkdir $2/$file
            recursiveDuplication $1/$file $2/$file
        elif [[ $1/$file == *.png ]]; then
            convert "$1/$file" "$2/${file%.png}.jpg"
        fi
    done
}

if [ ! -d $1 ]; then 
    echo -e "\nERROR: $1 does not exist\n"
    exit
elif [ -d $2 ] && [ ! -w $2 ]; then
    echo -e "\nERROR: You do not have permission to write to $2\n"
    exit
elif [ ! -d $2 ]; then
    echo -e "\nSUCCESS: $2 has been created"
    mkdir $2
fi

recursiveDuplication $1 $2