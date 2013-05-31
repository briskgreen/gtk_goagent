PKG_LIBS=`pkg-config --cflags --libs gtk+-2.0`
LIBS=dialog.o menu.o callback.o config.o autoupgrade.o

all:gtk_goagent pre_ui upload upgrade

gtk_goagent:gtk_goagent.o autoupgrade.o $(LIBS)
	gcc -o gtk_goagent gtk_goagent.o $(LIBS) $(PKG_LIBS) -lutil -lcurl

gtk_goagent.o:gtk_goagent.c callback.h autoupgrade.h
	gcc -c gtk_goagent.c $(PKG_LIBS)

autoupgrade.o:autoupgrade.c autoupgrade.h config.h
	gcc -c autoupgrade.c $(PKG_LIBS) -lz -lcurl

dialog.o:dialog.c dialog.h menu.h
	gcc -c dialog.c $(PKG_LIBS)

menu.o:menu.c menu.h
	gcc -c menu.c $(PKG_LIBS)

callback.o:callback.c callback.h dialog.h config.h autoupgrade.h
	gcc -c callback.c $(PKG_LIBS)

config.o:config.c config.h dialog.h 
	gcc -c config.c $(PKG_LIBS)

pre_ui:pre_ui.o ui.o $(LIBS)
	gcc -o pre_ui pre_ui.o ui.o $(LIBS) $(PKG_LIBS) -lutil -lcurl

pre_ui.o:pre_ui.c ui.h
	gcc -c pre_ui.c $(PKG_LIBS)

upload:upload.o $(LIBS)
	gcc -o upload upload.o $(LIBS) $(PKG_LIBS) -lcurl -lutil

upload.o:upload.c
	gcc -c upload.c $(PKG_LIBS)

ui.o:ui.c ui.h callback.h
	gcc -c ui.c $(PKG_LIBS)

upgrade:upgrade.o $(LIBS)
	gcc -o upgrade upgrade.o $(LIBS) $(PKG_LIBS) -lcurl -lutil

upgrade.o:upgrade.c autoupgrade.h
	gcc -c upgrade.c $(PKG_LIBS)

clean:
	rm gtk_goagent pre_ui upload upgrade *.o

install:
	./install.sh

uninstall:
	rm -rf /usr/local/share/gtk_goagent && rm $HOME/.gtk_goagentrc
