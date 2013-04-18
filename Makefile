PKG_INCLUDE=`pkg-config --cflags --libs gtk+-2.0`

gtk_goagent:gtk_goagent.o dialog.o menu.o callback.o
	gcc -o gtk_goagent gtk_goagent.o dialog.o menu.o callback.o $(PKG_INCLUDE)

gtk_goagent.o:gtk_goagent.c callback.h
	gcc -c gtk_goagent.c $(PKG_INCLUDE)

dialog.o:dialog.c dialog.h menu.h
	gcc -c dialog.c $(PKG_INCLUDE)

menu.o:menu.c menu.h
	gcc -c menu.c $(PKG_INCLUDE)

callback.o:callback.c callback.h dialog.h
	gcc -c callback.c $(PKG_INCLUDE)

clean:
	rm gtk_goagent *.o
