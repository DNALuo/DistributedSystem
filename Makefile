all: SERVER CLIENT

clean:
	rm -f client server *.o msg_clnt.c msg_svc.c msg_xdr.c msg.h

.PHONY:
	all clean

genmsg: msg.x
	rpcgen msg.x

CLIENT: genmsg hello_world_client.c
	gcc -o client hello_world_client.c msg_clnt.c

SERVER: genmsg hello_world_server.c
	gcc -o server hello_world_server.c msg_svc.c
