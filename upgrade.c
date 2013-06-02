#include "autoupgrade.h"

int main(int argc,char **argv)
{
	CURL *curl;
	char *p=getcwd(NULL,0);

	goagent_version=get_version(argv[2]);
	curl=curl_easy_init();

	curl_easy_setopt(curl,CURLOPT_URL,argv[1]);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,is_upgrade_goagent);
	curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);

	curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	change_path(p);
	gtk_init(&argc,&argv);
	message_box(NULL,_("New Version Now!"));
}
