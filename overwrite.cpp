#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstdlib>

/*
[file_size = 5M]
overwrite, time(ms) = 2562
overwrite, time(ms) = 2647
overwrite, time(ms) = 2614
overwrite, time(ms) = 2703
overwrite, time(ms) = 2665
*/

int main() {
    FILE* file = tmpfile();
    int fd = fileno(file);
    assert(fd != -1);
    int block_size = 4 << 10; // 4K
    int file_size = 5 << 20; // 5M

    std::vector<char> buf(block_size);
    char val = '0';
    std::fill(&buf[0], &buf[0] + block_size, val);

    for (int i = 0; i < file_size / block_size; i++) {
        int num = write(fd, &(buf[0]), block_size);
        assert(num == block_size);
        int ret_sync = fsync(fd);
        assert(ret_sync == 0);
    }

    val = '0' + 1;
    std::fill(&buf[0], &buf[0] + block_size, val);

    lseek(fd, 0, SEEK_SET);
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < file_size / block_size; i++) {
        int num = write(fd, &(buf[0]), block_size);
        assert(num == block_size);
        int ret_sync = fsync(fd);
        assert(ret_sync == 0);
    }
    const auto end = std::chrono::steady_clock::now();
    const auto duration = end - start;

    std::cout << "overwrite, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    close(fd);
}