/*
 * =====================================================================================
 *
 *       Filename:  transfer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2013 11:09:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shuting Chen (cst), cicy0125@gmail.com
 *        Company:  ucas.ict.bwstor
 *
 * =====================================================================================
*/

#include <stdio.h>

#define SUCCESS 1
#define ERROR 0
#define MAX_PATH_LEN 150
#define BUF_SIZE 256
#define CMD_SIZE 32


typedef struct transfer_params{
	char path[MAX_PATH_LEN];
	unsigned int size;
}transfer_params_t;


int sender(int sockfd, FILE *fd, unsigned int size);
int file_sender(int sockfd, char *dpath, char *spath);
int receiver(int sockfd, FILE *fd, unsigned int size);
int file_receiver(int sockfd);
