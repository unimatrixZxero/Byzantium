/*
Copyright (c) 2007, 2008 by Juliusz Chroboczek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#if defined(i386) || defined(__mc68020__) || defined(__x86_64__)
#define DO_NTOHS(_d, _s) do { _d = ntohs(*(unsigned short*)(_s)); } while(0)
#define DO_NTOHL(_d, _s) do { _d = ntohl(*(unsigned*)(_s)); } while(0)
#define DO_HTONS(_d, _s) do { *(unsigned short*)(_d) = htons(_s); } while(0)
#define DO_HTONL(_d, _s) do { *(unsigned*)(_d) = htonl(_s); } while(0)
/* Some versions of gcc seem to be buggy, and ignore the packed attribute.
   Disable this code until the issue is clarified. */
/* #elif defined __GNUC__*/
#elif 0
struct __us { unsigned short x __attribute__((packed)); };
#define DO_NTOHS(_d, _s) \
    do { _d = ntohs(((const struct __us*)(_s))->x); } while(0)
#define DO_HTONS(_d, _s) \
    do { ((struct __us*)(_d))->x = htons(_s); } while(0)
#else
#define DO_NTOHS(_d, _s) \
    do { short _dd; \
         memcpy(&(_dd), (_s), 2); \
         _d = ntohs(_dd); } while(0)
#define DO_HTONS(_d, _s) \
    do { unsigned short _dd; \
         _dd = htons(_s); \
         memcpy((_d), &(_dd), 2); } while(0)
#endif

static inline int
seqno_compare(unsigned short s1, unsigned short s2)
{
    if(s1 == s2)
        return 0;
    else
        return ((s2 - s1) & 0x8000) ? 1 : -1;
}

static inline short
seqno_minus(unsigned short s1, unsigned short s2)
{
    return (short)((s1 - s2) & 0xFFFF);
}

static inline unsigned short
seqno_plus(unsigned short s, int plus)
{
    return ((s + plus) & 0xFFFF);
}

unsigned roughly(unsigned value);
void timeval_minus(struct timeval *d,
                   const struct timeval *s1, const struct timeval *s2);
unsigned timeval_minus_msec(const struct timeval *s1, const struct timeval *s2)
    ATTRIBUTE ((pure));
void timeval_plus_msec(struct timeval *d,
                       const struct timeval *s, int msecs);
int timeval_compare(const struct timeval *s1, const struct timeval *s2)
    ATTRIBUTE ((pure));
void timeval_min(struct timeval *d, const struct timeval *s);
void timeval_min_sec(struct timeval *d, time_t secs);
int parse_msec(const char *string) ATTRIBUTE ((pure));
void do_debugf(int level, const char *format, ...)
    ATTRIBUTE ((format (printf, 2, 3))) COLD;
int in_prefix(const unsigned char *restrict address,
              const unsigned char *restrict prefix, unsigned char plen)
    ATTRIBUTE ((pure));
unsigned char *mask_prefix(unsigned char *restrict ret,
                           const unsigned char *restrict prefix,
                           unsigned char plen);
const char *format_address(const unsigned char *address);
const char *format_prefix(const unsigned char *address, unsigned char prefix);
const char *format_eui64(const unsigned char *eui);
int parse_address(const char *address, unsigned char *addr_r, int *af_r);
int parse_net(const char *net, unsigned char *prefix_r, unsigned char *plen_r,
              int *af_r);
int parse_eui64(const char *eui, unsigned char *eui_r);
int wait_for_fd(int direction, int fd, int msecs);
int martian_prefix(const unsigned char *prefix, int plen) ATTRIBUTE ((pure));
int v4mapped(const unsigned char *address) ATTRIBUTE ((pure));
void v4tov6(unsigned char *dst, const unsigned char *src);
int daemonise(void);

/* If debugging is disabled, we want to avoid calling format_address
   for every omitted debugging message.  So debug is a macro.  But
   vararg macros are not portable. */
#if defined NO_DEBUG

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define debugf(...) do {} while(0)
#define kdebugf(...) do {} while(0)
#elif defined __GNUC__
#define debugf(_args...) do {} while(0)
#define kdebugf(_args...) do {} while(0)
#else
static inline void debugf(const char *format, ...) { return; }
static inline void kdebugf(const char *format, ...) { return; }
#endif

#else

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define debugf(...) \
    do { \
        if(UNLIKELY(debug >= 2)) do_debugf(2, __VA_ARGS__);     \
    } while(0)
#define kdebugf(...) \
    do { \
        if(UNLIKELY(debug >= 3)) do_debugf(3, __VA_ARGS__);     \
    } while(0)
#elif defined __GNUC__
#define debugf(_args...) \
    do { \
        if(UNLIKELY(debug >= 2)) do_debugf(2, _args);   \
    } while(0)
#define kdebugf(_args...) \
    do { \
        if(UNLIKELY(debug >= 3)) do_debugf(3, _args);   \
    } while(0)
#else
static inline void debugf(const char *format, ...) { return; }
static inline void kdebugf(const char *format, ...) { return; }
#endif

#endif

