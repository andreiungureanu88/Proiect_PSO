#include "headers.h"
#include "ports.h"


struct ThreadArgs
{
    const char *ip_address;
    int start_port;
    int end_port;
};

void *scanPort(void *arg)
{
    struct ThreadArgs *args = (struct ThreadArgs *)arg;

    struct sockaddr_in temp = {0};
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(args->ip_address);

    for (int port = args->start_port; port <= args->end_port; port++)
    {
        int connection = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        temp.sin_port = htons(port);

        int error = connect(connection, (struct sockaddr *)&temp, sizeof(temp));
        if (error == -1 && errno == EINPROGRESS)
        {
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 80000;

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
                    char service[28];
                    getnameinfo((struct sockaddr *)&temp, sizeof(temp), host, sizeof(host), service, sizeof(service), 0);

                    printf("[*]Port: %d \t Service: %s\t Open\n", port, service);
                }
            }
        }
        close(connection);
    }

    pthread_exit(NULL);
}

void scanAllPorts(char *ipAddress)
{
    time_t startTime, endTime;
    time(&startTime);

    printf("Scan all ports\n");

    int ports_per_thread, num_threads;
    ports_per_thread = 6500;
    num_threads = 10;

    pthread_t threads[num_threads];
    struct ThreadArgs threadArgs[num_threads];

    int start_port = 1;
    int end_port = ports_per_thread;
    int thread_index = 0;

    for (int i = 0; i < num_threads; i++)
    {
        threadArgs[i].ip_address = ipAddress;
        threadArgs[i].start_port = start_port;
        threadArgs[i].end_port = end_port;

        pthread_create(&threads[i], NULL, scanPort, &threadArgs[i]);

        start_port += ports_per_thread;
        end_port += ports_per_thread;
        thread_index++;
    }

    printf("Scanning ports 0 --> %d for IP address: %s\n", num_threads * ports_per_thread, ipAddress);

    printf("Created %d threads\n", thread_index);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    time(&endTime);
    double elapsed_secs = difftime(endTime, startTime);
    printf("Elapsed Seconds: %.2f\n", elapsed_secs);
}

void DefaultScan(const char *ip_address)
{
    time_t startTime, endTime;
    time(&startTime);

    int ports_per_thread, num_threads;

    printf("Enter ports per thread: ");
    scanf("%d", &ports_per_thread);

    printf("Enter number of threads: ");
    scanf("%d", &num_threads);

    pthread_t threads[num_threads];
    struct ThreadArgs threadArgs[num_threads];

    int start_port = 1;
    int end_port = ports_per_thread;
    int thread_index = 0;

    for (int i = 0; i < num_threads; i++)
    {
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

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    time(&endTime);
    double elapsed_secs = difftime(endTime, startTime);
    printf("Elapsed Seconds: %.2f\n", elapsed_secs);
}

int resolveDomainToIP(const char *domain, char *ip_buffer, size_t ip_buffer_size)
{
    struct hostent *host_entry;
    struct in_addr **addr_list;

    host_entry = gethostbyname(domain);

    if (host_entry == NULL)
    {
        return -1;
    }

    addr_list = (struct in_addr **)host_entry->h_addr_list;

    if (addr_list[0] == NULL)
    {
        return -1;
    }

    strncpy(ip_buffer, inet_ntoa(*addr_list[0]), ip_buffer_size);

    return 0;
}

void *scanCommon(void *arg)
{
    struct ThreadArgs *args = (struct ThreadArgs *)arg;

    struct sockaddr_in temp = {0};
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(args->ip_address);

    for (int index = args->start_port; index < args->end_port; index++)
    {
        int connection = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        temp.sin_port = htons(MOST_COMMON_PORTS_1002[index]);
        int error = connect(connection, (struct sockaddr *)&temp, sizeof(temp));
        if (error == -1 && errno == EINPROGRESS)
        {
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 120000;

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
                    char service[28];
                    getnameinfo((struct sockaddr *)&temp, sizeof(temp), host, sizeof(host), service, sizeof(service), 0);

                    printf("[*]Port: %d \t Service: %s\t Open\n", MOST_COMMON_PORTS_1002[index], service);
                }
            }
        }
        close(connection);
    }

    pthread_exit(NULL);
}

void *scanCommonUDP(void *arg) {
   
    struct ThreadArgs *args = (struct ThreadArgs *)arg;

    struct sockaddr_in temp = {0};
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(args->ip_address);

    printf("Am intrat aici\n");

    for (int index = args->start_port; index < args->end_port; index++) {
        int connection = socket(AF_INET, SOCK_DGRAM, 0);

        if (connection == -1) {
            perror("Error creating socket");
            continue; 
        }

        temp.sin_port = htons(MOST_COMMON_PORTS_1002[index]);

        
        if (fcntl(connection, F_SETFL, O_NONBLOCK) == -1) {
            perror("Error setting non-blocking mode");
            close(connection);
            continue;
        }

        // Încercare de conectare la port
        if (connect(connection, (struct sockaddr *)&temp, sizeof(temp)) == -1) {
            if (errno == EINPROGRESS) {
                fd_set writeSet;
                FD_ZERO(&writeSet);
                FD_SET(connection, &writeSet);

                struct timeval timeout;
                timeout.tv_sec = 1; // Timpul în secunde
                timeout.tv_usec = 0; // Timpul în microsecunde

                // Verificare dacă conectarea a reușit
                if (select(connection + 1, NULL, &writeSet, NULL, &timeout) > 0) {
                    int socketError = 0;
                    socklen_t socketErrorLength = sizeof(socketError);

                    getsockopt(connection, SOL_SOCKET, SO_ERROR, &socketError, &socketErrorLength);

                    if (socketError == 0) {
                        char host[128];
                        char service[28];
                        getnameinfo((struct sockaddr *)&temp, sizeof(temp), host, sizeof(host), service, sizeof(service), 0);
                        printf("[*] Port: %d \t Service: %s\t Open\n", MOST_COMMON_PORTS_1002[index], service);
                    } else {
                        perror("Error in socket operation");
                    }
                } else {
                    perror("Error in select");
                }
            } else {
                perror("Error in connect");
            }
        }

        close(connection);
    }

    pthread_exit(NULL);
}


