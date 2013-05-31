/*自动更新功能
 * 程序在启动时会通过配置文件来检查是否需要自动更新
 * 同时也可以在帮助菜单选项中手动选择更新
 */

#include "autoupgrade.h"
#include <string.h>

/*复制文件，与copy_file不同之处在于该函数会进入在目录再复制*/

void _copy_file(const char *old_path,const char *new_path);

/*得到文件的绝对目录*/

char *get_rel_path(const char *dir,const char *path);

char *get_version(char *path)
{
	FILE *fp;
	char buf[512];
	char *version;
	int len,i,j;
	int index;

	if((fp=fopen(path,"r"))==NULL)
		error_quit(path);
/*提取版本号
 * 通过匹配version字符串来得到
 * 文件中以#开头的是注释，所以路过
 */
	while(!feof(fp))
	{
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf)-1,fp);

		if(buf[0] == '#')
			continue;

		if(strstr(buf,"version"))
			break;
	}

/*如果是到达文件未必退出循环，则没有找到version匹配字符串
 * 即没有得到版本号*/

	if(feof(fp))
		error_quit("Can Not Get Version");

/*找到第一个数字字符的下标*/

	for(i=0;buf[i];++i)
		if(buf[i] >= '0' && buf[i] <='9')
			break;
	index=i;

/*得到剩余字符串的长度*/

	for(len=0;buf[i];++len,++i);

/*去掉回车符*/

	--len;
	if(buf[i-2] < '0' || buf[i-2] >'9')
		--len;

	version=malloc(len+1);
	bzero(version,len+1);

/*复制版本号并返回*/

	for(j=0;len;++index,++j,--len)
		version[j]=buf[index];

	return version;
}

void auto_upgrade_goagent(char *url,CONFDATA *conf)
{
	//data=conf;
	//得到goagent当前的版本号
	//
	goagent_version=get_version(conf->proxy_py_path);

/*在后台运行*/

	if(fork()==0)
	{
		CURL *curl;

		curl=curl_easy_init();
		curl_easy_setopt(curl,CURLOPT_URL,url);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,
				is_upgrade_goagent);

		//由于众所周知的原因，所以需要使用代理
		//
		curl_easy_setopt(curl,CURLOPT_PROXY,PROXY);

		while(1)
		{
			curl_easy_perform(curl);

			//每隔UPDATE_TIME检查一次
			//
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

size_t is_upgrade_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream)
{
	char buf[40];
	char temp[40];
	char path[100];
	int i,j;

/*首先得到当前最新版本下载地址并保存*/

	if(strstr(ptr,"https://nodeload.github.com"))
	{
		bzero(buf,sizeof(buf));

		for(i=0;ptr[i];++i)
		{
			if(ptr[i] == '"')
			{
				bzero(temp,sizeof(temp));

/*每一次检查时先匹配到"
 * 然后将指针移动到该字符目标进行读39个字符操作
 */

				snprintf(temp,39,"%s",ptr+i);

				if(strstr(temp,"https://nodeload.github.com"))
					goto ok;
			}
		}
ok:

		/*如果返回的数据已经读到了结尾，则没有得到下载地址*/

		if(ptr[i] == '\0')
			return 0;

		for(++i,j=0;ptr[i] != '"';++i,++j)
			path[j]=ptr[i];

		path[j]='\0';
	}

	if(strstr(ptr,"最近更新") && strstr(ptr,"</li><ol><li>"))
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
			/*如果当前版本与最新版本不同
			 * 则弹出对话框询问是否更新*/

			gtk_init(NULL,NULL);
			if(message_box_ok(_("Have New Version GoAgent Do You Want To Upgrade Now?")))
				download_file(path,buf);

			gtk_init(NULL,NULL);

			return 0;
		}
		else
			return 0;
	}

	return nmebm;
}

/*下载文件界面*/

void download_file(char *path,char *is_upload)
{
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
	//添加下载线程
	//
	g_thread_create((GThreadFunc)_download_file,&data,TRUE,NULL);

/*gtk是线程不安全的，所以需要做线程同步*/

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

/*如果下载完成，则将下载后的文件解压*/

	if(data.d_ok == TRUE)
	{
		CONFDATA conf;
		FILE *fp;

		if((fp=open_config(&conf))==NULL)
			return;
		fclose(fp);

		unzip("/tmp/$goagent$",conf.goagent_path);
		goagent_version=get_version(conf.proxy_py_path);

	/*如果新版本需要重新上传服务端，弹出对话框进行提示*/

		if(strstr(is_upload,"是"))
			message_box(NULL,_("This Version Should Upload Again"));
	}
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
	curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0L);

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

