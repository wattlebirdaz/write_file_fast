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
[file_size = 500M]
fallocate_fillingzero_buffer_write, time(ms) = 4282
fallocate_fillingzero_buffer_write, time(ms) = 4294
fallocate_fillingzero_buffer_write, time(ms) = 4261
fallocate_fillingzero_buffer_write, time(ms) = 4269
fallocate_fillingzero_buffer_write, time(ms) = 4286

[file_size = 1G]
fallocate_fillingzero_buffer_write, time(ms) = 8638
fallocate_fillingzero_buffer_write, time(ms) = 8583
fallocate_fillingzero_buffer_write, time(ms) = 8537
fallocate_fillingzero_buffer_write, time(ms) = 8607
fallocate_fillingzero_buffer_write, time(ms) = 8618
*/

int main() {
    FILE* file = tmpfile();
    int fd = fileno(file);
    assert(fd != -1);
    int block_size = 4 << 10; // 4K
    int file_size = 5 << 20; // 500M
    fallocate(fd, FALLOC_FL_ZERO_RANGE, 0, file_size);

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

    std::cout << "fallocate_fillingzero_buffer_write, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    close(fd);
}