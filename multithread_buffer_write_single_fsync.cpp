#include <fcntl.h>
#include <unistd.h>

#include <thread>
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstdlib>

/*
[file_size = 5M]
multithread_buffer_write_single_fsync, time(ms) = 35
multithread_buffer_write_single_fsync, time(ms) = 22
multithread_buffer_write_single_fsync, time(ms) = 19
multithread_buffer_write_single_fsync, time(ms) = 20
multithread_buffer_write_single_fsync, time(ms) = 11

[file_size = 10G]
multithread_buffer_write_single_fsync, time(ms) = 17004
multithread_buffer_write_single_fsync, time(ms) = 17733
multithread_buffer_write_single_fsync, time(ms) = 18641
multithread_buffer_write_single_fsync, time(ms) = 18247
multithread_buffer_write_single_fsync, time(ms) = 18651
*/

int num_threads;
std::vector<int> fds;

void write_to_file(int thread_id, long long file_size, int block_size, const std::vector<char>& buf) {
    int fd = fds.at(thread_id);
    for (long long i = 0; i < file_size / block_size; i++) {
        if (i % num_threads == thread_id) {
            lseek(fd, block_size * i, SEEK_SET);
            int num = write(fd, &buf[0], block_size);
            assert(num == block_size);
        }
    }
    int ret_sync = fsync(fd);
    assert(ret_sync == 0);
}

int main() {
    num_threads = 10;
    for (int i = 0; i < num_threads; i++) {
        FILE* file = tmpfile();
        int fd = fileno(file);
        // std::string file_name = "file" + std::to_string(i);
        // int fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
        assert(fd != -1);
        fds.push_back(fd);
    }
        
    int block_size = 4 << 10; // 4K
    long long file_size = 10LL << 30; // 10GB
    // long long file_size = 5 << 20;

    std::vector<char> buf(block_size);
    char val = '0' + 1;
    std::fill(&buf[0], &buf[0] + block_size, val);

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(write_to_file, i, file_size, block_size, std::ref(buf));
    }
    for (int i = 0; i < num_threads; i++) {
        threads.at(i).join();
    }
    const auto end = std::chrono::steady_clock::now();
    const auto duration = end - start;

    std::cout << "multithread_buffer_write_single_fsync, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    for (int i = 0; i < num_threads; i++) {
        close(fds.at(i));
    }
}