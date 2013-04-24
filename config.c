#include "config.h"
#include <string.h>

char *get_conf_file_path(void);
char *get_argument(const char *option);
gboolean test_argument(FILE *fp,const char *option,char **result);

gboolean is_config_file_ok(void)
{
	FILE *fp;
	
	if((fp=fopen(get_conf_file_path(),"r"))==NULL)
		return FALSE;

	fclose(fp);

	return TRUE;
}

FILE *open_config(CONFDATA *data)
{
	FILE *fp;

	if(is_config_file_ok())
		fp=fopen(get_conf_file_path(),"r");
	else
		return NULL;

	data->save=TRUE;

	data->python_path=get_python_path(fp);
	data->goagent_path=get_goagent_path(fp);
	data->language_env=get_language_env(fp);
	data->gtk_goagent_path=get_gtk_goagent_path(fp);

	return fp;
}

void close_config(FILE *fp,CONFDATA *data)
{
	if(data->save)
	{
		fclose(fp);
		return;
	}

	if(message_box_ok(_("Are You Want to Save Config File?\nClicked OK To Save,Click No To Not Save")))
		save_config(&fp,data);
	fclose(fp);
}

void save_config(FILE **fp,CONFDATA *data)
{
	if(data->save)
	{
		message_box(NULL,_("Saved"));
		return;
	}

	//remove(get_conf_file_path());
	fclose(*fp);

	if((*fp=fopen(get_conf_file_path(),"w"))==NULL)
		message_box(NULL,strerror(errno));

	fputs("#Python Path\n\npython_path ",*fp);
	fputs(data->python_path,*fp);
	fputs("\n\n#GoAgent Path\n\ngoagent_path ",*fp);
	fputs(data->goagent_path,*fp);
	fputs("\n\n#Language\n\nlanguage_env ",*fp);
	fputs(data->language_env,*fp);
	fputs("\n\n#End Of Gtk GoAgent Config File\n",*fp);

	fclose(*fp);

	*fp=fopen(get_conf_file_path(),"r");

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

	if(test_argument(fp,"python_path",&python_path))
		return python_path;

	//message_box(NULL,_("Don't Find python_path Option!"));
	return NULL;

}

char *get_goagent_path(FILE *fp)
{
	char *goagent_path;
	char *temp;

	if(test_argument(fp,"goagent_path",&temp))
	{
		goagent_path=malloc(strlen(temp)+strlen(PYPATH)+1);
		bzero(goagent_path,strlen(goagent_path)+1);

		strncpy(goagent_path,temp,strlen(temp));
		strncat(goagent_path,PYPATH,strlen(PYPATH));

		free(temp);
		return goagent_path;
	}

	//message_box(NULL,_("Don't Find goagent_path Option!"));
	return NULL;
}

char *get_language_env(FILE *fp)
{
	char *language_env;

	if(test_argument(fp,"language_env",&language_env))
		return language_env;

	//message_box(NULL,_("Don't Find language_env Option!"));
	return NULL;
}

char *get_gtk_goagent_path(FILE *fp)
{
	char *gtk_goagent_path;

	if(test_argument(fp,"gtk_goagent_path",&gtk_goagent_path))
		return gtk_goagent_path;

	return NULL;
}

void set_python_path(CONFDATA *data,char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->python_path=arg;
}

void set_goagent_path(CONFDATA *data,char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->goagent_path=arg;
}

void set_language_env(CONFDATA *data,char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->language_env=arg;
}

void set_gtk_goagent_path(CONFDATA *data,char *arg)
{
	if(data->save)
		data->save=FALSE;

	data->gtk_goagent_path=arg;
}

char *get_conf_file_path(void)
{
	char *path;

	path=malloc(strlen(HOME)+strlen(CONFFILE)+1);
	bzero(path,strlen(HOME)+strlen(CONFFILE)+1);

	strncpy(path,HOME,strlen(HOME));
	strncat(path,CONFFILE,strlen(CONFFILE));

	return path;
}

gboolean test_argument(FILE *fp,const char *option,char **result)
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
			*result=get_argument(buf);
			break;
		}
	}

	if(feof(fp))
	{
		fseek(fp,0,SEEK_SET);
		return FALSE;
	}

	fseek(fp,0,SEEK_SET);

	return TRUE;
}

char *get_argument(const char *option)
{
	int i,j;
	char buf[512];
	char *result;

	bzero(buf,sizeof(buf));
	sscanf(option,"%s",buf);

	i=strlen(buf)+1;
	bzero(buf,sizeof(buf));
	//g_printf("%d:%s\n",i,buf);

	for(j=0;option[i]!=' ' && option[i];++j,++i)
	{
		if(option[i]=='#')
			break;

		buf[j]=option[i];
	}

	if(buf[j-1]=='\n')
		buf[j-1]='\0';
	else
		buf[j]='\0';

	result=malloc(strlen(buf)+1);
	strncpy(result,buf,strlen(buf)+1);

	return result;
}
