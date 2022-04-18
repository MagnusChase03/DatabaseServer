#include <stdint.h>

#define MAX_NAME_LENGTH 128

// Message types
#define PUT 1
#define GET 2
#define DEL 3
#define SUCCESS 4
#define FAIL 5

// record stored in the data base
struct record
{
    char name[MAX_NAME_LENGTH]; // name should not be null
    uint32_t id;                // id of the record
    char pad[380];              // size of the record is aligned to 512 bytes
};

// message structure
struct msg
{
    uint8_t type;
    struct record rd;
}