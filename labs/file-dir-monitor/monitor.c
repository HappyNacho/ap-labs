#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <ftw.h>
#include <fcntl.h> 
#include <unistd.h>
#include "logger.h"

#define MAX_EVENT_MONITOR 2048
#define NAME_LEN 48
#define MONITOR_EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN MAX_EVENT_MONITOR*(MONITOR_EVENT_SIZE+NAME_LEN)


int main(int argc, char **argv) {
    
    int fd,tr,monitor;
    char buffer[BUFFER_LEN];

    fd = inotify_init();
    // Place your magic here
    if(fd == -1){
        printf("Error: Could not start inotify ");
    }

    char* folder = argv[1];
    
    //arg eval
    if (argc < 2){
        printf("Error: Input is incorrect, missing location to monitor\n");
        return 0;
    }

    if (argc > 2){
        printf("Error: Input is incorrect, too many additional args\n");
        return 0;
    }

    //Folder messages
    if (folder== NULL){
        printf("Error: Location failed!: %s \n", argv[1]);
        exit(0);
    }
    else{
        printf("Starting monitor on %s \n", argv[1]);
        printf("-----------------------------------------------------\n");
    }

    inotify_add_watch(fd, argv[1],IN_OPEN|IN_CREATE|IN_DELETE|IN_MODIFY);

    if(monitor==-1){
        printf("Error: Couldn't add monitor to %s\n",argv[1]);
    }


    //Ehh pues uso print porque no habia hecho el logger ¯\_(ツ)_/¯ 
    //Supongo que lo tendre que hacer si se va a usar ya en los labs.
    //solo monitorea el folder actual, no incluye subfolders dentro.

    int i=0;
    while(1){
        i=0;
        tr= read(fd,buffer, BUFFER_LEN);
        
        if(tr<0){
            printf("read error");
        }

        while(i<tr){
            struct inotify_event *event=(struct inotify_event*)&buffer[i];

            if(event->len){
                if(event-> mask & IN_CREATE){
                    if(event-> mask & IN_ISDIR){
                        printf("Info: Directory \"%s\" was created \n", event->name);
                    }    
                    else{
                        printf("Info: File \"%s\" was created \n", event->name);
                    }
                }
                if(event-> mask & IN_MODIFY){
                    if(event-> mask & IN_ISDIR){
                        printf("Info: Directory \"%s\" was modified \n", event->name);
                    }    
                    else{
                        printf("Info: File \"%s\" was modified \n", event->name);
                    }
                }
                if(event-> mask & IN_DELETE){
                    if(event-> mask & IN_ISDIR){
                        printf("Info: Directory \"%s\" was deleted \n", event->name);
                    }    
                    else{
                        printf("Info: File \"%s\" was deleted \n", event->name);
                    }
                }
                i+=MONITOR_EVENT_SIZE+event->len;

            }
        }
    }
    inotify_rm_watch(fd, monitor);
    close(fd);
    exit(0);

}
