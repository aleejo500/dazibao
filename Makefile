#executable#
gloutont:  glouton.c 
	gcc glouton.c -o glouton -Wall

clean: 
	rm glouton 
