#include <stdio.h>
#include <pty.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void read_and_put(int pty,char *appid,char *mail,char *passwd)
{
	fd_set reads;
	char buf[64];

	while(1)
	{
		FD_ZERO(&reads);
		FD_SET(pty,&reads);
		select(pty+1,&reads,NULL,NULL,NULL);

		bzero(buf,sizeof(buf));

		if(read(pty,buf,sizeof(buf)-1)==-1)
			break;
		printf("%s",buf);

		if(strstr(buf,"APPID:"))
			write(pty,appid,strlen(appid));
		if(strstr(buf,"Email:"))
			write(pty,mail,strlen(mail));
		if(strstr(buf,"Password for"))
			write(pty,passwd,strlen(passwd));
	}
}

int main(void)
{
	char *appid="********************************\n";
	char *mail="************************\n";
	char *passwd="******************\n";
	int pty;

	if(forkpty(&pty,NULL,NULL,NULL)==0)
		execl("/usr/bin/python","python","./uploader.zip",NULL);

	read_and_put(pty,appid,mail,passwd);

	return 0;
}
