#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdint.h>

#define MAX_PAYLOAD_LEN 1024 /* max possible udp packet length */

typedef struct __attribute__((packed))
{
    uint32_t length;
    uint8_t  type;  // reserved
    uint8_t  flags; // reserved
    uint8_t  rcode; // reserved
} header_t;

typedef struct __attribute__((packed))
{
    header_t header;
    uint8_t payload[0];
} message_t;

#endif /* MESSAGES_H */