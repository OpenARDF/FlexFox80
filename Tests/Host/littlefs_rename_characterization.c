#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lfs.h"

enum { BLOCK_SIZE = 8192, BLOCK_COUNT = 125, IO_SIZE = 64 };

static uint8_t flash_bytes[BLOCK_SIZE * BLOCK_COUNT];
static uint64_t reads;
static uint64_t progs;
static uint64_t erases;

static int flash_read(const struct lfs_config *cfg, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    (void)cfg;
    reads++;
    memcpy(buffer, &flash_bytes[(size_t)block * BLOCK_SIZE + off], size);
    return 0;
}

static int flash_prog(const struct lfs_config *cfg, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    (void)cfg;
    progs++;
    uint8_t *dst = &flash_bytes[(size_t)block * BLOCK_SIZE + off];
    const uint8_t *src = buffer;
    for (lfs_size_t i = 0; i < size; i++) {
        if ((dst[i] & src[i]) != src[i]) return LFS_ERR_CORRUPT;
        dst[i] &= src[i];
    }
    return 0;
}

static int flash_erase(const struct lfs_config *cfg, lfs_block_t block) {
    (void)cfg;
    erases++;
    memset(&flash_bytes[(size_t)block * BLOCK_SIZE], 0xff, BLOCK_SIZE);
    return 0;
}

static int flash_sync(const struct lfs_config *cfg) {
    (void)cfg;
    return 0;
}

static void check(int result, const char *operation) {
    if (result < 0) {
        fprintf(stderr, "%s failed: %d\n", operation, result);
        exit(2);
    }
}

static void write_file(lfs_t *lfs, const char *path, uint32_t seed, size_t size) {
    uint8_t bytes[128];
    lfs_file_t file;
    check(lfs_file_open(lfs, &file, path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC), "open");
    for (size_t offset = 0; offset < size; offset += sizeof(bytes)) {
        size_t chunk = size - offset < sizeof(bytes) ? size - offset : sizeof(bytes);
        for (size_t i = 0; i < chunk; i++) bytes[i] = (uint8_t)(seed + offset + i);
        lfs_ssize_t written = lfs_file_write(lfs, &file, bytes, (lfs_size_t)chunk);
        if (written != (lfs_ssize_t)chunk) {
            fprintf(stderr, "%s short write at %zu: %" PRId32 "\n", path, offset, written);
            exit(2);
        }
    }
    check(lfs_file_close(lfs, &file), "close");
}

static void verify_file(lfs_t *lfs, const char *path, uint32_t seed, size_t size) {
    uint8_t bytes[128];
    lfs_file_t file;
    check(lfs_file_open(lfs, &file, path, LFS_O_RDONLY), "open for verification");
    for (size_t offset = 0; offset < size; offset += sizeof(bytes)) {
        size_t chunk = size - offset < sizeof(bytes) ? size - offset : sizeof(bytes);
        lfs_ssize_t count = lfs_file_read(lfs, &file, bytes, (lfs_size_t)chunk);
        if (count != (lfs_ssize_t)chunk) {
            fprintf(stderr, "%s short read at %zu: %" PRId32 "\n", path, offset, count);
            exit(2);
        }
        for (size_t i = 0; i < chunk; i++) {
            if (bytes[i] != (uint8_t)(seed + offset + i)) {
                fprintf(stderr, "%s content mismatch at %zu\n", path, offset + i);
                exit(2);
            }
        }
    }
    if (lfs_file_read(lfs, &file, bytes, 1) != 0) {
        fprintf(stderr, "%s contains unexpected trailing data\n", path);
        exit(2);
    }
    check(lfs_file_close(lfs, &file), "close after verification");
}

static void load_image(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("open image for read");
        exit(2);
    }
    if (fread(flash_bytes, 1, sizeof(flash_bytes), file) != sizeof(flash_bytes) ||
            fgetc(file) != EOF) {
        fprintf(stderr, "filesystem image has the wrong size: %s\n", path);
        exit(2);
    }
    if (fclose(file) != 0) {
        perror("close image after read");
        exit(2);
    }
}

