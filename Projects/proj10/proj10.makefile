#
# Usage:  make -f proj09.makefile
# Makes:  proj10
#

proj10: proj10.student.o
	g++ proj10.student.o  -o proj10

proj10.student.o:
	g++ -std=c++11 -Wall -c proj10.student.c

clean:
	rm -f proj10.student.o
	rm -f ./proj10
