/*******************
客户端：

*******************/

/**************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
/**************************************************/
#include <sys/types.h>
#include <sys/socket.h>
//#include <linux/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define	LISTEN_NUM	10
#define	PORT		8888
#define SER_IP		"192.169.0.109"

int main(int argc, char *argv[])
{
	int Socketfd = -1;
	int ret = -1;
	char buf[1024] = "Hello unix network communication!";

	struct sockaddr_in Server_addr;
	struct sockaddr_in Client_addr;

	/********************* socket **********************/
	Socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (Socketfd < 0)
	{
		printf("Client: socket create failed!\n");
		return -1;
	}

	memset(&Server_addr, 0, sizeof(Server_addr));
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.s_addr = inet_addr(SER_IP);
	Server_addr.sin_port = htons(PORT);

	/********************* connect **********************/
	ret = connect(Socketfd, (struct sockaddr *)&Server_addr, sizeof(Server_addr));
	if (ret < 0)
	{
		printf("Client: connect failed!\n");
		close(Socketfd);
		return -1;
	}
	
	printf("Client: connect success!\n");

	write(Socketfd, buf, strlen(buf)+1);
	printf("Client: buf = %s\n", buf);
	printf("Client: port: %d\n", ntohs(Client_addr.sin_port));

	close(Socketfd);

	return 0;
}

