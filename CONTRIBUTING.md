# Contributing to SysMon

Thanks for your interest in contributing!

## How to contribute

1. Fork the repo
2. Create a new branch
```
   git checkout -b feature/your-feature-name
```
3. Make your changes
4. Make sure tests still pass
```
   cd build && make && ./test_all
```
5. Commit and push
```
   git commit -m "add: your change description"
   git push origin feature/your-feature-name
```
6. Open a Pull Request on GitHub

## What you can work on

### CPU
- `--cpu` flag — detailed CPU stats (per-core usage, frequency)
- Per-core CPU breakdown in TUI
- CPU temperature via `/sys/class/thermal`

### Memory
- `--mem` flag — detailed memory breakdown (cached, buffers, swap usage)
- Swap usage monitoring

### Disk
- `--disk` flag — per-partition breakdown
- Disk read/write speed via `/proc/diskstats`
- I/O wait per process

### Network
- `--net` flag — per-interface detailed stats (packets, errors, drops)
- Network speed graph over time

### Process
- `--process` flag — detailed info for a specific PID
- User column in process list
- Filter processes by name or user

### General
- `--interval N` flag — custom refresh rate in seconds
- `--top N` flag — show only top N processes
- Metric logging to CSV
- Bug fixes and performance improvements

## Code style

- C++17
- Keep it simple and readable
- No external dependencies (except ncurses)
- Add tests for new features in `tests/test_all.cpp`

## Questions?

Open an issue on GitHub.
