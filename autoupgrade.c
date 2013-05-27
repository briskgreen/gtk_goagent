#include "autoupgrade.h"
#include <string.h>

char *get_version(char *path);
size_t is_upgrade_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream);
//size_t is_upgrade_gtk_goagent(char *ptr,size_t size,size_t nmebm,
//		void *stream);

void download_file(char *path,char *is_upload);
void _download_file(CURL_DATA *data);
int update_progress(void *data,double dltotal,double dlnow,
		double ultotal,double ulnow);
void memcat(char *tmp,char *buf,unsigned long len);
char *get_zip_first_file_name(char *zip_file);
void change_path(const char *path);

char *goagent_version;
//CONFDATA *data;
//char *gtk_goagent_version;

/*int main(int argc,char **argv)
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

	if((fp=fopen(".upgrade.log","r"))==NULL)
	{
		if((fp=fopenh(".upgrade.log","w"))==NULL)
			error_quit("Can Not Create Upgrade Log File");

		fputs(get_version(conf.proxy_py_path),fp);
		fputs(get_version("readme"),fp);
		fclose(fp);
	}

	goagent_version=get_version(conf.proxy_py_path);
	//gtk_goagent_version=get_version("readme");

	if(strcmp(argv[1]"1")==0)
		auto_upgrade_goagent(GOAGENT_URL);
	else if(strcmp(argv[1],"2")==0)
		auto_upgrade_gtk_goagent(GTK_GOAGENT_URL);
	else if(strcmp(argv[1],"3")==0)
	{
		auto_upgrade_goagent(GOAGENT_URL);
		auto_upgrade_gtk_goagent(GTK_GOAGENT_URL);
	}
	auto_upgrade_goagent(GOAGENT_URL);

	return 0;
}*/

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

void auto_upgrade_goagent(char *url,CONFDATA *conf)
{
	//data=conf;
	goagent_version=get_version(conf->proxy_py_path);

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
			sleep(UPDATE_TIME);
		}
	}
}

/*void auto_upgrade_gtk_goagent(char *url)
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
}*/

size_t is_upgrade_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream)
{
	char buf[40];
	char temp[40];
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

			return 0;
		}
		else
			return 0;
	}

	return nmebm;
}

void download_file(char *path,char *is_upload)
{
	/*CURL *curl;
	FILE *fp;

	gtk_init(NULL,NULL);

	if((fp=fopen("/tmp/$goagent$","w"))==NULL)
	{
		message_box(NULL,strerror(errno));
		return;
	}

	curl=curl_easy_init();
	curl_easy_setopt(curl,CURLOPT_URL,path);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
	curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);*/
	GtkWidget *progress_bar;
	GtkWidget *win;
	GtkWidget *vbox;
	GtkWidget *label;
	CURL_DATA data;

	gdk_threads_init();
	gtk_init(NULL,NULL);
	data.url=path;
	data.d_ok=FALSE;

	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER);
	gtk_window_set_icon_from_file(GTK_WINDOW(win),"img/64x64/download.png",NULL);
	g_signal_connect(G_OBJECT(win),"delete_event",G_CALLBACK(quit_no_download),&data.d_ok);

	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(win),vbox);

	label=gtk_label_new(_("Downloading . . ."));
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

	progress_bar=gtk_progress_bar_new();
	gtk_widget_set_size_request(progress_bar,500,30);
	data.progress_bar=progress_bar;
	gtk_box_pack_start(GTK_BOX(vbox),progress_bar,FALSE,FALSE,0);

	gtk_widget_show_all(win);
	//g_thread_new(NULL,(GThreadFunc)_download_file,&data);
	g_thread_create((GThreadFunc)_download_file,&data,TRUE,NULL);

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	if(data.d_ok == TRUE)
	{
		CONFDATA conf;
		FILE *fp;

		if((fp=open_config(&conf))==NULL)
			return;
		fclose(fp);

		unzip("/tmp/$goagent$",conf.goagent_path);
		goagent_version=get_version(conf.proxy_py_path);
	
		if(strstr(is_upload,"是"))
			message_box(NULL,_("This Version Should Upload Again"));
	}
}

void _download_file(CURL_DATA *data)
{
	CURL *curl;
	FILE *fp;

	if((fp=fopen("/tmp/$goagent$","w"))==NULL)
		gtk_main_quit();

	curl=curl_easy_init();

	curl_easy_setopt(curl,CURLOPT_URL,data->url);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
	curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0L);
	curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,data->progress_bar);
	curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,update_progress);

	curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	data->d_ok=TRUE;
	gtk_main_quit();
}

void quit_no_download(GtkWidget *widget,gpointer data)
{
	gboolean *d_ok=(gboolean *)data;

	d_ok=FALSE;
}

int update_progress(void *data,double dltotal,double dlnow,
		double ultotal,double ulnow)
{
	char buf[10];

	bzero(buf,sizeof(buf));
	snprintf(buf,sizeof(buf),"%.1lf %%",dlnow/dltotal*100.0);

	gdk_threads_enter();
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(data),buf);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data),
			dlnow/dltotal);
	gdk_threads_leave();

	return 0;
}

