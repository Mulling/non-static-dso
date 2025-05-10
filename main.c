#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void foo(void);

void show_maps(void) {
    FILE* file = fopen("/proc/self/maps", "r");

    if (file == NULL) {
        perror(strerror(errno));
    exit:
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    goto exit;
}

void load_dso(char const* file) {
    void* handle = dlopen(file, RTLD_LOCAL | RTLD_NOW);

    if (handle == NULL) {
    err:
        perror(strerror(errno));
        exit(1);
    }

    void (*fn)(void);
    *(void**)(&fn) = dlsym(handle, "foo");

    if (fn == NULL)
        goto err;

    fn();

    if (dlclose(handle) < 0)
        goto err;
}

int main() {
    puts("--- before loading lib1 ---");
    show_maps();
    load_dso("./lib1.so");
    puts("--- after unloading lib1 ---");
    show_maps();

    load_dso("./lib2.so");

    return 0;
}
