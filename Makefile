#executable#
dazibao:  projet.c
	#gcc projet.c -Wall -o projet  
	gcc projet.c -W -o projet 

clean: 
	rm projet