void unzip(char *zip_file,char *goagent_path)
{
	//chdir("/tmp/");
	change_path("/tmp/");

	struct zip_head zip;
	FILE *fp,*out;
	char *buf,*source,*name,*tmp;
	char *first_name=get_zip_first_file_name(zip_file);
	unsigned short s=0x9c78;
	unsigned long blen;
	CONFDATA conf;

	//chdir("/tmp/");

	if((fp=fopen(zip_file,"rb"))==NULL)
	{
		perror("Open File");
		return;
	}

	while(!feof(fp))
	{
		fread(&zip,sizeof(struct zip_head),1,fp);
		if(zip.f_len <= 0)
			break;
		name=malloc(zip.f_len+1);
		bzero(name,zip.f_len+1);
		fread(name,zip.f_len,1,fp);

		if(zip.e_len > 0)
			fseek(fp,zip.e_len,SEEK_CUR);

		if(zip.d_len == 0)
			mkdir(name,0777);
		else if(zip.d_len == zip.len)
		{
			if((out=fopen(name,"w"))==NULL)
			{
				perror("Create File");
				return;
			}

			printf("Unzip %s . . .\n",name);

			buf=malloc(zip.d_len+1);
			bzero(buf,zip.d_len+1);

			fread(buf,zip.d_len,1,fp);
			fwrite(buf,zip.d_len,1,out);

			fclose(out);
			free(buf);

			printf("Unzip %s Successed . . .\n",name);
		}
		else
		{
			buf=malloc(zip.d_len+1);
			bzero(buf,zip.d_len+1);
			fread(buf,zip.d_len,1,fp);

			source=malloc(zip.len+1);
			bzero(source,zip.len+1);

			tmp=malloc(zip.d_len+3);
			bzero(tmp,zip.d_len+3);
			memcpy(tmp,&s,sizeof(s));
			memcat(tmp,buf,zip.d_len);

			if((out=fopen(name,"w"))==NULL)
			{
				perror("Create Error!");
				return;
			}

			printf("Unzip %s . . .\n",name);

			blen=compressBound(zip.len);
			uncompress(source,&blen,tmp,zip.len);

			fwrite(source,zip.len,1,out);

			fclose(out);
			free(source);
			free(tmp);
			free(buf);

			printf("Unzip %s Successed . . .\n",name);
		}

		free(name);
	}

	fclose(fp);

	fp=open_config(&conf);
	fclose(fp);

	//chdir(first_name);
	change_path(first_name);
	//rename(conf.proxy_py_path,"locale/proxy.py.back");
	copy_file(conf.proxy_py_path,"locale/proxy.py.back");

	//rmdir(goagent_path);
	rm_dir(goagent_path);
	if(rename(getcwd(NULL,0),goagent_path)==-1)
		error_quit("rename");
}

int get_zip_file_num(char *zip_file)
{
	int i=0;
	FILE *fp;
	struct zip_head zip;

	if((fp=fopen(zip_file,"rb"))==NULL)
		return -1;

	while(!feof(fp))
	{
		fread(&zip,sizeof(struct zip_head),1,fp);
		if(zip.f_len <= 0)
			break;
		fseek(fp,zip.f_len,SEEK_CUR);

		if(zip.e_len > 0)
			fseek(fp,zip.e_len,SEEK_CUR);

		if(zip.d_len > 0)
			fseek(fp,zip.d_len,SEEK_CUR);

		++i;
	}

	fclose(fp);

	return i;
}

void memcat(char *tmp,char *buf,unsigned long len)
{
	int i=2,j=0;

	while(len)
	{
		tmp[i]=buf[j];

		++i;
		++j;
		--len;
	}
}

char *get_zip_first_file_name(char *zip_file)
{
	struct zip_head zip;
	char *name;
	FILE *fp;

	if((fp=fopen(zip_file,"rb")))
	{
		perror("Open FIle");
		return NULL;
	}

	fread(&zip,sizeof(struct zip_head),1,fp);

	name=malloc(zip.f_len+1);
	bzero(name,zip.f_len+1);
	fread(name,zip.f_len,1,fp);

	fclose(fp);

	return name;
}

void copy_file(const char *old_path,const char *new_path)
{
	FILE *in,*out;
	char buf;

	if(((in=fopen(old_path,"rb"))==NULL) && ((out=fopen(
		new_path,"w"))==NULL))
	{
		perror("Open File");
		return;
	}

	while(!feof(in))
	{
		fread(&buf,sizeof(buf),1,in);
		fwrite(&buf,sizeof(buf),1,out);
	}

	fclose(in);
	fclose(out);
}

void rm_dir(const char *path)
{
	DIR *dir;
	struct dirent *dirp;
	struct stat buf;
	char *p=getcwd(NULL,0);

	if((dir=opendir(path))==NULL)
		error_quit("OpenDir");

	change_path(path);

	while(dirp=readdir(dir))
	{
		if((strcmp(dirp->d_name,".")==0) || (strcmp(dirp->d_name,"..")==0))
			continue;

		if(stat(dirp->d_name,&buf)==-1)
			error_quit("stat");

		if(S_ISDIR(buf.st_mode))
		{
			rm_dir(dirp->d_name);
			/*if(rmdir(dirp->d_name)==-1)
				error_quit("rmdir");
			printf("rm %s Successed . . .\n",dirp->d_name);*/
			continue;
		}

		if(remove(dirp->d_name)==-1)
			error_quit("remove");

		printf("rm %s Successed . . .\n",dirp->d_name);
	}

	closedir(dir);
	change_path(p);

	if(rmdir(path)==-1)
		error_quit("rmdir");

	printf("rm %s Successed . . .\n",path);

}

void change_path(const char *path)
{
	printf("Leave %s Successed . . .\n",getcwd(NULL,0));

	if(chdir(path)==-1)
		error_quit("chdir");

	printf("ENtry %s Successed . . .\n",getcwd(NULL,0));
}
