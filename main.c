#include <stdio.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void user_ui(int argc,char *argv[],int kid_uid);
void music_player();
void quit_event(GtkWidget* win,gpointer user_data);
void button_event(GtkWidget* button,gpointer user_data);
void* progress_speed_fun1(void * arg);
void* progress_speed_fun2(void * arg);


int flag=0;
int fd_ui,fd[2];
int music_id=0;
char* music[20]={"./music/1.mp3","./music/2.mp3"};
GtkWidget* debug;
GtkWidget* progress_speed;
GtkWidget* progress_voice;

int main(int argc,char *argv[])
{
    mkfifo("fifo_temp_file",0777);

    pipe(fd);
    pid_t pid=fork();

    if(pid < 0) {
		perror("fork error");
		_exit(-1);
	}
	
	if(pid > 0) {
		user_ui(argc, argv, pid);

	}
	else {
		music_player();
	}

    return 0;
}

void user_ui(int argc,char *argv[],int kid_uid)
{
    fd_ui = open("fifo_temp_file",O_WRONLY);
    write(fd_ui,"volume 50 1\n",strlen("volume 50 1\n"));   

    char str_kid_uid[10];
    sprintf(str_kid_uid,"%d",kid_uid);

    gtk_init(&argc, &argv);
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win),"MusicPlayer");
    gtk_window_set_default_size(GTK_WINDOW(win),700,700);
    g_signal_connect(win,"destroy",G_CALLBACK(quit_event),str_kid_uid);

    GtkWidget *table = gtk_table_new(10,3,TRUE);
    gtk_container_add(GTK_WINDOW(win),table);
    
    debug = gtk_label_new("debug");
    gtk_table_attach_defaults(GTK_TABLE(table),debug,0,3,0,6);

    // progress_bar show
    GtkWidget* progress_speed = gtk_progress_bar_new();
    gtk_table_attach_defaults(GTK_TABLE(table),progress_speed,0,3,6,7);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_speed),0);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_speed),"speed");
    
    // volume show
    GtkWidget* progress_voice = gtk_progress_bar_new();
    gtk_table_attach_defaults(GTK_TABLE(table),progress_voice,0,3,7,8);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_voice), 0.5);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_voice), "voice");
    // create button
    GtkWidget* btn_mute = gtk_button_new_with_label("mute");
    gtk_table_attach_defaults(GTK_TABLE(table),btn_mute,1,2,8,9);
    GtkWidget* btn_up = gtk_button_new_with_label("up");
    gtk_table_attach_defaults(GTK_TABLE(table),btn_up,2,3,8,9);
    GtkWidget* btn_down = gtk_button_new_with_label("down");
    gtk_table_attach_defaults(GTK_TABLE(table),btn_down,0,1,8,9);

    // switch the song
    GtkWidget* btn_last = gtk_button_new_with_label("last");
    gtk_table_attach_defaults(GTK_TABLE(table),btn_last,0,1,9,10);
    GtkWidget* btn_start = gtk_button_new_with_label("start");
    gtk_table_attach_defaults(GTK_TABLE(table),btn_start,1,2,9,10);
    GtkWidget* btn_next = gtk_button_new_with_label("next");
    gtk_table_attach_defaults(GTK_TABLE(table),btn_next,2,3,9,10);

    g_signal_connect(btn_last,"clicked",G_CALLBACK(button_event),NULL);
    g_signal_connect(btn_start,"clicked",G_CALLBACK(button_event),NULL);
    g_signal_connect(btn_next,"clicked",G_CALLBACK(button_event),NULL);

    gtk_widget_show_all(win);
    gtk_main();

}

void music_player()
{
	dup2(fd[1], 1);
	execlp("mplayer", 
				"mplayer", 
				"-slave", "-quiet", "-idle", 
				"-input", 
				"file=./fifo_temp_file", NULL);
}

void quit_event(GtkWidget* win,gpointer user_data)
{
    printf("DEBUG");
    char quit_id[100];
    sprintf(quit_id,"kill -9 %s",(char*)user_data);
    system(quit_id);
    g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);
}
    

void button_event(GtkWidget* button,gpointer user_data)
{
    char *button_name = (char*)gtk_button_get_label(GTK_BUTTON(button)); 

    if(button_name[0]=='s')
    {
        gtk_label_set_text(GTK_LABEL(debug),"get into button_event");
        write(fd_ui,"loadfile ./music/1.mp3\n",strlen("loadfile ./music/1.mp3\n"));
        gtk_button_set_label(GTK_BUTTON(button), "pause");
        
    } else if(button_name[0]=='n') {
        music_id=(music_id+1)%2;
        char next_name[50];
        sprintf(next_name,"loadfile %s\n",music[music_id]);
        write(fd_ui,next_name,strlen(next_name));
    } else if(button_name[0]=='l') {
        music_id=(music_id-1+2)%2;
        char last_name[50];
        sprintf(last_name,"loadfile %s\n",music[music_id]);
        write(fd_ui,last_name,strlen(last_name));
    }
}
