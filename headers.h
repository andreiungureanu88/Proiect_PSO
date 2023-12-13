// headers zone
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

// define zone

#define MAX_PORT 65535
#define NUM_THREADS 10
#define PORTS_PER_THREAD 400

FILE *flog;