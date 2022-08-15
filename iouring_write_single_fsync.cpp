#include <fcntl.h>
#include <unistd.h>
#include <liburing.h>

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstdlib>


/*
[file_size = 5M]
iouring_write_single_fsync, time(ms) = 12
iouring_write_single_fsync, time(ms) = 3
iouring_write_single_fsync, time(ms) = 3
iouring_write_single_fsync, time(ms) = 3
iouring_write_single_fsync, time(ms) = 3

[file_size = 10G]
iouring_write_single_fsync, time(ms) = 507
iouring_write_single_fsync, time(ms) = 464
iouring_write_single_fsync, time(ms) = 469
iouring_write_single_fsync, time(ms) = 479
iouring_write_single_fsync, time(ms) = 491
*/

int main() {
    FILE* file = tmpfile();
    int fd = fileno(file);
    assert(fd != -1);
    constexpr int block_size = 4 << 10; // 4K
    constexpr long long file_size = 10LL << 30; // 10GB

    struct io_uring ring;
    constexpr long long QUEUE_DEPTH = 10000;
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
    io_uring_register_files(&ring, &fd, 1);

    std::vector<char> buf(block_size);
    char val = '0' + 1;
    std::fill(&buf[0], &buf[0] + block_size, val);

    const auto start = std::chrono::steady_clock::now();
    int cnt = 0;
    for (long long i = 0; i < file_size / block_size; i++) {
        auto* sqe = io_uring_get_sqe(&ring);
        sqe->flags |= IOSQE_FIXED_FILE;
        io_uring_prep_write(sqe, fd, &buf[0], block_size, -1);
        cnt++;
        if (i != 0 && i % QUEUE_DEPTH == 0) io_uring_submit(&ring);
    }
    int ret = io_uring_submit_and_wait(&ring, cnt);
    io_uring_cq_advance(&ring, ret);

    auto* sqe = io_uring_get_sqe(&ring);
    sqe->flags |= IOSQE_FIXED_FILE;
    io_uring_prep_fsync(sqe, fd, 0);
    ret = io_uring_submit_and_wait(&ring, 1);
    assert(ret == 1);
    io_uring_cq_advance(&ring , ret);
    
    const auto end = std::chrono::steady_clock::now();
    const auto duration = end - start;

    std::cout << "iouring_write_single_fsync, time(ms) = " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';

    io_uring_queue_exit(&ring);
    close(fd);
}