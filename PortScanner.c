#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#define MAX_PORT 65535
#define NUM_THREADS 10
#define PORTS_PER_THREAD 400

struct ThreadArgs {
    const char* ip_address;
    int start_port;
    int end_port;
};

void* scanPort(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
   
    struct sockaddr_in temp = { 0 };
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(args->ip_address); 

    for (int port = args->start_port; port <= args->end_port; port++) {
        
        int connection = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK , 0);
        temp.sin_port = htons(port);
        
        
        int error = connect(connection, (struct sockaddr *)&temp, sizeof(temp));
        if (error == -1 && errno == EINPROGRESS)
        {
            struct timeval timeout;
            timeout.tv_sec = 0;      // Setează timpul de așteptare la 0 secunde
            timeout.tv_usec = 80000; // Setează timpul de așteptare la 800 de milisecunde 

            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(connection, &writeSet);

            int selectResult = select(connection + 1, NULL, &writeSet, NULL, &timeout);
            if (selectResult > 0)
            {
                int socketError = 0;
                socklen_t socketErrorLength = sizeof(socketError);
                getsockopt(connection, SOL_SOCKET, SO_ERROR, &socketError, &socketErrorLength);
                if (socketError == 0)
                {
                    char host[128];
                    char service[128];
                    getnameinfo((struct sockaddr *)&temp, sizeof(temp), host, sizeof(host), service, sizeof(service), 0);
                    printf("Port: %d \t Service: %s\t\t Open\n", port, service);
                    //fprintf(logFileDescriptor, "Port: %d, Service: %s, Open\n", port, service);
                }
            }
        
        } 
        close(connection);    
    }
     
   pthread_exit(NULL);
}

int main(int argc, char** argv) {
    time_t startTime, endTime;
    time(&startTime);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <IPaddress>\n", argv[0]);
        return EINVAL;
    } 
    else if (argc == 2){
        printf("Scanning ports 0 --> %d for IP address: %s\n",NUM_THREADS * PORTS_PER_THREAD ,argv[1]);

        pthread_t threads[NUM_THREADS];
        struct ThreadArgs threadArgs[NUM_THREADS];

        int start_port = 1;
        int end_port = PORTS_PER_THREAD;
        int thread_index = 0;

        for (int i = 0; i < NUM_THREADS; i++) {
                threadArgs[i].ip_address = argv[1];
                threadArgs[i].start_port = start_port;
                threadArgs[i].end_port = end_port;

                pthread_create(&threads[i], NULL, scanPort, &threadArgs[i]);

                start_port += PORTS_PER_THREAD;
                end_port += PORTS_PER_THREAD;
                thread_index++;
            }
        printf("Create %d threads\n",thread_index);
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        time(&endTime);
        double elapsed_secs = difftime(endTime, startTime);
        printf("Elapsed Seconds: %.2f\n", elapsed_secs);
        } 
        else if(argc == 3) 
        { 
            printf("More arguments\n");
        }
    return 0;
}