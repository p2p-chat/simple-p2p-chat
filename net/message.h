#ifndef MESSAGE_H
#define MESSAGE_H

typedef enum
{
    MESSAGE_LENGTH_MIN = 0,
    MESSAGE_LENGTH_MAX = 32*1024
} message_length_t;

typedef enum
{
    MESSAGE_TYPE_TEXT
} message_type_t;

typedef enum
{
    MESSAGE_CODE_OK = 0,
    MESSAGE_CODE_ERR
} message_code_t;

typedef struct __attribute__((packed))
{
    message_length_t length;
    message_type_t   type;
    message_code_t   code;
} header_t;

typedef struct __attribute__((packed))
{
    header_t header;
    uint8_t  payload[0];
} message_t;

#endif /* MESSAGE_H */