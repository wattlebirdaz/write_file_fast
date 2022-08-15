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
fallocate_buffer_write, time(ms) = 4252
fallocate_buffer_write, time(ms) = 4312
fallocate_buffer_write, time(ms) = 4333
fallocate_buffer_write, time(ms) = 4246
fallocate_buffer_write, time(ms) = 4230

[file_size = 10M]
fallocate_buffer_write, time(ms) = 8413
fallocate_buffer_write, time(ms) = 8424
fallocate_buffer_write, time(ms) = 8603
fallocate_buffer_write, time(ms) = 8596
fallocate_buffer_write, time(ms) = 8559
*/

int main() {
    FILE* file = tmpfile();
    int fd = fileno(file);
    assert(fd != -1);
    int block_size = 4 << 10; // 4K
    int file_size = 5 << 20; // 5M
    fallocate(fd, 0, 0, file_size);

    std::vector<char> buf(block_size);
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

    std::cout << "fallocate_buffer_write, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    close(fd);
}