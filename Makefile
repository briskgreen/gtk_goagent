PKG_LIBS=`pkg-config --cflags --libs gtk+-2.0`

gtk_goagent:gtk_goagent.o dialog.o menu.o callback.o config.o
	gcc -o gtk_goagent gtk_goagent.o dialog.o menu.o callback.o config.o $(PKG_LIBS)

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

clean:
	rm gtk_goagent *.o
