#
# Usage:  make -f proj05.makefile
# Makes:  proj05
#

proj05: proj05.student.o
	g++ -lpthread proj05.student.o  -o proj05

proj05.student.o:
	g++ -std=c++11 -Wall -c proj05.student.c

clean:
	rm -f proj05.student.o
	rm -f proj05
