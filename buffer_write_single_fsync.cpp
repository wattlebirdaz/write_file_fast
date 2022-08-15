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
buffer_write_single_fsync, time(ms) = 15
buffer_write_single_fsync, time(ms) = 7
buffer_write_single_fsync, time(ms) = 7
buffer_write_single_fsync, time(ms) = 7
buffer_write_single_fsync, time(ms) = 7

[file_size = 10G]
buffer_write_single_fsync, time(ms) = 10028
buffer_write_single_fsync, time(ms) = 12022
buffer_write_single_fsync, time(ms) = 12224
buffer_write_single_fsync, time(ms) = 12509
buffer_write_single_fsync, time(ms) = 12368
*/

int main() {
    FILE* file = tmpfile();
    int fd = fileno(file);
    assert(fd != -1);
    int block_size = 4 << 10; // 4K
    //long long file_size = 10LL << 30; // 10GB
    long long file_size = 5 << 20;

    std::vector<char> buf(block_size);
    char val = '0' + 1;
    std::fill(&buf[0], &buf[0] + block_size, val);

    const auto start = std::chrono::steady_clock::now();
    for (long long i = 0; i < file_size / block_size; i++) {
        int num = write(fd, &(buf[0]), block_size);
        assert(num == block_size);
    }
    int ret_sync = fsync(fd);
    assert(ret_sync == 0);
    const auto end = std::chrono::steady_clock::now();
    const auto duration = end - start;

    std::cout << "buffer_write_single_fsync, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    close(fd);
}