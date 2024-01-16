#include "PortScanner.h" 
#include "ports.h"
#include <stdio.h>

FILE *flog;

char animatie[]=" ███████████                      █████     █████████                                                             \n"
                "░░███░░░░░███                    ░░███     ███░░░░░███                                                            \n"
                " ░███    ░███  ██████  ████████  ███████  ░███    ░░░   ██████   ██████   ████████   ████████    ██████  ████████ \n"
                " ░██████████  ███░░███░░███░░███░░░███░   ░░█████████  ███░░███ ░░░░░███ ░░███░░███ ░░███░░███  ███░░███░░███░░███\n"
                " ░███░░░░░░  ░███ ░███ ░███ ░░░   ░███     ░░░░░░░░███░███ ░░░   ███████  ░███ ░███  ░███ ░███ ░███████  ░███ ░░░ \n"
                " ░███        ░███ ░███ ░███       ░███ ███ ███    ░███░███  ███ ███░░███  ░███ ░███  ░███ ░███ ░███░░░   ░███     \n"
                " █████       ░░██████  █████      ░░█████ ░░█████████ ░░██████ ░░████████ ████ █████ ████ █████░░██████  █████    \n"
                " ░░░░░         ░░░░░░  ░░░░░        ░░░░░   ░░░░░░░░░   ░░░░░░   ░░░░░░░░ ░░░░ ░░░░░ ░░░░ ░░░░░  ░░░░░░  ░░░░░    \n";  

int main(int argc, char **argv)
{   
    

    for(int i=0;i<2020;i++) 
        printf("%c",animatie[i]);

    flog = fopen("log.txt","a");

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
    else if(argc == 5)
    { 
        if(strcmp(argv[1],"-sX") == 0 ) 
        {  
            
            const char *target_ip = argv[2];
            int start_port = atoi(argv[3]);
            int end_port = atoi(argv[4]);
            xmas_scan(target_ip, start_port, end_port);
        }
    }

    return 0;
}