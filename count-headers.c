#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_FILEPATH	"headers.txt"

#define ARRAY_SIZE	512
#define TOKEN_SIZE	128

#define array_size(a)	(sizeof (a) / sizeof (*a))
#define print_row(s, n)	(printf("%-32.32s%6d\n", (s), (int)(n)))

extern char* strdup(const char* s);

static char* const headers[] = {"Accept",
                                "Cache-Control",
                                "Client-Date",
                                "Client-Peer",
                                "Client-Response-Num",
                                "Connection",
                                "Content-Disposition",
                                "Content-Encoding",
                                "Content-Length",
                                "Content-Location",
                                "Content-MD5",
                                "Content-Type",
                                "Date",
                                "Expires",
                                "HTTP",
                                "Keep-Alive",
                                "Last-Modified",
                                "Link",
                                "Location",
                                "Refresh",
                                "Server",
                                "Set-Cookie",
                                "Status",
                                "Transfer-Encoding",
                                "Upgrade",
                                "User-Agent",
                                "Via",
                                "WWW-Authenticate",
                                NULL};

static char** alloc_headers(size_t n)
{
    return calloc(n, sizeof(const char*));
}

static int compare_headers(const void* p, const void* q)
{
    return strcmp(*(const char**)p, *(const char**)q);
}

static char** copy_headers(char* const* v, size_t n)
{
    char** w = alloc_headers(n);
    char* const* p = v;
    char** q = w;

    assert ( w != NULL );

    for ( ; *p != NULL; p++, q++ )
        *q = strdup(*p);

    return w;
}

static size_t count_headers(char* const* v)
{
    char* const* p = v;

    for ( ; *p != NULL; p++ );
    return p - v;
}

static void free_headers(char** v)
{
    char** p = v;

    for ( ; *p != NULL; p++ )
        free(*p);

    free(v);
}

static void print_headers(char* const* v)
{
    char* const* p = v;

    for ( ; *p != NULL; p++ )
        printf("%s\n", *p);
}

static char** read_headers(const char* path, size_t n)
{
    FILE* fp = fopen(path, "r");
    char** p = calloc(n, sizeof *p);
    char** q = p;
    char header[TOKEN_SIZE];

    assert ( p != NULL );
    assert ( fp != NULL );

    while ( q < p + n - 1 )
        if ( fscanf(fp, "%[^:]: %*s\n", header) > 0 )
            *q++ = strdup(header);
        else
            break;

    fclose(fp);
    return p;
}

static void sort_headers(char** v)
{
    qsort(v, count_headers(v), sizeof *v, compare_headers);
}

static void tally_headers(char** v, char** w)
{
    char** p = v;
    char** q = w;
    char* s = NULL;
    size_t n = 0;
    int result;

    while ( *p != NULL && *q != NULL ) {
        result = strcmp(*p, *q);

        if ( result == 0 ) {
            if ( s == NULL )
                s = *p;

            n++;
            q++;
        }
        else if ( result > 0 ) {
            if ( s != NULL ) {
                print_row(s, n);
                s = NULL;
                n = 0;
            }

            q++;
        }
        else if ( result < 0 ) {
            if ( s != NULL ) {
                print_row(s, n);
                s = NULL;
                n = 0;
            }

            p++;
        }
    }

    if ( s != NULL )
        print_row(s, n);
}

int main(int argc, const char* argv[])
{
    char** p = copy_headers(headers, array_size(headers));
    char** q = read_headers(HEADER_FILEPATH, ARRAY_SIZE);

    sort_headers(p);
    sort_headers(q);

    if ( argc > 1 && strcmp(argv[1], "-v") == 0 ) {
        printf("Expected headers:\n\n");
        print_headers(p);
        printf("\n");
        printf("Actual headers:\n\n");
        print_headers(q);
        printf("\n");
    }

    printf("Header counts:\n\n");
    tally_headers(p, q);
    free_headers(p);
    free_headers(q);
}
