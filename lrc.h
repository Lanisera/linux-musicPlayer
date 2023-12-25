#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct lrc{
    struct lrc *next;
    int timescale;
    char words[300];
}lrc;

typedef struct lrc_ptr{
    lrc *head;
    lrc **ptr_array;
    int array_size;
}lrc_ptr;

char* open_lrc(char *lrc_file);
lrc* add_node(lrc* head,int time,char *word);
void sort_lrc(lrc_ptr* ptr);
char judge_label(char *label);
long get_time(char *label);
void free_all_node(lrc_ptr *ptr);
lrc* dispose_one_line(char *line,lrc* head);
lrc_ptr* get_all_line(char *text_buf,lrc_ptr *ptr);
void free_array(lrc_ptr *ptr);
void dispose_lrc(char *file_name,lrc_ptr *ptr);
