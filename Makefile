main: dbserver dbclient
	echo "Done compiling"

dbserver: dbserver.c
	gcc -o dbserver dbserver.c

dbclient: dbclient.c
	gcc -o dbclient dbclient.c