/*自动更新功能
 * 程序在启动时会通过配置文件来检查是否需要自动更新
 * 同时也可以在帮助菜单选项中手动选择更新
 */

#include "autoupgrade.h"

/*复制文件，与copy_file不同之处在于该函数会进入在目录再复制*/

void _copy_file(const char *old_path,const char *new_path);

/*得到文件的绝对目录*/

char *get_rel_path(const char *dir,const char *path);

char *get_version(char *path)
{
	FILE *fp;
	//char buf[512];
	char *buf=NULL;
	size_t f_len;
	char *version;
	int len,i;

	if((fp=fopen(path,"r"))==NULL)
		error_quit(path);
/*提取版本号
 * 通过匹配__version__字符串来得到
 */
	while(getline(&buf,&f_len,fp) > 0)
	{
		if(strstr(buf,"__version__"))
			break;

		free(buf);
		buf=NULL;
	}

/*如果是到达文件未必退出循环，则没有找到version匹配字符串
 * 即没有得到版本号*/

	if(feof(fp))
		error_quit("Can Not Get Version");

/*找到第一个数字字符的下标*/
	for(i=0;buf[i] < '0' || buf[i] > '9';++i);
/*得到字符串的长度*/
	len=strlen(buf)-i-2;
	version=malloc(sizeof(char)*len+1);
/*复制版本号并返回*/
	snprintf(version,sizeof(char)*len+1,"%s",buf+i);
	free(buf);

	return version;
}

void auto_upgrade_goagent(char *url,CONFDATA *conf)
{
	//data=conf;
	//得到goagent当前的版本号
	//
	//goagent_version=get_version(conf->proxy_py_path);

/*在后台运行*/

	if(fork()==0)
	{
		char *res;
/*		HTTP *http;
		char *host;
		char *proxy=PROXY;
		int port;*/
		CURL *curl;
		RET_DATA ret;
		char *download_url;
		int is_upload;
		char *goagent_version;

		curl=curl_easy_init();
		ret.data=NULL;
		ret.len=0;

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
/*		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,
				is_upgrade_goagent);*/

		//由于众所周知的原因，所以需要使用代理
		//
		//curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);
/*		http=http_head_init();
		http_head_add(http,url);
		http_head_add(http,"Host: github.com\n");
		http_head_add(http,"Accept: *//**\n");
		http_head_add(http,"Connection: close\n\n");
		
		host=match_string(".[^:]*",proxy);
		port=atoi(proxy+strlen(host)+1);*/

		while(1)
		{
			//curl_easy_perform(curl);

			//每隔UPDATE_TIME检查一次
			//
			goagent_version=get_version(conf->proxy_py_path);
/*			res=https_perform(http,host,port,NULL,NULL);
			while(res == NULL)
			{
				res=https_perform(http,host,port,NULL,NULL);
				sleep(1);
			}*/
			curl_easy_setopt(curl,CURLOPT_URL,url);
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
				ret.data=NULL;
				ret.len=0;
				free(goagent_version);
			}

			sleep(UPDATE_TIME);
		}
	}
}

/*Gtk GoAgent的自动更新
 * 由于考虑到Gtk GoAgent基本不会有什么变化
 * 所以取消掉了
 */

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

/*判断是否有新的版本需要更新*/

