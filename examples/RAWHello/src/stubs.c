
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "phnx02.h"

static inline int _stub(int err) { return -1; }

void *_sbrk(ptrdiff_t incr) {
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) ||
        (curbrk + incr > _heap_end)) // cppcheck-suppress comparePointers
        return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}

int _close(int fd) { return _stub(EBADF); }

int _fstat(int fd, struct stat *st) {
    if (isatty(fd)) {
        st->st_mode = S_IFCHR;
        return 0;
    }
    return _stub(EBADF);
}

int _isatty(int fd) {
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
        return 1;

    return 0;
}

off_t _lseek(int fd, off_t ptr, int dir) {
    if (isatty(fd))
        return 0;

    return _stub(EBADF);
}

ssize_t _read(int fd, void *ptr, size_t len) { return _stub(EBADF); }

#ifndef _UART2
#define DBGPORT UART1
#else
#define DBGPORT UART2
#endif

ssize_t _write(int fd, const void *ptr, size_t len) {
    const uint8_t *current = (const uint8_t *)ptr;
    if (isatty(fd)) {
        for (size_t jj = 0; jj < len; jj++) {
            DBGPORT->SBUF = current[jj]; // sbuf
            while (((DBGPORT->ISR) & 0x00000001) != 0x00000001) {
            };
            DBGPORT->ISR = (0xff << 0); // clear intf
            if (current[jj] == '\n') {
                DBGPORT->SBUF = '\r';
                while (((DBGPORT->ISR) & 0x00000001) != 0x00000001) {
                };
                DBGPORT->ISR = (0xff << 0); // clear intf
            }
        }
        return len;
    }
    return _stub(EBADF);
}
