#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

CURL *curl;
CURLcode res;
FILE * fd;

long musicsize=0; 
long realsize=0;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	static int i = 0;
	printf("write data : %d\n",i++);
	if (strlen((char *)stream) + strlen((char *)ptr) > 999999) return 0;
	//	strcat(stream, (char *)ptr);
	realsize += fwrite(ptr,size,nmemb,stream);

	//	printf("size=%d, nmemb=%d, size*nmemb=%d\n", size, nmemb, size*nmemb);
	//	printf("%s\n",(char*)stream);
	musicsize+=nmemb*size;
	return size*nmemb;
}

char *down_file(char *filename)
{
	static char str[10000000];
	strcpy(str,"");
	curl_easy_setopt(curl, CURLOPT_URL, filename); 				
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);	
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);			
	res = curl_easy_perform(curl);					
	str[9999999] = '\0';
	if(CURLE_OK != res) return NULL;		
	return str;
}

int main(int argc, char **argv)
{
	char url[200];
	curl = curl_easy_init();							
	char *result;
	char *file_name;
	
	if (argv[1] == NULL)  {
		printf("Please input music url.%s\n",argv[1]);
		fgets(url, 200, stdin);
	}
	else {
		strcpy(url, argv[1]);		
	}

	file_name = strrchr(url, '/');
	file_name += 1;
printf("file name : %s\n", file_name);

	fd = fopen(file_name,"w+");
	result = down_file(url);
	fclose(fd);

	printf("musicsize=%d,realsize=%d\n",musicsize,realsize);
	curl_easy_cleanup(curl);

	return 0;
}



















