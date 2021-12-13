#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

int initServer();
void handClient(int fd);
int main(){
	//realize connection
	int serverFd = initServer();	
	int clientFd;
	//handle request
	while(1){
		//waiting for client's connection
		printf("1");
		clientFd = accept(serverFd,NULL,NULL);
		//handle the client request
		printf("there's host is connecting");
		handClient(clientFd);
		//close
		close(clientFd);
	
	}
	return 0;
}
int initServer(){
	//create socketfd
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == fd){
		perror("create socket failed!");
	}
	printf("create socket success!\n");
	//set server address suite
	struct sockaddr_in addr = {0};
	
	addr.sin_family = AF_INET;//same as the 1st parameter in function socket
	addr.sin_port = htons(80);//small end to big end 
	addr.sin_addr.s_addr = INADDR_ANY;//random network card
	//bond fd with address suite
	int r = bind(fd,(struct sockaddr*)&addr,sizeof addr);
	if(-1 == r){
		perror("bond faild!\n");
		close(fd);
		return -1;
	}
	printf("bond success!\n");
	//listen
	r = listen(fd,10);
	if(-1 == r){
		perror("listen faild!\n");
		close(fd);
		return -1;
	}
	printf("lisen success!\n");
	return fd;
}

void handClient(int fd){
	//accerpt the host's information
	char buff[1024*1024] = {0};
	int r = read(fd,buff,sizeof buff);
	if(r > 0)
		printf("the request is : %s\n",buff);

	//parsing the request
	char filename[20] = {0};
	sscanf(buff,"GET /%s",filename);
	printf("The filename of the parsed file:%s\n",filename);


	//get the type of the file and tell browser to send what file
	char* mime = NULL;
	if(strstr(filename,".html")){
		mime = "text/html";//text or html 
	}else if(strstr(filename,".jpg")){
		mime = "image/jpg";//image
	}


	char response[1024*1024] = {0}; //response head
	sprintf(response,"HTTP/1.1 200 ok\r\nContent-Type:%s\r\n\r\n",mime);

	int responseLen = strlen(response);
	int fileFd = open(filename,O_RDONLY);
	int fileLen = read(fileFd,responseLen+response,sizeof(response)-responseLen);

	write(fd,response,responseLen+fileLen);
	close(fileFd);
	sleep(1000);

}
