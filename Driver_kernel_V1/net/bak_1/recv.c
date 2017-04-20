/*******************
服务器端：

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


int main(int argc, char *argv[])
{
	int Socketfd = -1;
	int ret = -1;
	char buf[1024] = {0};

	struct sockaddr_in Server_addr;
	struct sockaddr_in Client_addr;

	/********************* socket **********************/
	Socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (Socketfd < 0)
	{
		printf("Server: socket create failed!\n");
		return -1;
	}

	memset(&Server_addr, 0, sizeof(Server_addr));
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	Server_addr.sin_port = htons(PORT);

	/********************* bind **********************/
	ret = bind(Socketfd, (struct sockaddr *)&Server_addr, sizeof(Server_addr));
	if (ret < 0)
	{
		printf("Server: bind failed!\n");
		close(Socketfd);
		return -1;
	}
	
	/********************* listen **********************/
	ret = listen(Socketfd, LISTEN_NUM);
	if (ret < 0)
	{
		printf("Server: add listen failed!\n");
		close(Socketfd);
		return -1;
	}

	/********************* accept **********************/
	int fd = -1;
	int accept_len = sizeof(Client_addr);

	while(1)
	{
		fd = accept(Socketfd, (struct sockaddr *)&Client_addr, &accept_len);
		if (fd < 0)
		{
			printf("Server: accept failed!\n");
			close(Socketfd);
			return -1;
		}

		memset(buf, 0, sizeof(buf));
		read(fd, buf, sizeof(buf));
		printf("Server: fd = %d, buf = %s\n", fd, buf);
		printf("Server: ip: %s, port: %d\n", inet_ntoa(Client_addr.sin_addr), ntohs(Client_addr.sin_port));
	}


	close(Socketfd);

	return 0;
}

