#
# Usage:  make -f proj08.makefile
# Makes:  proj08
#

proj08: proj08.student.o
	g++ proj08.student.o  -o proj08

proj08.student.o:
	g++ -std=c++11 -Wall -c proj08.student.c

clean:
	rm -f proj08.student.o
	rm -f proj08