static void save_image(const char *path) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        perror("open image for write");
        exit(2);
    }
    if (fwrite(flash_bytes, 1, sizeof(flash_bytes), file) != sizeof(flash_bytes)) {
        fprintf(stderr, "could not write complete filesystem image: %s\n", path);
        exit(2);
    }
    if (fclose(file) != 0) {
        perror("close image after write");
        exit(2);
    }
}

int main(int argc, char **argv) {
    struct lfs_config cfg = {
        .read = flash_read,
        .prog = flash_prog,
        .erase = flash_erase,
        .sync = flash_sync,
        .read_size = IO_SIZE,
        .prog_size = IO_SIZE,
        .block_size = BLOCK_SIZE,
        .block_count = BLOCK_COUNT,
        .block_cycles = 16,
        .cache_size = IO_SIZE,
        .lookahead_size = IO_SIZE,
    };
    lfs_t lfs;

    if (argc == 3 && strcmp(argv[1], "compat-create") == 0) {
        memset(flash_bytes, 0xff, sizeof(flash_bytes));
        check(lfs_format(&lfs, &cfg), "compatibility format");
        check(lfs_mount(&lfs, &cfg), "compatibility mount after format");
        write_file(&lfs, "Target.event", 7, 1200);
        write_file(&lfs, "Marker.txt", 42, 256);
        check(lfs_unmount(&lfs), "compatibility unmount after create");
        save_image(argv[2]);
        puts("compat-create=pass");
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "compat-replace") == 0) {
        load_image(argv[2]);
        check(lfs_mount(&lfs, &cfg), "compatibility mount before replace");
        verify_file(&lfs, "Target.event", 7, 1200);
        verify_file(&lfs, "Marker.txt", 42, 256);
        write_file(&lfs, "Temp", 99, 1200);
        check(lfs_rename(&lfs, "Temp", "Target.event"), "compatibility rename");
        verify_file(&lfs, "Target.event", 99, 1200);
        check(lfs_unmount(&lfs), "compatibility unmount after replace");
        save_image(argv[2]);
        puts("compat-replace=pass");
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "compat-verify") == 0) {
        load_image(argv[2]);
        check(lfs_mount(&lfs, &cfg), "compatibility mount for rollback verification");
        verify_file(&lfs, "Target.event", 99, 1200);
        verify_file(&lfs, "Marker.txt", 42, 256);
        struct lfs_info info;
        if (lfs_stat(&lfs, "Temp", &info) != LFS_ERR_NOENT) {
            fprintf(stderr, "Temp unexpectedly remained after compatibility rename\n");
            return 2;
        }
        check(lfs_unmount(&lfs), "compatibility rollback unmount");
        puts("compat-verify=pass");
        return 0;
    }

    unsigned rounds = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 3000;
    memset(flash_bytes, 0xff, sizeof(flash_bytes));
    check(lfs_format(&lfs, &cfg), "format");
    check(lfs_mount(&lfs, &cfg), "mount");

    char path[33];
    for (unsigned i = 0; i < 40; i++) {
        snprintf(path, sizeof(path), "Asset-%02u.event", i);
        write_file(&lfs, path, i, i < 9 ? 1200 : 256);
    }

    uint64_t max_reads = 0;
    uint64_t max_progs = 0;
    uint64_t max_erases = 0;
    unsigned max_round = 0;
    uint64_t total_reads = 0;
    for (unsigned round = 0; round < rounds; round++) {
        write_file(&lfs, "Temp", round, 1200);
        snprintf(path, sizeof(path), "Asset-%02u.event", round % 9);
        reads = progs = erases = 0;
        check(lfs_rename(&lfs, "Temp", path), "rename");
        total_reads += reads;
        if (reads > max_reads) {
            max_reads = reads;
            max_progs = progs;
            max_erases = erases;
            max_round = round;
        }
    }

    printf("rounds=%u total_reads=%" PRIu64 " max_round=%u max_reads=%" PRIu64
           " max_progs=%" PRIu64 " max_erases=%" PRIu64 "\n",
           rounds, total_reads, max_round, max_reads, max_progs, max_erases);
    check(lfs_unmount(&lfs), "unmount");
    return 0;
}