char *is_upgrade_goagent(RET_DATA *data,char *goagent_version)
{
	//char *temp;
	const char *version;
	char *download_url;
	const char *url;
	char msg[1024]={0};
	pcre *reg;
	const char *errptr;
	int erroffset;
	int rc;
	int ovector[3*3];
	int download=0;

	reg=pcre_compile("goagent (\\d+\\.\\d+\\.\\d+) 正式版下载 <a href=\"(.*?)\"",0,&errptr,&erroffset,NULL);
	if(reg == NULL)
		return NULL;

	rc=pcre_exec(reg,NULL,data->data,data->len,0,0,ovector,3*3);
	if(!rc)
	{
		pcre_free(reg);
		return NULL;
	}

	pcre_get_substring(data->data,ovector,rc,1,&version);
	pcre_get_substring(data->data,ovector,rc,2,&url);
	pcre_free(reg);
//	int v_len=strlen("<span class=\"tag-name\">v");

	//temp=match_string("<p>goagent.*正式版下载.*",buf);
	//free(buf);
	//version=match_string("<p>goagent.[^<]*",temp);
/*	version=match_string("<span class=\"tag-name\">v.[^<]*",buf);*/
	if(!version)
	{
		pcre_free_substring(url);
		gtk_init(NULL,NULL);
		message_box(NULL,_("The Release Page Had Changed\nPlease Email To briskgreen@163.com"));
		return NULL;
	}

	//printf("goagent_version=%s version=%s\n",goagent_version,version);
	/*if(!strstr(version,goagent_version))
		download=1;*/
	if(strcmp(version,goagent_version) != 0)
		download=1;

	//printf("%s\n",version+v_len);
	if(!download)
	{
		pcre_free_substring(version);
		pcre_free_substring(url);

		return NULL;
	}
	/*free(version);
	if(!download)
	{
		free(temp);
		return;
	}
	version=match_string("正式版下载 <a href=\".[^\"]*",temp);
	url=string_add("%s",version+strlen("正式版下载 <a href=\""));
	free(version);
	version=match_string("\\\[.[^<]*",temp);
	if(strstr(version,"是"))
		is_upload=1;
	free(version);
	free(temp);*/
/*	url=string_add("https://github.com/goagent/goagent/archive/v%s.zip",version+v_len);*/

	gtk_init(NULL,NULL);
	sprintf(msg,"Have New Version GoAgent v%s\nDo You Want To Upgrade Now?",version);

	download_url=strdup(url);
	pcre_free_substring(url);
	pcre_free_substring(version);

	if(message_box_ok(_(msg)))
		return download_url;
	else
	{
		free(download_url);

		return NULL;
	}
}

int goagent_is_upload(RET_DATA *data)
{
	pcre *reg;
	const char *errptr;
	int erroffset;
	int rc;
	int ovector[2*3];
	const char *is_upload;

	reg=pcre_compile("\\[\\d+? (.*?)\\].*? \\d+\\.\\d+\\.\\d+",0,&errptr,&erroffset,NULL);
	if(reg == NULL)
		return 0;
	rc=pcre_exec(reg,NULL,data->data,data->len,0,0,ovector,2*3);
	if(!rc)
	{
		pcre_free(reg);
		return 0;
	}

	pcre_get_substring(data->data,ovector,rc,1,&is_upload);
	pcre_free(reg);

	if(strcmp(is_upload,"是"))
	{
		pcre_free_substring(is_upload);
		return 1;
	}

	pcre_free_substring(is_upload);

	return 0;
}

/*下载文件界面*/

void download_file(char *path,int is_upload)
{
	GtkWidget *progress_bar;
	GtkWidget *win;
	GtkWidget *vbox;
	GtkWidget *label;
	CURL_DATA data;
	char *goagent_version;

	gdk_threads_init();
	//gtk_init(NULL,NULL);
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
	//添加下载线程
	//
	g_thread_create((GThreadFunc)_download_file,&data,TRUE,NULL);

/*gtk是线程不安全的，所以需要做线程同步*/

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
	gtk_widget_hide(win);
	gtk_widget_destroy(win);

/*如果下载完成，则将下载后的文件解压*/

	if(data.d_ok == TRUE)
	{
		CONFDATA conf;
		FILE *fp;

		if((fp=open_config(&conf))==NULL)
			return;
		fclose(fp);

		unzip_t("/tmp/$goagent$",conf.goagent_path);
		goagent_version=get_version(conf.proxy_py_path);
		change_path(conf.gtk_goagent_path);

	/*如果新版本需要重新上传服务端，弹出对话框进行提示*/

		if(is_upload)
			message_box(NULL,_("This Version Should Upload Again"));
	}

	free(path);
}

/*下载文件线程*/

void _download_file(CURL_DATA *data)
{
	CURL *curl;
	FILE *fp;

	if((fp=fopen("/tmp/$goagent$","w"))==NULL)
		gtk_main_quit();

	curl=curl_easy_init();

	curl_easy_setopt(curl,CURLOPT_URL,data->url);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
	//curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0L);
	curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);

	//设置下载进度函数参数
	//
	curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,data->progress_bar);

	//设置下载进度函数
	//
	curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,update_progress);

	curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	data->d_ok=TRUE;
	gtk_main_quit();
}

/*没有下载完成退出下载后设置d_ok为FALSE*/

