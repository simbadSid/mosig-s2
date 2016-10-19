#!/bin/bash

echo "Testing File System"
echo
echo "The following tests will be executing using the nachos-filesys executable with the commands stated in the description"
echo
echo
echo "************************************************"
echo "Formating disk"
echo "command: -f"
echo "************************************************"
../build/nachos-filesys -f
echo
echo
echo "************************************************"
echo "Displaying new Root directory"
echo "command: -l"
echo "************************************************"
echo
../build/nachos-filesys -l
read -p "Press enter to continue... " -n1 -s
echo
echo
echo "************************************************"
echo "Creating new sub directory sub1"
echo "command: -mkdir sub1"
echo "************************************************"
echo
echo
echo "Current Directory State:"
../build/nachos-filesys -mkdir sub1 -l

read -p "Press enter to continue... " -n1 -s

echo "************************************************"
echo "Changing current directory to sub1:"
echo "command: -cd sub1"
echo "************************************************"
echo
echo
echo "Old current Directory State:"
../build/nachos-filesys -mkdir sub1 -l
echo
echo
echo "New current Directory State:"
../build/nachos-filesys -cd sub1 -l
echo
echo
echo "************************************************"
echo "Adding small file in sub1 directory from Unix to nachos"
echo "command -cd sub1 -cp testcp test"
echo "************************************************"
../build/nachos-filesys -cd sub1 -cp testcp test -l
echo
echo "comment: in the display, test is preceded by a '-' to show that this is a file and not a directory."
read -p "Press enter to continue... " -n1 -s
echo
echo "displaying content of file"
echo "command: -cd sub1 -D"
../build/nachos-filesys -cd sub1 -D
echo
read -p "Press enter to continue... " -n1 -s
echo
echo "************************************************"
echo "Removing test from sub1"
echo "command -cd sub1 -rm test"
echo "************************************************"
../build/nachos-filesys -cd sub1 -rm test
echo
echo "************************************************"
echo "display result"
echo "command -cd sub1 -l"
../build/nachos-filesys -cd sub1 -l
read -p "Press enter to continue... " -n1 -s

../build/nachos-filesys -D

read -p "Press enter to continue... " -n1 -s

echo "************************************************"
echo "Testing path Part 1: creation of a directory tree"
echo "command: -f then -mkdir bla -mkdir bli -cd bli -mkdir he -mkdir ho -cd ho -mkdir hi" 
echo "************************************************"
echo
echo
echo "tree representation :"
echo
echo
echo "       |---/bla"
echo "root---|          |---/he"
echo "       |---/bli---|"
echo "                  |---/ho---/hi"
echo
echo
read -p "Press enter to continue... " -n1 -s
echo
../build/nachos-filesys -f
../build/nachos-filesys -mkdir bla -mkdir bli -cd bli -mkdir he -mkdir ho -cd ho -mkdir hi
echo
echo
echo
echo
echo "we want to access /bli/ho"
echo "command: -cd /bli/ho -l should display ho with one entry (hi)."
../build/nachos-filesys -cd /bli/ho -l
echo
echo
read -p "Press enter to continue... " -n1 -s
echo
echo
echo "************************************************"
echo "Testing Big File allocation"
echo "command: -cp ../Makefile.define-origin make"
../build/nachos-filesys -f 
../build/nachos-filesys -cp ../Makefile.define-origin make
read -p "Press enter to continue... " -n1 -s
../build/nachos-filesys -D
