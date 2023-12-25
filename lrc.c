#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include"lrc.h"

char* open_lrc(char *lrc_file)
{   
    FILE *fptr=NULL;
    char *text_buf=NULL;
    int size=0;

    if(lrc_file==NULL)
    {
        perror("can't find file");
        exit(-1);
    }
    else
    {
        fptr=fopen(lrc_file,"rb");
        if(fptr==NULL)
        {
            perror("open error");
            exit(-1);
        }
        else
        {
            fseek(fptr,0,SEEK_END);
            size=ftell(fptr);
            rewind(fptr);
            text_buf=(char*)malloc(size+1);
            bzero(text_buf,size+1);
            if(text_buf==NULL)
            {
                perror("malloc error");
                exit(-1);
            }
            else
            {
                fread(text_buf,size,1,fptr);
            }
            fclose(fptr);
        }
    }

    return text_buf;
}

lrc* add_node(lrc* head,int time,char* words)
{
    if(head==NULL)
    {
        head=(lrc*)malloc(sizeof(lrc));
        if(head==NULL)
        {
            perror("malooc error");
            exit(-1);
        }
        
        head->next=NULL;
        head->timescale=time;
        strcpy(head->words,words);
    }
    else
    {
        lrc *next_node=head->next,*last_node=head;
        while(next_node)
        {
            last_node=next_node;
            next_node=next_node->next;
        }

        next_node=(lrc*)malloc(sizeof(lrc));
        if(next_node==NULL)
        {
            perror("malloc error");
            exit(-1);
        }

        last_node->next=next_node;
        next_node->next=NULL;
        next_node->timescale=time;
        strcpy(next_node->words,words);
    }

    return head;
}

// init lrc_ptr and sort the link
void sort_lrc(lrc_ptr* ptr)
{
    lrc* next_ptr=ptr->head;

    // get the size
    while(next_ptr)
    {
        ptr->array_size++;
        next_ptr=next_ptr->next;
    }

    // assign ptr
    ptr->ptr_array=calloc(ptr->array_size,sizeof(ptr->ptr_array));
    next_ptr=ptr->head;
    for(int i=0;i<ptr->array_size;i++)
    {
        ptr->ptr_array[i]=next_ptr;
        next_ptr=next_ptr->next;
    }

    // sort
    for(int i=0;i<ptr->array_size;i++)
        for(int j=i+1;j<ptr->array_size;j++)
            if(ptr->ptr_array[i]->timescale > ptr->ptr_array[j]->timescale)
            {
                lrc *temp=ptr->ptr_array[i];
                ptr->ptr_array[i]=ptr->ptr_array[j];
                ptr->ptr_array[j]=temp;
            }

    // refind the ptr
    ptr->head=ptr->ptr_array[0];
    for(int i=0;i<ptr->array_size-1;i++) ptr->ptr_array[i]->next=ptr->ptr_array[i+1];
    ptr->ptr_array[ptr->array_size-1]->next=NULL;
}

char judge_label(char *label)
{
    if(label)
    {
        if(strlen(label)==6)
        {
            if((*label)=='[' && *(label+3)==':'
                && *(label+1) <= '9' && *(label+1) >= '0'
                && *(label+2) <= '9' && *(label+2) >= '0'
                && *(label+4) <= '9' && *(label+4) >= '0'
                && *(label+5) <= '9' && *(label+5) >= '0')
            {
                return 1;
            }
        }
    }

    return 0;
}

long get_time(char *label)
{
    int tm=0;
    int minute=0;
    int second=0;

    sscanf(label,"[%d:%d",&minute,&second);
    tm=minute*60+second;
    return tm;
}

lrc* dispose_one_line(char *line, lrc *head)
{
    char *time_words;
    char *text_words;

    time_words=strtok_r(line,"]",&text_words);
    
    long tm=0;
    if(judge_label(time_words))
    {
        tm=get_time(time_words);
        head=add_node(head,tm,text_words);
    }

    return head;
}

lrc_ptr* get_all_line(char *text_buf,lrc_ptr *ptr)
{
    int flag=1;
    char *cur_line;
    char *rest_line;

    // will delete
    ptr=(lrc_ptr*)malloc(sizeof(lrc_ptr));
    ptr->head=NULL;
    ptr->array_size=0;
    ptr->ptr_array=NULL;

    while((cur_line=strtok_r(text_buf,"\n",&rest_line))!=NULL)
    {
        ptr->head=dispose_one_line(cur_line,ptr->head);
        text_buf=rest_line;
        rest_line=NULL;
    }

    return ptr;
}

void free_array(lrc_ptr *ptr)
{
    if(ptr->ptr_array==NULL) return;

    for(int i=0;i<ptr->array_size;i++) free(ptr->ptr_array[i]);
    free(ptr->ptr_array);
    ptr->ptr_array=NULL;
    free(ptr);
}

void dispose_lrc(char *file_name,lrc_ptr *ptr)
{
    char *file=NULL;
    file=open_lrc(file_name);
    ptr=(lrc_ptr*)malloc(sizeof(lrc_ptr));
    memset(ptr,0,sizeof(lrc_ptr));

    if(file!=NULL)
    {
        get_all_line(file,ptr);
        free(file);
        sort_lrc(ptr);
    }
    else
    {
        perror("lrc error");
        exit(-1);
    }
}