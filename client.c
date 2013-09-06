/*
 * =====================================================================================
 *
 *       Filename:  cServer.c
 *
 *    Description:  The server of transter file
 *
 *        Version:  1.0
 *        Created:  09/06/2013 09:55:40 AM
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
	int sockfd;
	char buffer[1024];
	struct sockaddr_in server_addr;
	struct hostent *host;
	int port_number, nbytes;

	if (argc != 5) {
		fprintf(stderr, "Usage:%s hostname port_number destinationPath sourcePath\a\n", argv[0]);
		exit(1);
	}
	
	if ((host=gethostbyname(argv[1])) == NULL) {
		fprintf(stderr, "Gethostname error\n");
		exit(1);
	}

	if ((port_number = atoi(argv[2])) <= 0) {
		fprintf(stderr, "Usage:%s hostname port_number destinationPath sourcePath\a\n", argv[0]);
		exit(1);
	}

	/* build socket descriptor  */
	if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Socket error:%s\a\n", strerror(errno));
		exit(1);
	}

	/* initialize server socket descriptor  */
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port_number);
	server_addr.sin_addr=*((struct in_addr *)host->h_addr);

	/* connect to server  */
	if (connect(sockfd, (struct sockaddr *)(&server_addr), 
		sizeof(struct sockaddr)) == -1) {
		fprintf(stderr, "Connect error:%s\a\n", strerror(errno));
		exit(1);
	}

	file_sender(sockfd, argv[3], argv[4]);

	close(sockfd);
	exit(0);
}
