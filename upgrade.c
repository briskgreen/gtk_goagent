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
	http_head_add(http,"Host: github.com\n");
	http_head_add(http,"Accept: */*\n");
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
	is_upgrade_goagent(res);

	printf("OK\n");
	gtk_init(NULL,NULL);
	message_box(NULL,_("Your GoAgent is the newest version now!"));

	return 0;
}
