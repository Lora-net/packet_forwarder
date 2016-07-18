/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2013 Semtech-Cycleo

Description:
    Network sink, receives UDP packets and sends an acknowledge

License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Sylvain Miermont
*/


/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

/* fix an issue between POSIX and C99 */
#if __STDC_VERSION__ >= 199901L
    #define _XOPEN_SOURCE 600
#else
    #define _XOPEN_SOURCE 500
#endif

#include <stdint.h>     /* C99 types */
#include <stdio.h>      /* printf, fprintf, sprintf, fopen, fputs */
#include <unistd.h>     /* usleep */

#include <string.h>     /* memset */
#include <time.h>       /* time, clock_gettime, strftime, gmtime, clock_nanosleep*/
#include <stdlib.h>     /* atoi, exit */
#include <errno.h>      /* error messages */

#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gai_strerror */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define STRINGIFY(x)    #x
#define STR(x)          STRINGIFY(x)
#define MSG(args...)    fprintf(stderr, args) /* message that is destined to the user */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define PROTOCOL_VERSION 2

#define PKT_PUSH_DATA    0
#define PKT_PUSH_ACK     1
#define PKT_PULL_DATA    2
#define PKT_PULL_RESP    3
#define PKT_PULL_ACK     4

/* -------------------------------------------------------------------------- */
/* --- MAIN FUNCTION -------------------------------------------------------- */

int main(int argc, char **argv)
{
    int i; /* loop variable and temporary variable for return value */

    /* server socket creation */
    int sock; /* socket file descriptor */
    struct addrinfo hints;
    struct addrinfo *result; /* store result of getaddrinfo */
    struct addrinfo *q; /* pointer to move into *result data */
    char host_name[64];
    char port_name[64];

    /* variables for receiving and sending packets */
    struct sockaddr_storage dist_addr;
    socklen_t addr_len = sizeof dist_addr;
    uint8_t databuf[4096];
    int byte_nb;

    /* variables for protocol management */
    uint32_t raw_mac_h; /* Most Significant Nibble, network order */
    uint32_t raw_mac_l; /* Least Significant Nibble, network order */
    uint64_t gw_mac; /* MAC address of the client (gateway) */
    uint8_t ack_command;

    /* check if port number was passed as parameter */
    if (argc != 2) {
        MSG("Usage: util_ack <port number>\n");
        exit(EXIT_FAILURE);
    }

    /* prepare hints to open network sockets */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* should handle IP v4 or v6 automatically */
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; /* will assign local IP automatically */

    /* look for address */
    i = getaddrinfo(NULL, argv[1], &hints, &result);
    if (i != 0) {
        MSG("ERROR: getaddrinfo returned %s\n", gai_strerror(i));
        exit(EXIT_FAILURE);
    }

    /* try to open socket and bind it */
    for (q=result; q!=NULL; q=q->ai_next) {
        sock = socket(q->ai_family, q->ai_socktype,q->ai_protocol);
        if (sock == -1) {
            continue; /* socket failed, try next field */
        } else {
            i = bind(sock, q->ai_addr, q->ai_addrlen);
            if (i == -1) {
                shutdown(sock, SHUT_RDWR);
                continue; /* bind failed, try next field */
            } else {
                break; /* success, get out of loop */
            }
        }
    }
    if (q == NULL) {
        MSG("ERROR: failed to open socket or to bind to it\n");
        i = 1;
        for (q=result; q!=NULL; q=q->ai_next) {
            getnameinfo(q->ai_addr, q->ai_addrlen, host_name, sizeof host_name, port_name, sizeof port_name, NI_NUMERICHOST);
            MSG("INFO: result %i host:%s service:%s\n", i, host_name, port_name);
            ++i;
        }
        exit(EXIT_FAILURE);
    }
    MSG("INFO: util_ack listening on port %s\n", argv[1]);
    freeaddrinfo(result);

    while (1) {
        /* wait to receive a packet */
        byte_nb = recvfrom(sock, databuf, sizeof databuf, 0, (struct sockaddr *)&dist_addr, &addr_len);
        if (byte_nb == -1) {
            MSG("ERROR: recvfrom returned %s \n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* display info about the sender */
        i = getnameinfo((struct sockaddr *)&dist_addr, addr_len, host_name, sizeof host_name, port_name, sizeof port_name, NI_NUMERICHOST);
        if (i == -1) {
            MSG("ERROR: getnameinfo returned %s \n", gai_strerror(i));
            exit(EXIT_FAILURE);
        }
        printf(" -> pkt in , host %s (port %s), %i bytes", host_name, port_name, byte_nb);

        /* check and parse the payload */
        if (byte_nb < 12) { /* not enough bytes for packet from gateway */
            printf(" (too short for GW <-> MAC protocol)\n");
            continue;
        }
        /* don't touch the token in position 1-2, it will be sent back "as is" for acknowledgement */
        if (databuf[0] != PROTOCOL_VERSION) { /* check protocol version number */
            printf(", invalid version %u\n", databuf[0]);
            continue;
        }
        raw_mac_h = *((uint32_t *)(databuf+4));
        raw_mac_l = *((uint32_t *)(databuf+8));
        gw_mac = ((uint64_t)ntohl(raw_mac_h) << 32) + (uint64_t)ntohl(raw_mac_l);

        /* interpret gateway command */
        switch (databuf[3]) {
            case PKT_PUSH_DATA:
                printf(", PUSH_DATA from gateway 0x%08X%08X\n", (uint32_t)(gw_mac >> 32), (uint32_t)(gw_mac & 0xFFFFFFFF));
                ack_command = PKT_PUSH_ACK;
                printf("<-  pkt out, PUSH_ACK for host %s (port %s)", host_name, port_name);
                break;
            case PKT_PULL_DATA:
                printf(", PULL_DATA from gateway 0x%08X%08X\n", (uint32_t)(gw_mac >> 32), (uint32_t)(gw_mac & 0xFFFFFFFF));
                ack_command = PKT_PULL_ACK;
                printf("<-  pkt out, PULL_ACK for host %s (port %s)", host_name, port_name);
                break;
            default:
                printf(", unexpected command %u\n", databuf[3]);
                continue;
        }

        /* add some artificial latency */
        usleep(30000); /* 30 ms */

        /* send acknowledge and check return value */
        databuf[3] = ack_command;
        byte_nb = sendto(sock, (void *)databuf, 4, 0, (struct sockaddr *)&dist_addr, addr_len);
        if (byte_nb == -1) {
            printf(", send error:%s\n", strerror(errno));
        } else {
            printf(", %i bytes sent\n", byte_nb);
        }
    }
}
