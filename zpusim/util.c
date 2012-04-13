#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

uint8_t *file_read(const char *name, size_t *retlen)
{
    int fd;
    size_t count;
    uint8_t *buf, *p;
    struct stat st;
    size_t len;

    if ((fd = open(name, O_RDONLY)) < 0)
        return NULL;

    if (fstat(fd, &st) < 0)
        return NULL;

    if (st.st_size == 0)
        return NULL;

    if (NULL == (buf = (uint8_t *)malloc(st.st_size+1)))   // +1 for null termination
        return NULL;

    p = buf;
    len = st.st_size;
    while(((count = read(fd, p, len)) > 0) && len > 0)
    {
        p += count;
        len -= count;
    }
    if (len!=0)
    {
        free(buf);
        return NULL;
    }

    *retlen = st.st_size;
    return buf;
}

void dump(const uint8_t *data, size_t len)
{
    size_t i;
    size_t off = 0;

    while(off < len)
    {
        size_t n, pad = 0;
        if (len-off > 16)
            n = 16;
        else
        {
            n = len - off;
            pad = 16 - n;
        }

        fprintf(stderr, "%04X: ", (int)off);

        for (i=off;i<off+n;i++)
            fprintf(stderr, "%02X", data[i]);
        for (i=0;i<pad;i++)
            fprintf(stderr, "  ");
        fprintf(stderr, "    ");
        for (i=off;i<off+n;i++)
            fprintf(stderr, "%c", isprint(data[i]) ? data[i] : '.');
        fprintf(stderr, "\r\n");

        off += n;
    }
}

