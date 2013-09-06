/*
 * =====================================================================================
 *
 *       Filename:  transfer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2013 11:13:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shuting Chen (cst), cicy0125@gmail.com
 *        Company:  ucas.ict.bwstor
 *
 * =====================================================================================
 */

#include "transfer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

int sender(int sockfd, FILE *fd, unsigned int size)
{
	int status = ERROR;
	char buffer[BUF_SIZE];
	char cmd[CMD_SIZE];
	int bytes = 0;
	int currentSendBytes = 0;

	if (sockfd < 0 || fd == NULL || size <= 0) {
		goto out;
	}
	
	while (currentSendBytes < size) {
		if((bytes = fread(buffer, 1, BUF_SIZE, fd)) <= 0) {
			printf("buffer:>%s\n", buffer);
			printf("Read finished!\n");
			break;
		}
		if ((bytes = write(sockfd, buffer, bytes)) < 0) {
			fprintf(stderr, "Write buffer failed!\n");
			goto out;
		}
		currentSendBytes += bytes;
	}
	printf("Finish transfer file!\n");

out:
	return status;
}

int file_sender(int sockfd, char *dpath, char *spath)
{
	int sendBytes = 0;
	int status = ERROR;
	FILE *fileIn = NULL;
	transfer_params_t params;

	if (dpath == NULL || spath == NULL) {
		goto out;
	}

	//receive status
	if (read(sockfd, &status, sizeof(int)) < 0) {
		fprintf(stderr, "Read data failed:%s\n", strerror(errno));
		goto out;
	}

	/* open file */
	if ((fileIn= fopen(spath, "rt")) == NULL) {
		fprintf(stderr, "Open file failed:%s\n", strerror(errno));
		goto out;
	}
	strcpy(params.path, dpath);
	fseek(fileIn, 0, SEEK_END);
	params.size = ftell(fileIn);
	fseek(fileIn, 0, SEEK_SET);

	if (write(sockfd, &params, sizeof(transfer_params_t)) < 0) {
		fprintf(stderr, "Write data failed:%s\n", strerror(errno));
		goto close_file;
	}

	//receive status
	if (read(sockfd, &status, sizeof(int)) < 0) {
		fprintf(stderr, "Read data failed:%s\n", strerror(errno));
		goto close_file;
	}

	if (status != SUCCESS) {
		goto close_file;
	}
	status = sender(sockfd, fileIn, params.size);

	if (status == SUCCESS) {
		if (read(sockfd, &status, sizeof(int)) < 0) {
			status = ERROR;
			fprintf(stderr, "Read data failed:%s\n", strerror(errno));
		}
	}
close_file:
	fclose(fileIn);
out:
	return status;
}

int receiver(int sockfd, FILE *fd, unsigned int size)
{
	int status = ERROR;
	char buffer[BUF_SIZE];
	int currentSendBytes = 0;
	int bytes = 0;

	if (sockfd < 0 || fd == NULL || size <= 0) {
		goto out;
	}

	while (currentSendBytes < size) {
		memset(buffer,'\0', BUF_SIZE);
		if ((bytes = read(sockfd, buffer, BUF_SIZE)) <= 0) {
			fprintf(stderr, "Read file data failed!:%s\n", strerror(errno));
			goto out;
		}
		if (fwrite(buffer, 1, bytes, fd) <= 0) {
			fprintf(stderr, "Write data to file failed!\n", strerror(errno));
			goto out;
		}

		currentSendBytes += bytes;
	}
	printf("Finish receive file!\n");

	status = SUCCESS;

out:
	return status;
}

int file_receiver(int sockfd)
{
	transfer_params_t params;
	FILE *fileOut;
	int status = ERROR;
	int write_status = SUCCESS;

	if (write(sockfd, &write_status, sizeof(int)) < 0) {
		fprintf(stderr, "Connect success write status failed:%s\n",strerror(errno));
		goto out;
	}
	
	if (read(sockfd, &params, sizeof(transfer_params_t)) < 0) {
		fprintf(stderr, "Read command failed:%s\n",strerror(errno));
		goto out;
	}
	
	if ((fileOut= fopen(params.path, "wt+")) == NULL) {
		if (write(sockfd, ERROR, sizeof(int)) < 0) {
			fprintf(stderr, "Write status open dir failed:%s\n",strerror(errno));
		}
		goto out;
	}

	if (write(sockfd, &write_status, sizeof(int)) < 0) {
		fprintf(stderr, "Write status open dir success failed:%s\n",strerror(errno));
		goto out;
	}
	status = receiver(sockfd, fileOut, params.size);
	

	if (status == SUCCESS ) {
		if (write(sockfd, &write_status, sizeof(int)) < 0) {
			status = ERROR;
			fprintf(stderr, "Write status finish failed:%s\n",strerror(errno));
		}
	} else {
		write_status = ERROR;
		if (write(sockfd, &write_status, sizeof(int)) < 0) {
			fprintf(stderr, "Write status finish failed:%s\n",strerror(errno));
		}
	}
	
close_file:
	fflush(fileOut);
	fclose(fileOut);
out:
	return status;
}
