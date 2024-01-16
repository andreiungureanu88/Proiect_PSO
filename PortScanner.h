// PortScanner.h

#ifndef PORT_SCANNER_H
#define PORT_SCANNER_H

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
#include <fcntl.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <error.h>
#include <net/if.h>
#include <ifaddrs.h> 


// define zone
#define SOURCE_PORT 12345  
#define TIMEOUT 2  
#define MAX_PORT 65535
#define NUM_THREADS 10
#define PORTS_PER_THREAD 400
#define SOURCE_PORT 12345
#define SIZE_ANIMATIE 2020



struct ThreadArgs
{
    const char *ip_address;
    int start_port;
    int end_port;
};



void *scanPort(void *arg);

void scanAllPorts(char *ipAddress);

void DefaultScan(const char *ip_address);

int resolveDomainToIP(const char *domain, char *ip_buffer, size_t ip_buffer_size);

void *scanCommon(void *arg);

void *scanCommonUDP(void *arg);

void scanMostCommonPorts(char *ipAddress, int flag);

void scan_domain(char *Domain);

void get_local_ip(char *ip_buffer);

void xmas_scan(const char *target_ip, int start_port, int end_port);

#endif // PORT_SCANNER_H
