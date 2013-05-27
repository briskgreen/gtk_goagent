#include "autoupgrade.h"

int main(int argc,char **argv)
{
	CURL *curl;

	goagent_version=get_version(argv[2]);

	curl=curl_easy_init();

	curl_easy_setopt(curl,CURLOPT_URL,argv[1]);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,is_upgrade_goagent);
	curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);

	curl_easy_perform(curl);

	curl_easy_cleanup(curl);
}