void scanMostCommonPorts(char *ipAddress, int flag)
{
    time_t startTime, endTime;
    time(&startTime);

    int ports_per_thread = 200;
    int num_threads = 5;
    int index_to_start = 0;

    pthread_t threads[num_threads];
    struct ThreadArgs ThreadArgs[num_threads];
    int thread_index = 0;
    if (flag == 0)
    {
        for (int i = 0; i < num_threads; i++)
        {

            ThreadArgs[i].ip_address = ipAddress;
            ThreadArgs[i].start_port = index_to_start;
            ThreadArgs[i].end_port = index_to_start + ports_per_thread;

            pthread_create(&threads[i], NULL, scanCommon, &ThreadArgs[i]);

            thread_index++;
            index_to_start += ports_per_thread;
        }

        printf("Scanning ports\n");
        printf("Created %d threads\n", thread_index);

        for (int i = 0; i < num_threads; i++)
        {
            pthread_join(threads[i], NULL);
        }
    } else if (flag == 1) { 
        printf("Scan with UDP\n");
        for (int i = 0; i < num_threads; i++)
        {

            ThreadArgs[i].ip_address = ipAddress;
            ThreadArgs[i].start_port = index_to_start;
            ThreadArgs[i].end_port = index_to_start + ports_per_thread;

            pthread_create(&threads[i], NULL, scanCommonUDP, &ThreadArgs[i]);

            thread_index++;
            index_to_start += ports_per_thread;
        }

        printf("Scanning ports\n");
        printf("Created %d threads\n", thread_index);

        for (int i = 0; i < num_threads; i++)
        {
            pthread_join(threads[i], NULL);
        }

    }
    time(&endTime);
    double elapsed_secs = difftime(endTime, startTime);
    printf("Elapsed Seconds: %.2f\n", elapsed_secs);
}

void scan_domain(char *Domain)
{
    printf("***DOMAIN NAME RESOLUTION***\n");

    char ip_buffer[INET_ADDRSTRLEN];
    int len_domain = strlen(Domain);

    char *domain = (char *)malloc(len_domain);

    strcpy(domain, Domain);

    if (resolveDomainToIP(domain, ip_buffer, sizeof(ip_buffer) - 1) == 0)
    {
        printf("IP address for %s is %s\n", domain, ip_buffer);
        DefaultScan(ip_buffer);
    }
    else
    {
        printf("Failed to resolve %s to an IP address.\n", domain);
    }
}

void setupEnvironment() 
{
    flog = fopen("log.txt","a");
}

void finishScanApp() 
{ 
    fclose(flog);
}

int main(int argc, char **argv)
{   
    setupEnvironment(); 

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <IPaddress>\n", argv[0]);
        return EINVAL;
    }
    else if (argc == 2)
    {   
        fprintf(flog,"Default scanning for IP %s \n",argv[1]);
        DefaultScan(argv[1]);
    }
    else if (argc == 3)
    {

        if (strcmp(argv[1], "-dns") == 0)
        {   
            fprintf(flog,"Domanin name scanning for domain %s \n",argv[2]);
            scan_domain(argv[2]);
        }

        if (strcmp(argv[1], "-p-") == 0)
        {   
            fprintf(flog,"All ports scanning for IP %s \n",argv[2]);
            scanAllPorts(argv[2]);
        }

        if (strcmp(argv[1], "-i") == 0)
        {
            printf("***MOST COMMON PORTS***\n");
            fprintf(flog,"Scanning most common ports for IP %s \n",argv[2]);

            size_t num_ports = sizeof(MOST_COMMON_PORTS_1002) 
                / sizeof(MOST_COMMON_PORTS_1002[0]);
            int len_domain = strlen(argv[2]);
            char *domain = (char *)malloc(len_domain);
            strcpy(domain, argv[2]); 
            scanMostCommonPorts(domain, 0);
        } 

        if (strcmp(argv[1],"-udp") == 0) 
        { 
            int len_domain = strlen(argv[2]);
            char *domain = (char *)malloc(len_domain);
            strcpy(domain, argv[2]);
            scanMostCommonPorts(domain, 1);
        }
    }
    else if (argc == 4)
    {
        if ((strcmp(argv[1], "-dns") == 0 && strcmp(argv[2], "-i") == 0 ) 
            || (strcmp(argv[2], "-dns") == 0 && strcmp(argv[1], "-i") == 0 ))
        {
            printf("***DOMAIN NAME RESOLUTION***\n");

            char ip_buffer[INET_ADDRSTRLEN];
            int len_domain = strlen(argv[3]);

            char *domain = (char *)malloc(len_domain);

            strcpy(domain, argv[3]);

            if (resolveDomainToIP(domain, ip_buffer,   
                 sizeof(ip_buffer) - 1) == 0)
            {   
                fprintf(flog,"Scanning most common ports for IP %s -> domain %s \n",ip_buffer ,domain);
                printf("IP address for %s is %s\n", domain, ip_buffer);
                scanMostCommonPorts(ip_buffer,0);
            }
            else
            {
                printf("Failed to resolve %s to an IP address.\n", domain);
            }
        }
    }

    finishScanApp();

    return 0;
}