void quit_no_download(GtkWidget *widget,gpointer data)
{
	gboolean *d_ok=(gboolean *)data;

	d_ok=FALSE;

	gtk_main_quit();
}

/*进度条更新函数*/

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

/*解压*/

void unzip_t(char *zip_file,char *goagent_path)
{
	struct zip_head zip;
	FILE *fp;

	if((fp=fopen(zip_file,"rb")) == NULL)
	{
		perror(_("Open File Error"));
		return;
	}

	fread(&zip,sizeof(zip),1,fp);
	fclose(fp);

	if(zip.type == 0)
		unzip(zip_file,goagent_path);
	else if(zip.type == 8)
		_unzip(zip_file,goagent_path);
	else
		perror(_("Unknown Type"));
}

void _unzip(char *zip_file,char *goagent_path)
{
	int pipefd[2];
	pid_t pid;
	char buf[512]={0};
	const char *first_name;
	int n;
	int len=0;
	pcre *re;
	int rc;
	const char *errptr;
	int erroffset;
	int ovector[2*3];

	change_path("/tmp/");

	pipe(pipefd);
	if((pid=fork()) == 0)
	{
		close(pipefd[0]);

		dup2(pipefd[1],STDOUT_FILENO);
		execl("/usr/bin/unzip","unzip",zip_file,NULL);
	}
	
	close(pipefd[1]);
	while(len < sizeof(buf)-1)
	{
		n=len;
		n=read(pipefd[0],buf+n,sizeof(buf)-1-len);
		len+=n;
	}

	waitpid(pid,NULL,WNOHANG);

	close(pipefd[0]);
	re=pcre_compile("inflating: (.[^/]*)",0,&errptr,&erroffset,NULL);
	rc=pcre_exec(re,NULL,buf,strlen(buf),0,0,ovector,2*3);
	pcre_get_substring(buf,ovector,rc,1,&first_name);
	//first_name=match_string("inflating: .[^/]*",buf);
	if(first_name)
		change_path(first_name);
	else
	{
		perror(_("Can Not Get uncompress target file name\nmaybe you can uncompress it at /tmp with yourself"));
		return;
	}

	pcre_free(re);
	pcre_free_substring(first_name);
	copy_file(get_proxy_ini_path(goagent_path),"local/proxy.ini.back");
	rm_dir(goagent_path);
	copy_dir(getcwd(NULL,0),goagent_path);
	printf(_("Unzip And Copy Successed . . .\n"));
}

