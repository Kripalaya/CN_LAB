/*
  tcp_sniffer_simple.c
  Simple TCP sniffer for lab practice.
  - No command-line args
  - Uses fixed interface: change IFACE_NAME below 
  - Prints Ethernet → IPv4 → TCP headers + payload (first 128 bytes)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <linux/if_packet.h>
#include <linux/if_ether.h>

#define BUFSIZE 65536
#define PAYLOAD_MAX 128

/* CHANGE THIS FOR MININET */
#define IFACE_NAME "h1-eth0"     // <---- Set your interface here!

static volatile int keep_running = 1;
void handler(int x) { keep_running = 0; }

/* Hex dump for payload */
void hexdump(const unsigned char *buf, int len) {
    for (int i = 0; i < len; i += 16) {
        printf("%04x  ", i);
        for (int j = 0; j < 16; j++) {
            if (i + j < len) printf("%02x ", buf[i + j]);
            else printf("   ");
        }
        printf(" ");
        for (int j = 0; j < 16 && i + j < len; j++) {
            unsigned char c = buf[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("\n");
    }
}

int main() {
    signal(SIGINT, handler);

    printf("Using interface: %s\n", IFACE_NAME);

    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    /* Get interface index */
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, IFACE_NAME, IFNAMSIZ - 1);

    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        close(sockfd);
        return 1;
    }

    /* Bind AF_PACKET socket */
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);

    if (bind(sockfd, (struct sockaddr*)&sll, sizeof(sll)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    /* Add timeout so Ctrl+C works immediately */
    struct timeval tv = {1, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    unsigned char *buffer = malloc(BUFSIZE);

    printf("Capturing TCP packets... Press Ctrl+C to stop.\n");

    while (keep_running) {
        ssize_t n = recvfrom(sockfd, buffer, BUFSIZE, 0, NULL, NULL);

        if (n <= 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) continue;
            else continue;
        }

        if (n < sizeof(struct ethhdr)) continue;
        struct ethhdr *eth = (struct ethhdr*)buffer;

        if (ntohs(eth->h_proto) != ETH_P_IP) continue;

        /* IP header */
        struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
        int ip_len = ip->ihl * 4;
        if (ip->protocol != IPPROTO_TCP) continue;

        char src_ip[16], dst_ip[16];
        inet_ntop(AF_INET, &ip->saddr, src_ip, 16);
        inet_ntop(AF_INET, &ip->daddr, dst_ip, 16);

        /* TCP header */
        struct tcphdr *tcp = (struct tcphdr*)(buffer + sizeof(struct ethhdr) + ip_len);
        int tcp_len = tcp->doff * 4;

        unsigned char *payload =
            buffer + sizeof(struct ethhdr) + ip_len + tcp_len;

        int payload_len = n - sizeof(struct ethhdr) - ip_len - tcp_len;

        printf("\n----- TCP Packet -----\n");
        printf("%s:%d -> %s:%d\n",
               src_ip, ntohs(tcp->source),
               dst_ip, ntohs(tcp->dest));

        printf("SEQ=%u  ACK=%u  Flags:",
               ntohl(tcp->seq), ntohl(tcp->ack_seq));

        if (tcp->syn) printf(" SYN");
        if (tcp->ack) printf(" ACK");
        if (tcp->fin) printf(" FIN");
        if (tcp->rst) printf(" RST");
        printf("\n");

        printf("Payload length: %d bytes\n", payload_len);

        if (payload_len > 0) {
            int show = payload_len < PAYLOAD_MAX ? payload_len : PAYLOAD_MAX;
            hexdump(payload, show);
        }
    }

    printf("Stopped.\n");

    free(buffer);
    close(sockfd);
    return 0;
}

