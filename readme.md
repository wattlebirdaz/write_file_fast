# 5M write

5M を 4K ずつ書き込み**逐一** fsync するのにかかった時間を計測

```sh
g++ name.cpp
for i in {1..5}; do ./a.out; done;
```

`Intel(R) Core(TM) i9-9900 CPU @ 3.10GHz`

## buffer write

- buffer_write, time(ms) = 4448
- buffer_write, time(ms) = 4493
- buffer_write, time(ms) = 4436
- buffer_write, time(ms) = 4468
- buffer_write, time(ms) = 4410

## overwrite

- overwrite, time(ms) = 2562
- overwrite, time(ms) = 2647
- overwrite, time(ms) = 2614
- overwrite, time(ms) = 2703
- overwrite, time(ms) = 2665

## fallocate + buffer write

- fallocate_buffer_write, time(ms) = 4252
- fallocate_buffer_write, time(ms) = 4312
- fallocate_buffer_write, time(ms) = 4333
- fallocate_buffer_write, time(ms) = 4246
- fallocate_buffer_write, time(ms) = 4230

## fallocate + filling zero + buffer write

- fallocate_fillingzero_buffer_write, time(ms) = 4282
- fallocate_fillingzero_buffer_write, time(ms) = 4294
- fallocate_fillingzero_buffer_write, time(ms) = 4261
- fallocate_fillingzero_buffer_write, time(ms) = 4269
- fallocate_fillingzero_buffer_write, time(ms) = 4286

## fallocate + flkeepsize + buffer write

- fallocate_flkeepsize_buffer_write, time(ms) = 4321
- fallocate_flkeepsize_buffer_write, time(ms) = 4295
- fallocate_flkeepsize_buffer_write, time(ms) = 4318
- fallocate_flkeepsize_buffer_write, time(ms) = 4323
- fallocate_flkeepsize_buffer_write, time(ms) = 4343

## buffer write big

5M を 5M 一気に書き込み、**一度** fsync するのにかかった時間を計測

- buffer_write_big, time(ms) = 13
- buffer_write_big, time(ms) = 7
- buffer_write_big, time(ms) = 6
- buffer_write_big, time(ms) = 6
- buffer_write_big, time(ms) = 6

## buffer write single fsync

5M を 4K ずつ書き込み、**一度** fsync するのにかかった時間を計測

- buffer_write_single_fsync, time(ms) = 15
- buffer_write_single_fsync, time(ms) = 7
- buffer_write_single_fsync, time(ms) = 7
- buffer_write_single_fsync, time(ms) = 7
- buffer_write_single_fsync, time(ms) = 7

10G を 4K ずつ書き込み、**一度** fsync するのにかかった時間を計測

- buffer_write_single_fsync, time(ms) = 10028
- buffer_write_single_fsync, time(ms) = 12022
- buffer_write_single_fsync, time(ms) = 12224
- buffer_write_single_fsync, time(ms) = 12509
- buffer_write_single_fsync, time(ms) = 12368

# 考察

- chunk にわけて書き込む場合 overwrite が一番早い
- fallocate を使うと通常の buffer write より**少し**早くなる
- fallocate の filling zero option がどれくらい効いているのかは不明
- fsync の数は少なければ少ないほどはやい
- write の数は大きな影響はなさそう
