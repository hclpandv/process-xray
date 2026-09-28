#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct {
    unsigned long start;
    unsigned long end;
    char permissions[5];
} MemoryRegion;

int find_memory_region(
    const char *maps_path,
    unsigned long address,
    MemoryRegion *result
)
{
    FILE *file = fopen(maps_path, "r");

    if (file == NULL) {
        return 0;
    }

    char line[512];

    while (fgets(line, sizeof(line), file) != NULL) {

        MemoryRegion region;

        int parsed = sscanf(
            line,
            "%lx-%lx %4s",
            &region.start,
            &region.end,
            region.permissions
        );

        if (parsed != 3) {
            continue;
        }

        if (address >= region.start &&
            address < region.end) {

            *result = region;

            fclose(file);

            return 1;
        }
    }

    fclose(file);

    return 0;
}

int main(void)
{
    printf("Process X-Ray v0.4.1\n");
    printf("====================\n\n");

    pid_t pid = getpid();

    printf("PID: %d\n", pid);

    long page_size = sysconf(_SC_PAGESIZE);

    printf("Page size: %ld bytes\n", page_size);

    printf("\nCreating memory mapping...\n");

    void *memory = mmap(
        NULL,
        page_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (memory == MAP_FAILED) {
        printf("mmap failed\n");
        return 1;
    }

    printf("Mapping address: %p\n", memory);
    printf("Mapping size:    %ld bytes\n", page_size);

    int *number = memory;

    *number = 42;

    printf("\nMapped memory\n");
    printf("-------------\n");
    printf("Value:   %d\n", *number);
    printf("Address: %p\n", (void *)number);

    char maps_path[64];

    snprintf(
        maps_path,
        sizeof(maps_path),
        "/proc/%d/maps",
        pid
    );

    MemoryRegion region;

    unsigned long address =
        (unsigned long)memory;

    if (find_memory_region(
            maps_path,
            address,
            &region)) {

        printf("\nContaining memory region\n");
        printf("------------------------\n");

        printf(
            "Start:       0x%lx\n",
            region.start
        );

        printf(
            "End:         0x%lx\n",
            region.end
        );

        printf(
            "Size:        %lu KB\n",
            (region.end - region.start) / 1024
        );

        printf(
            "Permissions: %s\n",
            region.permissions
        );

    } else {
        printf("\nCould not find containing region.\n");
    }

    printf("\nPress Enter to unmap and exit...\n");

    getchar();

    if (munmap(memory, page_size) != 0) {
        printf("munmap failed\n");
        return 1;
    }

    printf("Memory mapping removed.\n");

    return 0;
}