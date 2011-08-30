#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

CURL *curl;
CURLcode res;
FILE * fd;

size_t musicsize=0; 
size_t realsize=0;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	static int i = 0;
	
	realsize += fwrite(ptr,size,nmemb,(FILE*)stream);
	musicsize+=nmemb*size;
	return size*nmemb;
}

static char *down_file(char *filename)
{
	curl_easy_setopt(curl, CURLOPT_URL, filename); 				
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);	
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);

	res = curl_easy_perform(curl);					
	if(CURLE_OK != res) return NULL;		
	return NULL;
}

int download(char *url)
{
	curl = curl_easy_init();							
	char *result;
	char *file_name;
	
	file_name = strrchr(url, '/');
	file_name += 1;
	printf("file name : %s\n", file_name);

	fd = fopen(file_name,"w+");
	down_file(url);
	fclose(fd);

	printf("musicsize=%d,realsize=%d\n",musicsize,realsize);
	curl_easy_cleanup(curl);

	return 0;
}



