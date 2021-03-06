#include "jnxfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#define __USE_XOPEN_EXTENDED
#include <ftw.h>
#include <libgen.h>
#include <assert.h>
size_t jnx_file_readb(char* path, char **buffer)
{
	FILE* fp;
	if ((fp = fopen(path, "rb")) == NULL) {
		perror("file: ");
		return 0;
	}
	if(fseek(fp, 0, SEEK_END) != 0)
	{
		perror("file: ");
		fclose(fp);
		exit(1);
	}
	size_t size = ftell(fp);
	rewind(fp);
	(*buffer) = calloc(size, sizeof(char));
	fread((*buffer), 1, size, fp);
	fclose(fp);
	return size;
}
size_t jnx_file_read(char* path, char **buffer)
{
	FILE* fp;
	if ((fp = fopen(path, "r")) == NULL) {
		perror("file: ");
		return 0;
	}
	if(fseek(fp, 0, SEEK_END) != 0)
	{
		perror("file: ");
		exit(1);
	}
	size_t size = ftell(fp);
	rewind(fp);
	(*buffer) = calloc(size, sizeof(char));
	fread((*buffer), 1, size, fp);
	fclose(fp);
	return size;
}
jnx_file_kvp_node* jnx_file_read_keyvaluepairs(char* path, char* delimiter) {
	FILE* file;
	if((file = fopen(path, "r+")) == NULL)
	{
		printf("Could not open file for KVP matching\n");
		return NULL;
	}
	jnx_file_kvp_node* list = NULL, ** nextp = &list;
	char buffer[1024];
	while (fgets(buffer, sizeof buffer, file) != NULL) {
		char *key = strtok(buffer,delimiter);
		char *value = strtok(NULL,delimiter);
		if(value == NULL) { continue; }
		jnx_file_kvp_node* node;
		node = malloc(sizeof(jnx_file_kvp_node));
		node->key = malloc(strlen(key) + 1);
		node->value = malloc(strlen(value) +1);
		strcpy(node->key,key);
		strcpy(node->value,value);
		if(node->value[strlen(node->value)-1] == '\n'){
			node->value[strlen(node->value) -1] = 0;
		}
		node->next = NULL;
		*nextp = node;
		nextp = &node->next;
	}
	fclose(file);
	return list;
}
size_t jnx_file_writeb(char* path, char* data, size_t data_size)
{
	FILE* fp;
	if ((fp = fopen(path, "wb")) == NULL) {
		perror("file: ");
		return -1;
	}
	size_t size = fwrite(data, 1,data_size, fp);
	fclose(fp);
	return size;
}
size_t jnx_file_write(char* path, char* data, size_t data_size)
{
	FILE* fp;
	if ((fp = fopen(path, "w")) == NULL) {
		perror("file: ");
		return -1;
	}
	size_t size = fwrite(data, 1,data_size, fp);
	fclose(fp);
	return size;
}
int jnx_file_recursive_delete_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	remove(fpath);
	return 0;
}
int jnx_file_recursive_delete(char* path, int depth)
{
	if( nftw(path,jnx_file_recursive_delete_callback, depth, FTW_DEPTH) != 0)
	{
		perror("jnx_file_recursive_delete");
		return -1;
	}	
	return 0;
}
