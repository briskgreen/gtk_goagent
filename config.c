#include "config.h"
#include <string.h>

char *get_conf_file_path(void);
char *get_argument(const char *option);
gboolean test_argument(FILE *fp,const char *option,char *result);

gboolean is_config_file_ok(void)
{
	FILE *fp;

	if((fp=fopen(get_conf_file_path(),"r"))==NULL)
		return FALSE;

	return TRUE;
}

FILE *open_config(CONFDATA *data)
{
	FILE *fp;

	if(is_config_file_ok())
		fp=fopen(get_conf_file_path(),"rw");
	else
		return NULL;

	data->python_path=get_python_path(fp);
	data->goagent_path=get_goagent_path(fp);
	data->language_env=get_language_env(fp);
	
	return fp;
}

void close_config(FILE *fp,CONFDATA *data)
{
	if(data->save)
		fclose(fp);

	if(message_box_ok(_("Are You Want to Save Config File?\nClicked OK To Save,Click No To Not Save")))
		save_config(fp,data);

	fclose(fp);
}

void save_config(FILE *fp,CONFDATA *data)
{
	if(data->save)
	{
		message_box(NULL,_("Saved"));
		return;
	}

	fwrite(data->python_path,strlen(data->python),1,fp);
	fwrite(data->goagent_path,strlen(data->goagent_path),1,fp);
	fwrite(data->language_env,strlen(data->language_env),1,fp);
	data->save=TRUE;
}

char *get_python_path(FILE *fp)
{
	char *python_path;

	/*while(!feof(fp))
	{
		bzero(buf,sizeof(buf));

		fgets(buf,sizeof(buf)-1,fp);

		if(buf[0]=='#')
			continue;

		if(strstr(buf,"python_path"))
		{
			fseek(fp,0L,SEEK_SET);
			python_path=get_argument("python_path");
			break;
		}
	}*/

	if(test_argument(fp,"python_path",python_path))
		return python_path;

	message_box(NULL,_("Don't Find python_path Option!"));
	return NULL;

}

char *get_goagent_path(FILE *fp)
{
	char *goagent_path;

	if(test_argument(fp,"goagent_path",goagent_path))
		return goagent_path;

	message_box(NULL,_("Don't Find goagent_path Option!"));
	return NULL;
}

char *get_language_env(FILE *fp)
{
	char *language_env;

	if(test_argument(fp,"language_env",language_env))
		return language_env;

	message_box(NULL,_("Don't Find language_env Option!"));
	return NULL;
}

void set_python_path(CONFDATA *data,const char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->python_path=arg;
}

void set_goagent_path(CONFDATA *data,const char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->goagent_path=arg;
}

void set_language_env(CONFDATA *data,const char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->language_env=arg;
}

char *get_conf_file_path(void)
{
	char *path;

	path=malloc(strlen(HOME)+strlen(CONFFILE)+1);

	strncpy(path,HOME,strlen(HOME));
	strncat(path,CONFFILE,strlen(CONFFILE));

	return path;
}

gboolean test_argument(FILE *fp,const char *option,char *result)
{
	char buf[1024];

	while(!feof(fp))
	{
		bzero(buf,sizeof(buf));

		fgets(buf,sizeof(buf)-1,fp);

		if(buf[0]=='#')
			continue;

		if(strstr(buf,option))
		{
			result=get_argument(buf);
			break;
		}
	}

	fseek(fp,0L,SEEK_SET);

	if(feof(fp))
		return FALSE;

	return TRUE;
}

char *get_argument(const char *option)
{
	int i,j;
	char buf[512];
	char *result;

	bzero(buf,sizeof(buf));

	i=strlen(option)+1;

	for(j=0;option[i];++j,++i)
	{
		if(option[i]=='#')
		{
			option[j]='\0';
			break;
		}

		buf[j]=option[i];
	}

	buf[j]='\0';

	result=malloc(strlen(buf)+1);
	strncpy(result,buf,strlen(buf));
	return result;
}
