#include "autoupgrade.h"

/*手动更新进程进入点*/
int main(int argc,char **argv)
{
	HTTP *http;
	char *res;
	char *host;
	char *proxy=PROXY;
	int port;

	http=http_head_init();
	http_head_add(http,argv[1]);
	http_head_add(http,"Host: code.google.com\n");
	http_head_add(http,"Accept: */*\n");
	http_head_add(http,"Connection: close\n\n");

	host=match_string(".[^:]*",proxy);
	port=atoi(proxy+strlen(host)+1);
	goagent_version=get_version(argv[2]);
	res=http_perform(http,host,port);
	free(host);
	if(res == NULL)
		return -1;
	is_upgrade_goagent(res);
	free(res);

	return 0;
}
