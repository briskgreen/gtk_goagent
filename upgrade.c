#include "autoupgrade.h"

/*手动更新进程进入点*/
int main(int argc,char **argv)
{
	CURL *curl;
	RET_DATA ret;
	char *download_url;
	int is_upload;
	char *goagent_version;

/*	HTTP *http;
	char *res;
	char *host;
	char *proxy=PROXY;
	int port;

	http=http_head_init();
	http_head_add(http,argv[1]);
	http_head_add(http,"Host: github.com\n");
	http_head_add(http,"Accept: *//**\n");
	http_head_add(http,"Connection: close\n\n");

	host=match_string(".[^:]*",proxy);
	port=atoi(proxy+strlen(host)+1);
	goagent_version=get_version(argv[2]);
	res=https_perform(http,host,port,NULL,NULL);
	free(host);
	printf("OK\n");
	if(res == NULL)
		return -1;
	printf("OK\n");
	is_upgrade_goagent(res);*/
	curl=curl_easy_init();
	ret.data=NULL;
	ret.len=0;
	goagent_version=get_version(argv[2]);

	//获取Cookies
	curl_easy_setopt(curl,CURLOPT_URL,"https://www.google.com/");
	curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"/dev/null");
	curl_easy_setopt(curl,CURLOPT_COOKIEFILE,"/dev/null");
	curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_easy_setopt(curl,CURLOPT_NOBODY,1);
	curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);
	curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_perform(curl);

	curl_easy_setopt(curl,CURLOPT_NOBODY,0);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,get_data);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,&ret);
	curl_easy_setopt(curl,CURLOPT_URL,argv[1]);
	curl_easy_perform(curl);

	if(ret.len)
	{
		if(download_url=is_upgrade_goagent(&ret,goagent_version))
		{
			free(ret.data);
			ret.data=NULL;
			ret.len=0;

			curl_easy_setopt(curl,CURLOPT_URL,GOAGENT_HIS_URL);
			curl_easy_perform(curl);
			is_upload=goagent_is_upload(&ret);
			download_file(download_url,is_upload);

			free(download_url);
		}

		free(ret.data);
	}
	curl_easy_cleanup(curl);
	free(goagent_version);

	printf("OK\n");
	gtk_init(NULL,NULL);
	message_box(NULL,_("Your GoAgent is the newest version now!"));
	kill(getpid(),SIGKILL);

	return 0;
}
