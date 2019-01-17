#
# Usage:  make -f proj02.makefile
# Makes:  proj02
#

proj02: proj02.student.o
	g++ proj02.student.o -o proj02

proj02.student.o:
	g++ -std=c++11 -Wall -c proj02.student.cpp

clean:
	rm -f proj02.student.o
	rm -f proj02
