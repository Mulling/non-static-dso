#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

const uint8_t payload[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00,              // mov eax, 1 (write)
    0xbf, 0x01, 0x00, 0x00, 0x00,              // mov edi, 1 (stdout)
    0x48, 0xbe, 0x00, 0x00, 0x00,              //
    0x00, 0x00, 0x00, 0x00, 0x00,              // mov rsi, <msg addr>
    0xba, 0x00, 0x00, 0x00, 0x00,              // mov edx, <msg len>
    0x0f, 0x05,                                // syscall
    0xb8, 0x23, 0x00, 0x00, 0x00,              // mov eax, 35 (nanosleep)
    0x48, 0xbf, 0x00, 0x00, 0x00,              //
    0x00, 0x00, 0x00, 0x00, 0x00,              // mov rdi, <tv addr>
    0x48, 0xc7, 0xc6, 0x00, 0x00, 0x00, 0x00,  // mov rsi, 0 (NULL)
    0x0f, 0x05,                                // syscall
    0xeb, 0xCB                                 // jmp short -31
};

#define MSG_ADDR_OFF 12
#define MSG_LEN_OFF 21
#define TV_OFF 34

void foo(void) {
    pthread_t thread;

    void* mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (mem == MAP_FAILED) {
    err:
        perror(strerror(errno));
        return;
    }

    const char msg[] = "hello from lib1\n";
    const uint32_t msg_len = sizeof(msg);

    size_t payload_size = sizeof(payload);

    struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };

    uint64_t const msg_addr = (uint64_t)mem + payload_size;
    uint64_t const tv_off = (payload_size + msg_len + 7) & ~7;
    uint64_t const tv_addr = (uint64_t)mem + tv_off;

    memcpy(mem, payload, sizeof(payload));
    memcpy((char*)mem + payload_size, msg, sizeof(msg));
    memcpy((uint8_t*)mem + tv_off, (void*)&tv, sizeof(tv));
    memcpy((uint8_t*)mem + MSG_ADDR_OFF, &msg_addr, sizeof(msg_addr));
    memcpy((uint8_t*)mem + MSG_LEN_OFF, &msg_len, sizeof(msg_len));
    memcpy((uint8_t*)mem + TV_OFF, &tv_addr, sizeof(tv_addr));

    void* (*fn)(void*);
    *(void**)(&fn) = mem;

    if (pthread_create(&thread, NULL, fn, NULL) != 0) {
        goto err;
    }

    puts("--- thread created ---");
}
