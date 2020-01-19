#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

enum name_of_pipe_descriptors {READ, WRITE}; //fd[0]  fd[1], enum - lista slownych wartosci, ktore moga zostac przypisane do zmienych tego typu; sama zmienna przechowuje wartosci liczbowe calkowite



int main(void) {
    int pipe_descriptors[2];
    if(pipe(pipe_descriptors)<0) //tworzy lacze
    perror("pipe");
    int x;

   pid_t pid = fork(); // powstanie rodzica i dziecka, rozgalezienie

    if (pid<0)
    {
        perror("fork");
        printf("no child created\n");
        //exit(0);
    }
    if (pid!=0)
        {
            printf("Podaj x: \n");
            scanf("%d",&x);

            if(close(pipe_descriptors[READ])<0)
                perror("parent - close read"); // perror -wypisuje komunikat bledu na standardowy srumien bledow(funkcja)
            if(write(pipe_descriptors[WRITE], &x, sizeof(x))<0)
                perror("write");
            printf("parent:%d   value:%d\n",getpid(),x);
            if(close(pipe_descriptors[WRITE])<0)
                perror("parent - close write");
            

        }



    else 
    {
        if(close(pipe_descriptors[WRITE])<0)
            perror("child - close write"); // perror -wypisuje komunikat bledu na standardowy srumien bledow(funkcja)
        if(read(pipe_descriptors[READ], &x, sizeof(x))<0)
            perror("read");
            printf("child:%d   Odebrana wiadomosc, x = %d\n",getpid(), x);
            //fflush() dodalam dodatkowo z poprzedniego przykladu; wymuszazapisanie danych znajdujacych sie w buforach obslugi podanego pliku
        if(close(pipe_descriptors[READ])<0)
        {
            perror("child - close read");

        //exit(3);
        }


    }

            int fd;
            printf("value of x %d",x);
            char *myfifo="/tmp/myfifo";
            mkfifo(myfifo, 0666);
            fd = open(myfifo, O_WRONLY);
            write(fd,&x, sizeof(int));
            close(fd);
            unlink(myfifo);



    return 0;
}
