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
buffer_write_big, time(ms) = 15
buffer_write_big, time(ms) = 7
buffer_write_big, time(ms) = 6
buffer_write_big, time(ms) = 7
buffer_write_big, time(ms) = 6
*/

int main() {
    FILE* file = tmpfile();
    int fd = fileno(file);
    assert(fd != -1);
    
    int file_size = 5 << 20; // 5M
    int block_size = file_size;

    std::vector<char> buf(file_size);
    char val = '0' + 1;
    std::fill(&buf[0], &buf[0] + block_size, val);

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < file_size / block_size; i++) {
        int num = write(fd, &(buf[0]), block_size);
        assert(num == block_size);
        int ret_sync = fsync(fd);
        assert(ret_sync == 0);
    }
    const auto end = std::chrono::steady_clock::now();
    const auto duration = end - start;

    std::cout << "buffer_write_big, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    close(fd);
}