#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>


unsigned short csum(unsigned short *ptr, int nbytes) {
    unsigned long sum;
    unsigned short oddbyte;
    short checksum;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char*)&oddbyte) = *(u_char*)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    checksum = (short)~sum;
    return checksum;
}

void sendPacket(int src_port, int dest_port, unsigned char *flags, char *target_ip) {
    int sockfd;
    char buffer[4096];
    struct iphdr *ip = (struct iphdr *)buffer;
    struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ip));
    struct sockaddr_in dest_addr;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
    ip->id = htons(54321);
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_TCP;
    ip->check = 0; 
    ip->saddr = inet_addr("1.2.3.4"); 
    ip->daddr = inet_addr(target_ip);

    
    tcp->source = htons(src_port);
    tcp->dest = htons(dest_port);
    tcp->seq = 0;
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->fin = flags[0];
    tcp->syn = flags[1];
    tcp->rst = flags[2];
    tcp->psh = flags[3];
    tcp->ack = flags[4];
    tcp->urg = flags[5];
    tcp->window = htons(5840);
    tcp->check = 0; // Will be filled in by the kernel
    tcp->urg_ptr = 0;

    // Calculate TCP checksum
    tcp->check = csum((unsigned short *)buffer, sizeof(struct iphdr) + sizeof(struct tcphdr));

    // Destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
    dest_addr.sin_addr.s_addr = inet_addr(target_ip);

    // Send the packet
    if (sendto(sockfd, buffer, ip->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto");
    }

    close(sockfd);
}
void interpretResponse(struct tcphdr *tcp) {
    if (tcp->rst) {
        printf("Port is closed\n");
    } else if (tcp->syn && tcp->ack) {
        printf("Port is open\n");
    } else {
        printf("Port state unknown (no response)\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <target_ip> <start_port> <end_port>\n", argv[0]);
        exit(1);
    }

    char *target_ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    
    unsigned char null_flags[] = {0, 0, 0, 0, 0, 0};
    unsigned char fin_flags[] = {1, 0, 0, 0, 0, 0};
    unsigned char xmas_flags[] = {1, 0, 1, 0, 0, 1};


    struct tcphdr tcp_packet;

    for (int port = start_port; port <= end_port; ++port) {
        printf("Scanning port %d\n", port);

        // Null Scan
        sendPacket(port, port, null_flags, target_ip);
        sleep(1); // Pause briefly to avoid flooding the target
        interpretResponse(&tcp_packet); // Call the interpretation function

        // FIN Scan
        sendPacket(port, port, fin_flags, target_ip);
        sleep(1); // Pause briefly to avoid flooding the target
        interpretResponse(&tcp_packet); // Call the interpretation function

        // Xmas Scan
        sendPacket(port, port, xmas_flags, target_ip);
        sleep(1); // Pause briefly to avoid flooding the target
        interpretResponse(&tcp_packet); // Call the interpretation function
    }

    return 0;
}