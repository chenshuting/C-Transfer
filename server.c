/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  The server of file transfer
 *
 *        Version:  1.0
 *        Created:  09/06/2013 09:56:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shuting Chen (cst), cicy0125@gmail.com
 *        Company:  ucas.ict.bwstor
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "transfer.h"

int main(int argc, char *argv[]) 
{
	int sockfd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size, port_number;

	if (argc != 2) {
		fprintf(stderr, "Usage:%s port_number\a\n", argv[0]);
		exit(1);
	}

	if((port_number = atoi(argv[1])) < 0){
		fprintf(stderr, "Usage:%s port_number\a\n", argv[0]);
		exit(1);
	}

	/* build socket descriptor  */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));
		exit(1);
	}

	/* initialize server's socket descriptor  */
	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port=htons(port_number);
	
	/* binding */
	if (bind(sockfd, (struct sockaddr *)(&server_addr), 
		sizeof(struct sockaddr)) == -1) {
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		exit(1);
	}

	/* listen  */
	if (listen(sockfd, 5) == -1) {
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
		exit(1);
	}
	
	while (1) {
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd=accept(sockfd, (struct sockaddr *)(&client_addr),
			&sin_size)) == -1) {
			fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
			exit(1);
		}
		fprintf(stderr, "Server get connetion from %s\n",
			inet_ntoa(client_addr.sin_addr));
		inet_ntoa(client_addr.sin_addr);

		file_receiver(new_fd);
		
		close(new_fd);
	}
	close(sockfd);

	return 0;
}
