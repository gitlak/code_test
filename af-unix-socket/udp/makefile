
.PHONY : all
all : server client
server : server.c
	gcc -o server server.c
client : client.c
	gcc -o client client.c
clean:
	-rm server client

#要生成多个可执行文件，需要用PHONY：all实现