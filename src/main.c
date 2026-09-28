#include <stdio.h>
#include <unistd.h>

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
    printf("Process X-Ray v0.3.4\n");
    printf("====================\n\n");

    int local_variable = 42;

    printf("Local variable\n");
    printf("--------------\n");
    printf("Value:   %d\n", local_variable);
    printf("Address: %p\n", (void *)&local_variable);

    pid_t pid = getpid();

    printf("\nPID: %d\n", pid);

    long page_size = sysconf(_SC_PAGESIZE);

    printf("Page size: %ld bytes\n", page_size);

    char maps_path[64];

    snprintf(
        maps_path,
        sizeof(maps_path),
        "/proc/%d/maps",
        pid
    );

    MemoryRegion region;

    unsigned long address =
        (unsigned long)&local_variable;

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

        unsigned long mapping_offset =
            address - region.start;

        printf(
            "Offset:      0x%lx\n",
            mapping_offset
        );

        unsigned long page_offset =
            address % page_size;

        printf(
            "Page offset: 0x%lx\n",
            page_offset
        );

        unsigned long page_start =
            address - page_offset;

        unsigned long page_end =
            page_start + page_size;

        printf("\nContaining page\n");
        printf("----------------\n");

        printf(
            "Page start:  0x%lx\n",
            page_start
        );

        printf(
            "Page end:    0x%lx\n",
            page_end
        );

        printf(
            "Page size:   %ld bytes\n",
            page_size
        );

    } else {
        printf("\nCould not find containing region.\n");
    }

    return 0;
}