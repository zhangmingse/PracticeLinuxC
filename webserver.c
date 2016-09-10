#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include"socklib.h"
void read_til_crnl(FILE * fp)
{
	char buf[BUFSIZ];
	while(fgets(buf,BUFSIZ,fp) != NULL && strcmp(buf,"\r\n")!=0);
}

void header(FILE * fp,char * content_type)
{
	fprintf(fp,"HTTP/1.0 200 OK \r\n");
	if(content_type)
		fprintf(fp,"Content-type:%s\r\n",content_type);
}

void cannot_do(int fd)
{
	FILE * fp = fdopen(fd,"w");
	fprintf(fp,"HTTP/1.0 501 Not Implemented\r\n");
	fprintf(fp,"Content-type:text/plain\r\n");
	fprintf(fp,"\r\n");
	fprintf(fp,"That command is not yet implemented\r\n");
	fclose(fp);
}

void do_404(char * item,int fd)
{
	FILE * fp = fdopen(fd,"w");
	
	fprintf(fp,"HTTP/1.0 404 Not Found\r\n");
	fprintf(fp,"Content-type:text/plain\r\n");
	fprintf(fp,"\r\n");
	fprintf(fp,"The item you requested:%s \r\nisnot found\r\n",item);
	fclose(fp);
}

bool isadir(char * f)
{
	struct stat info;
	return(stat(f,&info)!=-1 && S_ISDIR(info.st_mode));
}

bool not_exist(char *f)
{
	struct stat info;
	return(stat(f,&info) == -1);
}

void do_ls(char *dir,int fd)
{
	FILE * fp;
	fp = fdopen(fd,"w");
	header(fp,"text/plain");
	fprintf(fp,"\r\n");
	fflush(fp);

	dup2(fd,1);
	dup2(fd,2);
	close(fd);
//	fprintf(stdout,"doing ls\n");
//	fprintf(stdout,"doing ls 2\n");
	execlp("ls","ls","-l",dir,NULL);
	perror(dir);
	exit(1);
}

char *file_type(char * f)
{
	char * cp;
	if((cp = strchr(f,'.'))!=NULL)
		return cp+1;
	return "";
}

bool ends_in_cgi(char *f)
{
	return (strcmp(file_type(f),"cgi")==0);
}

void do_exec(char * prog,int fd)
{
	FILE * fp;
	fp = fdopen(fd,"w");
	header(fp,NULL);
	fflush(fp);
	dup2(fd,1);
	dup2(fd,2);
	close(fd);
	execl(prog,prog,NULL);
	perror(prog);
}

void do_cat(char * f,int fd)
{
	char * extension = file_type(f);
	char * content = "text/plain";
	FILE * fpsock,*fpfile;
	int c;
	if(strcmp(extension,"html") == 0)
		content = "text/html";
	else if(strcmp(extension,"gif")==0)
		content = "image/gif";
	else if(strcmp(extension,"jpg") == 0)
		content = "image/jpg";
	else if(strcmp(extension,"jpeg")==0)
		content = "image/jpeg";

	fpsock = fdopen(fd,"w");
	fpfile = fopen(f,"r");
	if(fpsock!=NULL && fpfile!=NULL)
	{
		header(fpsock,content);
		fprintf(fpsock,"\r\n");
		while((c=getc(fpfile))!=EOF)
			putc(c,fpsock);
		fclose(fpsock);
		fclose(fpfile);
	}
	exit(0);
}
void process_rq(char * rq,int fd)
{
	char cmd[BUFSIZ],arg[BUFSIZ];
	if(fork()!=0)
	{
		return;
	}
//	strcpy(arg,"./");
	if(sscanf(rq,"%s %s",cmd,arg) != 2)
		return;
	if(strcmp(cmd,"GET")!=0)
	{
		printf("missing GET\n");
		cannot_do(fd);
	}
	else if(not_exist(arg))
	{
		printf("%s not exist\n",arg);
		do_404(arg,fd);
	}
	else if(isadir(arg))
	{
		printf("%s is dir\n",arg);
		do_ls(arg,fd);
	}
	else if(ends_in_cgi(arg))
	{
		printf("ends in cgi\n");
		do_exec(arg,fd);
	}
	else
	{
		printf("do cat\n");
		do_cat(arg,fd);
	}

}

void main(int ac,char * args[])
{
	int sock,fd;
	FILE *fpin;
	char request[BUFSIZ];
	if(ac == 1)
	{
		fprintf(stderr,"usage:ws portnum\n");
		exit(1);
	}

	sock = make_server_socket(atoi(args[1]));
	if(sock == -1)
		exit(2);

	while(1)
	{
		fd = accept(sock,NULL,NULL);
		fpin = fdopen(fd,"r");

		fgets(request,BUFSIZ,fpin);
		printf("got a call:request = %s\n",request);
//		read_til_crnl(fpin);
		printf("begin to process request\n");
		process_rq(request,fd);
		printf("end of process request\n");
		fclose(fpin);
	}
}



