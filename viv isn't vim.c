#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 检查是否提供了文件名作为命令行参数
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 打开用户指定的文件
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // 获取文件大小
    off_t filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // 映射文件到内存
    void *mapped_memory = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped_memory == MAP_FAILED) {
        perror("Error mapping file to memory");
        close(fd);
        return EXIT_FAILURE;
    }

    // 此时mapped_memory指向了文件的内容
    // 将文件内容打印到标准输出
    char *file_content = (char *)mapped_memory;
    while (*file_content) {
        putchar(*file_content++);
    }

    // 释放映射
    if (munmap(mapped_memory, filesize) == -1) {
        perror("Error unmapping memory");
    }

    // 关闭文件
    close(fd);

    return EXIT_SUCCESS;
}