void unzip(char *zip_file,char *goagent_path)
{
	//chdir("/tmp/");
	change_path("/tmp/");

	struct zip_head zip;
	FILE *fp,*out;
	char *buf,*source,*name,*tmp;
	char *first_name=get_zip_first_file_name(zip_file);

	//zlib与标准zip文件格式的不同之处
	//
	unsigned short s=0x9c78;
	unsigned long blen;

	//chdir("/tmp/");

	if((fp=fopen(zip_file,"rb"))==NULL)
	{
		perror("Open File");
		return;
	}

	while(!feof(fp))
	{
		//读取zip头，并得到文件名与扩散字符
		//
		fread(&zip,sizeof(struct zip_head),1,fp);
		if(zip.f_len <= 0)
			break;
		name=malloc(zip.f_len+1);
		bzero(name,zip.f_len+1);
		fread(name,zip.f_len,1,fp);

		if(zip.e_len > 0)
			fseek(fp,zip.e_len,SEEK_CUR);

//创建文件夹
//
		if(zip.d_len == 0)
			mkdir(name,0777);
		else if(zip.d_len == zip.len)
		{
			//如果压缩后的数据与压缩前数据长度相同，则表明该文件未经压缩
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
			//解压文件
			//
			buf=malloc(zip.d_len+1);
			bzero(buf,zip.d_len+1);
			fread(buf,zip.d_len,1,fp);

			source=malloc(zip.len+1);
			bzero(source,zip.len+1);

			tmp=malloc(zip.d_len+3);
			bzero(tmp,zip.d_len+3);
			memcpy(tmp,&s,sizeof(s));

			//添加格式头
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

	//chdir(first_name);
	//进入到解压后的文件的目录
	change_path(first_name);

	//rename(conf.proxy_py_path,"locale/proxy.py.back");
	//保存前一版本的proxy.ini
	copy_file(get_proxy_ini_path(goagent_path),"local/proxy.ini.back");

	//rmdir(goagent_path);
	//删除前一版本文件夹
	rm_dir(goagent_path);

	/*if(rename(getcwd(NULL,0),goagent_path)==-1)
		error_quit("rename");*/
	//复制新版本文件夹
	//
	copy_dir(getcwd(NULL,0),goagent_path);

	printf(_("Unzip And Copy Successed . . .\n"));
}

//得到压缩文件的数量
//
int get_zip_file_num(char *zip_file)
{
	int i=0;
	FILE *fp;
	struct zip_head zip;

	if((fp=fopen(zip_file,"rb"))==NULL)
		return -1;

//通过读出zip头并移动文件名和扩散指针计算压缩文件的数量，直到读出的文件名大小小于0
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

//得到压缩文件第一个目录的名字，用来在解压后进入这个目录
char *get_zip_first_file_name(char *zip_file)
{
	struct zip_head zip;
	char *name;
	FILE *fp;

	if((fp=fopen(zip_file,"rb"))==NULL)
	{
		perror("Open File");
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
	char buf[64];
	size_t len;

	if(((in=fopen(old_path,"rb"))==NULL) || ((out=fopen(
		new_path,"w"))==NULL))
	{
		perror("Open File");
		return;
	}

	while(!feof(in))
	{
		bzero(buf,sizeof(buf));

		len=fread(buf,1,sizeof(buf)-1,in);
		fwrite(buf,len,1,out);
	}

	fclose(in);
	fclose(out);
}

//递归删除文件夹
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

	printf("Entry %s Successed . . .\n",getcwd(NULL,0));
}

//递归复制文件夹
void copy_dir(const char *old_path,const char *new_path)
{
	DIR *dir;
	struct stat buf;
	struct dirent *dirp;
	//保存当前路径
	char *p=getcwd(NULL,0);

//打开文件夹
	if((dir=opendir(old_path))==NULL)
		error_quit(old_path);

	//创建相应文件夹
	if(mkdir(new_path,0777)==-1)
		error_quit(new_path);

//进入老的文件夹
	change_path(old_path);

	while((dirp=readdir(dir)))
	{
		//路过.和..目录
		if(strcmp(dirp->d_name,".")==0 || strcmp(dirp->d_name,"..")==0)
			continue;
		if(stat(dirp->d_name,&buf)==-1)
			error_quit("stat");
		if(S_ISDIR(buf.st_mode))
		{
			//如果当前所读为文件夹，递归
			copy_dir(dirp->d_name,get_rel_path(dirp->d_name,new_path));
			continue;
		}
//复制文件
		_copy_file(dirp->d_name,new_path);
	}

	closedir(dir);
	change_path(p);
}

//得到要复制文件的目的绝对路径
char *get_rel_path(const char *dir,const char *path)
{
	char *rel_path;
	unsigned long d_len,p_len;

	d_len=strlen(dir);
	p_len=strlen(path);
	if((rel_path=malloc(d_len+p_len+2))==NULL)
		error_quit("malloc");
	bzero(rel_path,d_len+p_len+2);

	strncpy(rel_path,path,p_len);
	strncat(rel_path,"/",sizeof(char));
	strncat(rel_path,dir,d_len);

	return rel_path;
}

void _copy_file(const char *old_path,const char *new_path)
{
	FILE *in,*out;
	size_t len;
	char buf[64];
	//保存当前路径
	char *p=getcwd(NULL,0);

	if((in=fopen(old_path,"rb"))==NULL)
		error_quit(old_path);

//进入目的文件夹
	change_path(new_path);

	if((out=fopen(old_path,"wb"))==NULL)
		error_quit(old_path);

	while(!feof(in))
	{
		bzero(buf,sizeof(buf));

		len=fread(&buf,1,sizeof(buf)-1,in);
		fwrite(&buf,len,1,out);
	}

	fclose(in);
	fclose(out);

//返回之前文件
	change_path(p);
}

size_t get_data(char *ptr,size_t size,size_t nmemb,RET_DATA *data)
{
	data->data=realloc(data->data,sizeof(char)*(data->len+nmemb)+1);
	snprintf(data->data+data->len,sizeof(char)*nmemb+1,"%s",ptr);

	data->len+=nmemb;

	return nmemb;
}

