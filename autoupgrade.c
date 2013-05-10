#include "autoupgrade.h"

void error_quit(const char *msg);
char *get_version(char *path);

int main(int argc,char **argv)
{
	if(argc!=2)
		return -1;

	CONFDATA conf;
	FILE *fp;

	//gtk_init(&argc,&argv);

	if((fp=open_config(&conf))==NULL)
		error_quit("Open Conf");

	//close_config(fp,&conf);
	fclose(fp);

	/*if((fp=fopen(".upgrade.log","r"))==NULL)
	{
		if((fp=fopenh(".upgrade.log","w"))==NULL)
			error_quit("Can Not Create Upgrade Log File");

		fputs(get_version(conf.proxy_py_path),fp);
		fputs(get_version("readme"),fp);
		fclose(fp);
	}*/

	if(strcmp(argv[1]"1")==0)
		auto_upgrade_goagent(GOAGENT_URL,conf.goagent_path,
				get_version(conf.proxy_py_path));
	else if(strcmp(argv[1],"2")==0)
		auto_upgrade_gtk_goagent(GTK_GOAGENT_URL,
				conf.gtk_goagent_path,get_version(
					"readme"));
	else if(strcmp(argv[1],"3")==0)
	{
		auto_upgrade_goagent(GOAGENT_URL,conf.goagent_path,
				get_version(conf.proxy_py_path));
		auto_upgrade_gtk_goagent(GTK_GOAGENT_URL,
				conf.gtk_goagent_path,get_version(
					"readme"));
	}

	return 0;
}

void error_quit(const char *msg)
{
	perror(errno);
	exit(-2);
}

char *get_version(char *path)
{
	FILE *fp;
	char buf[512];
	char *version;
	int len,i,j;

	if((fp=fopen(path,"r"))==NULL)
		error_quit("Open");

	while(!feof(fp))
	{
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf)-1,fp);

		if(buf[0] == '#')
			continue;

		if(strstr(buf,"version"))
			break;
	}

	if(feof(fp))
		error_quit("Can Not Get Version");

	for(i=0;buf[i] >= '0' || buf[i] <= '9';++i);
	for(len=0;buf[i];++len);
	if(buf[i-1] <= '0' || buf[i-1] >='9')
		len-=1;

	version=malloc(len+1);
	bzero(version,len+1);

	for(i=0;buf[i] >= '0' || buf[i] <= '9';++i);
	for(j=0;len;++i,++j,--len)
		version[j]=buf[i];

	return version;
}
