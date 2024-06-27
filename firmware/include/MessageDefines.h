/*
Every message will include a header, body, and footer

The header and footer have fixed size, the body can be any size


*/
#define PACKEDSTRUCT typedef struct __attribute__((packed))

enum MessageID
{
        
};

enum MessageCommand{
    None,
    Read,
    Write
};


PACKEDSTRUCT MessageHeader
{

};

PACKEDSTRUCT MessageFooter
{

};

