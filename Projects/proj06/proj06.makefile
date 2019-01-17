#
# Usage:  make -f proj06.makefile
# Makes:  proj06
#

proj06: proj06.student.o
	g++ proj06.student.o  -o proj06

proj06.student.o:
	g++ -std=c++11 -Wall -c proj06.student.c

clean:
	rm -f proj06.student.o
	rm -f proj06
