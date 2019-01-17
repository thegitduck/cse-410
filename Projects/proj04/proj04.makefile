#
# Usage:  make -f proj04.makefile
# Makes:  proj04
#

proj04: proj04.student.o
	g++ proj04.student.o -o proj04

proj04.student.o:
	g++ -std=c++11 -Wall -c proj04.student.c

clean:
	rm -f proj04.student.o
	rm -f proj04
