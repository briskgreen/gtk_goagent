#include "autoupgrade.h"

void error_quit(const char *msg);
char *get_version(char *path);
size_t is_upgrade_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream);
size_t is_upgrade_gtk_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream);

void download_file(char *path,char *is_upload);

char *goagent_version;
char *gtk_goagent_version;

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

	goagent_version=get_version(conf.proxy_py_path);
	gtk_goagent_version=get_version("readme");

	if(strcmp(argv[1]"1")==0)
		auto_upgrade_goagent(GOAGENT_URL);
	else if(strcmp(argv[1],"2")==0)
		auto_upgrade_gtk_goagent(GTK_GOAGENT_URL);
	else if(strcmp(argv[1],"3")==0)
	{
		auto_upgrade_goagent(GOAGENT_URL);
		auto_upgrade_gtk_goagent(GTK_GOAGENT_URL);
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
	int index;

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

	for(i=0;buf[i];++i)
		if(buf[i] >= '0' && buf[i] <='9')
			break;
	index=i;

	for(len=0;buf[i];++len,++i);
	--len;

	if(buf[i-2] < '0' || buf[i-2] >'9')
		--len;

	version=malloc(len+1);
	bzero(version,len+1);

	for(j=0;len;++index,++j,--len)
		version[j]=buf[index];

	return version;
}

void auto_upgrade_goagent(char *url)
{
	if(fork()==0)
	{
		CURL *curl;

		curl=curl_easy_init();
		curl_easy_setopt(curl,CURLOPT_URL,url);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,
				is_upgrade_goagent);
		curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);

		while(1)
		{
			curl_easy_perform(curl);
			sleep(10*60);
		}
	}
}

void auto_upgrade_gtk_goagent(char *url)
{
	if(fork()==0)
	{
		CURL *curl;

		curl=curl_easy_init();
		curl_easy_setopt(curl,CURLOPT_URL,url);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,
				is_upgrade_gtk_goagent);
		curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);

		while(1)
		{
			curl_easy_perform(curl);
			sleep(10*60);
		}
	}
}

size_t is_upgrade_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream)
{
	char buf[40];
	char path[100];
	int i,j;

	if(strstr(ptr,"https://nodeload.github.com"))
	{
		bzero(buf,sizeof(buf));

		for(i=0;ptr[i];++i)
		{
			if(ptr[i] == '"')
			{
				bzero(temp,sizeof(temp));

				snprintf(temp,39,"%s",ptr+i);

				if(strstr(temp,"https://nodeload.github.com"))
					goto ok;
			}
		}
ok:
		if(ptr[i] == '\0')
			return 0;

		for(++i,j=0;ptr[i] != '"';++i,++j)
			path[j]=ptr[i];
	}

	if(strstr(ptr,"最近更新") && strstr(ptr,"</li><li><tt>"))
	{
		bzero(buf,sizeof(buf));

		for(i=0;ptr[i] != '[';++i);
		for(j=0;ptr[i-1] != ']';++i,++j)
			buf[j]=ptr[i];

		buf[j]=' ';

		for(++j;ptr[i] != ' ';++i);
		for(++i;ptr[i] != ' ';++i,++j)
			buf[j]=ptr[i];

		if(!strstr(buf,goagent_version))
		{
			gtk_init(NULL,NULL);
			if(message_box_ok(_("Have New Version GoAgent Do You Want To Upgrade Now?")))
				download_file(path,buf);
			else
				return 0;
		}
		else
			return 0;
	}

	return nmebm;
}

void download_file(char *path,char *is_upload)
{
	CURL *curl;
	FILE *fp;

	gtk_init(NULL,NULL);

	if((fp=fopen("/tmp/$goagent&","w"))==NULL)
	{
		message_box(NULL,strerror(errno));
		return;
	}

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,path);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
	curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	if(strstr(is_upload,"是"))
		message_box(NULL,_("This Version Should Upload Again"));
}
