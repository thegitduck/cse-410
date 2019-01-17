#
# Usage:  make -f proj03.makefile
# Makes:  proj03
#

proj03: proj03.student.o
	g++ proj03.student.o -o proj03

proj03.student.o:
	g++ -std=c++11 -Wall -c proj03.student.c

clean:
	rm -f proj03.student.o
	rm -f proj03
