#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_PORT 100

int main(int argc, char **argv)
{
    time_t startTime, endTime;
    time(&startTime);

    if (argc < 2)
    {
        printf("---------Created By Ungureanu Andrei -------------\n");
        printf("--------------------------------------------------\n");
        printf("Usage: ./PortScanner <IPaddress>                  \n");
        printf("--------------------------------------------------\n");
        return (EINVAL);
    }

    printf("Scanning ports 0 --> 65535 for IP address: %s\n", argv[1]);

    int logFileDescriptor = open("log.txt", O_RDWR | O_CREAT, 0640);

    struct sockaddr_in temp = {0};
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(argv[1]);

    for (int port = 0; port < 100; port++)
    {
        int connection = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        temp.sin_port = htons(port);

        int error = connect(connection, (struct sockaddr *)&temp, sizeof(temp));
        if (error == -1 && errno == EINPROGRESS)
        {
            struct timeval timeout;
            timeout.tv_sec = 0;      // Setează timpul de așteptare la 0 secunde
            timeout.tv_usec = 80000; // Setează timpul de așteptare la 800 de milisecunde (jumătate de secundă)

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
                    printf("Port: %d, Service: %s, Open\n", port, service);
                    //fprintf(logFileDescriptor, "Port: %d, Service: %s, Open\n", port, service);
                }
            }
        }
        close(connection);
    }

    ///close(logFileDescriptor);
    time(&endTime);
    double elapsed_secs = difftime(endTime, startTime);
    printf("Elapsed Seconds: %lf\n", elapsed_secs);
    return 0;
}