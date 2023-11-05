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
            timeout.tv_usec = 100000; // Setează timpul de așteptare la 800 de milisecunde 

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
                    printf("[*]Port: %d \t Service: %s\t Open\n", port, service);
                    //fprintf(logFileDescriptor, "Port: %d, Service: %s, Open\n", port, service);
                }
            }
        
        } 
        close(connection);    
    }
     
   pthread_exit(NULL);
}

void mainFunction(const char* ip_address) {
    time_t startTime, endTime;
    time(&startTime);
    

    int ports_per_thread, num_threads;

    printf("Enter ports per thread: ");
    scanf("%d", &ports_per_thread);

    printf("Enter number of threads: ");
    scanf("%d", &num_threads);
    
    pthread_t threads[NUM_THREADS];
    struct ThreadArgs threadArgs[NUM_THREADS];

    int start_port = 1;
    int end_port = ports_per_thread;
    int thread_index = 0;

    for (int i = 0; i < num_threads; i++) {
        threadArgs[i].ip_address = ip_address;
        threadArgs[i].start_port = start_port;
        threadArgs[i].end_port = end_port;

        pthread_create(&threads[i], NULL, scanPort, &threadArgs[i]);

        start_port += ports_per_thread;
        end_port += ports_per_thread;
        thread_index++;
    }
    printf("Scanning ports 0 --> %d for IP address: %s\n", num_threads * ports_per_thread, ip_address);

    printf("Created %d threads\n", thread_index);
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    time(&endTime);
    double elapsed_secs = difftime(endTime, startTime);
    printf("Elapsed Seconds: %.2f\n", elapsed_secs);
}

int resolveDomainToIP(const char* domain, char* ip_buffer, size_t ip_buffer_size) {
    struct hostent *host_entry;
    struct in_addr **addr_list;
    
    host_entry = gethostbyname(domain);
    
    if (host_entry == NULL) {
        // Domain resolution failed
        return -1;
    }
    
    addr_list = (struct in_addr**)host_entry->h_addr_list;
    
    if (addr_list[0] == NULL) {
        // No IP address found
        return -1;
    }

    strncpy(ip_buffer, inet_ntoa(*addr_list[0]), ip_buffer_size);
    
    return 0;
}

int main(int argc, char** argv) {


    if (argc < 2) {
        fprintf(stderr, "Usage: %s <IPaddress>\n", argv[0]);
        return EINVAL;
    } 
    else if (argc == 2)
    {
     
        mainFunction(argv[1]);
         
    } 
    else if(argc == 3) 
    {   

        if(strcmp(argv[1],"-dns")==0) 
        { 
            printf("***DOMAIN NAME RESOLUTION***\n"); 
           
            char ip_buffer[INET_ADDRSTRLEN]; // Buffer for the IP address
            
            int len_domain=strlen(argv[2]); 
            
            char *domain = (char*)malloc(len_domain); 
            
            strcpy(domain,argv[2]);
            
            if (resolveDomainToIP(domain, ip_buffer, sizeof(ip_buffer) - 1) == 0) {
            
               printf("IP address for %s is %s\n", domain, ip_buffer);
               mainFunction(ip_buffer);
            } else {
               printf("Failed to resolve %s to an IP address.\n", domain);
            }
            

        }
        
    }
    
    return 0;
}