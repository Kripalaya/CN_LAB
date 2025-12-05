/*
    tcp_raw_roll_multi.c
    Sends multiple raw TCP packets containing ROLL_NUMBER as payload.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define TARGET_IP   "10.0.0.2"
#define TARGET_PORT 9000
#define SRC_IP      "10.0.0.1"
#define SRC_PORT    54321
#define ROLL_NUMBER "CSM24038"

unsigned short checksum(unsigned short *buf, int nwords) {
    unsigned long sum = 0;
    for (; nwords > 0; nwords--) sum += *buf++;
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    return (unsigned short)(~sum);
}

unsigned short tcp_checksum(const struct iphdr *iph, const struct tcphdr *tcph,
                            const unsigned char *payload, int payloadlen)
{
    struct {
        uint32_t saddr, daddr;
        uint8_t zero;
        uint8_t proto;
        uint16_t tcp_len;
    } pseudo;

    int header_and_payload_len = sizeof(struct tcphdr) + payloadlen;

    int total_len = sizeof(pseudo) + header_and_payload_len;
    unsigned char *buf = malloc(total_len);
    unsigned char *p = buf;

    pseudo.saddr = iph->saddr;
    pseudo.daddr = iph->daddr;
    pseudo.zero = 0;
    pseudo.proto = IPPROTO_TCP;
    pseudo.tcp_len = htons(header_and_payload_len);

    memcpy(p, &pseudo, sizeof(pseudo)); p += sizeof(pseudo);
    memcpy(p, tcph, sizeof(struct tcphdr)); p += sizeof(struct tcphdr);
    memcpy(p, payload, payloadlen);

    unsigned short res = checksum((unsigned short*)buf, total_len/2 + total_len%2);
    free(buf);
    return res;
}

int main() {
    int sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sd < 0) { perror("socket"); return 1; }

    int one = 1;
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt IP_HDRINCL");
        return 1;
    }

    const char *payload = ROLL_NUMBER;
    int payloadlen = strlen(payload);

    int packet_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + payloadlen;
    unsigned char *packet = malloc(packet_len);

    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (struct tcphdr *)(packet + sizeof(struct iphdr));
    unsigned char *data = packet + sizeof(struct iphdr) + sizeof(struct tcphdr);

    memcpy(data, payload, payloadlen);

    /* Destination */
    struct sockaddr_in dst;
    dst.sin_family = AF_INET;
    dst.sin_port = htons(TARGET_PORT);
    dst.sin_addr.s_addr = inet_addr(TARGET_IP);

    /* Send multiple packets */
    int count = 10;   // <-- Send 10 packets (change this if needed)

    for (int i = 0; i < count; i++) {
        memset(packet, 0, packet_len);

        /* IP header */
        iph->ihl = 5;
        iph->version = 4;
        iph->tot_len = htons(packet_len);
        iph->id = htons(10000 + i);  // different ID per packet
        iph->ttl = 64;
        iph->protocol = IPPROTO_TCP;
        iph->saddr = inet_addr(SRC_IP);
        iph->daddr = inet_addr(TARGET_IP);
        iph->check = checksum((unsigned short*)iph, sizeof(struct iphdr)/2);

        /* TCP header */
        tcph->source = htons(SRC_PORT);
        tcph->dest = htons(TARGET_PORT);
        tcph->seq = htonl(0x1000 + i);  // different sequence
        tcph->ack_seq = 0;
        tcph->doff = sizeof(struct tcphdr) / 4;
        tcph->psh = 1;
        tcph->syn = 1;
        tcph->window = htons(65535);
        tcph->check = 0;

        /* Copy payload again */
        memcpy(data, payload, payloadlen);

        /* TCP checksum */
        tcph->check = tcp_checksum(iph, tcph, data, payloadlen);

        /* SEND */
        ssize_t sent = sendto(sd, packet, packet_len, 0,
                              (struct sockaddr*)&dst, sizeof(dst));

        if (sent > 0)
            printf("Packet %d sent (%zd bytes). Payload=\"%s\"\n",
                    i+1, sent, payload);
        else
            perror("sendto");

        usleep(200000); // small delay so Wireshark separates packets
    }

    free(packet);
    close(sd);
    return 0;
}

