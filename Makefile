obj=main.c lrc.c
exec=main
$(exec):$(obj)
	gcc $^ -o $@ `pkg-config --cflags --libs gtk+-2.0` -lpthread
