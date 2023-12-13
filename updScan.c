#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_BUFFER_SIZE 1024

struct ThreadArgs
{
    const char *ip_address;
    int start_port;
    int end_port;
};

void *scanUDP(void *arg)
{
    struct ThreadArgs *args = (struct ThreadArgs *)arg;

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(args->ip_address);

    for (int port = args->start_port; port <= args->end_port; port++)
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }

        serverAddr.sin_port = htons(port);

        // Sending an empty message to the server
        char buffer[MAX_BUFFER_SIZE] = "";
        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            perror("Sendto error");
            close(sock);
            continue;
        }

        // Waiting for a short period to receive a response
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;


        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        if (select(sock + 1, &readSet, NULL, NULL, &timeout) > 0)
        {
            // Received a response, consider the port open
            printf("[*] Port: %d \t Open\n", port);
        }


        close(sock);
    }

    pthread_exit(NULL);
}

void DefaultScanUDP(const char *ip_address)
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

        pthread_create(&threads[i], NULL, scanUDP, &threadArgs[i]);

        start_port += ports_per_thread;
        end_port += ports_per_thread;
        thread_index++;
    }

    printf("Scanning ports 0 --> %d for IP address: %s using UDP\n", num_threads * ports_per_thread, ip_address);

    printf("Created %d threads\n", thread_index);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    time(&endTime);
    double elapsed_secs = difftime(endTime, startTime);
    printf("Elapsed Seconds: %.2f\n", elapsed_secs);
}

int main()
{
    const char *ip_address = "45.33.32.156"; // Change this to the target IP address
    DefaultScanUDP(ip_address);

    return 0;
}
