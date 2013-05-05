PKG_LIBS=`pkg-config --cflags --libs gtk+-2.0`
LIBS=dialog.o menu.o callback.o config.o

all:gtk_goagent pre_ui upload

gtk_goagent:gtk_goagent.o $(LIBS)
	gcc -o gtk_goagent gtk_goagent.o $(LIBS) $(PKG_LIBS) -lutil

gtk_goagent.o:gtk_goagent.c callback.h
	gcc -c gtk_goagent.c $(PKG_LIBS)

dialog.o:dialog.c dialog.h menu.h
	gcc -c dialog.c $(PKG_LIBS)

menu.o:menu.c menu.h
	gcc -c menu.c $(PKG_LIBS)

callback.o:callback.c callback.h dialog.h config.h
	gcc -c callback.c $(PKG_LIBS)

config.o:config.c config.h dialog.h 
	gcc -c config.c $(PKG_LIBS)

pre_ui:pre_ui.o ui.o $(LIBS)
	gcc -o pre_ui pre_ui.o ui.o $(LIBS) $(PKG_LIBS) -lutil

pre_ui.o:pre_ui.c ui.h
	gcc -c pre_ui.c $(PKG_LIBS)

upload:upload.o $(LIBS)
	gcc -o upload upload.o $(LIBS) $(PKG_LIBS) -lutil

upload.o:upload.c
	gcc -c upload.c $(PKG_LIBS)

ui.o:ui.c ui.h callback.h
	gcc -c ui.c $(PKG_LIBS)

clean:
	rm gtk_goagent pre_ui upload *.o
