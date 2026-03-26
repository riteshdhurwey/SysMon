# SysMon 🖥️

A lightweight, real-time Linux system monitor written in **modern C++17** — reads directly from the `/proc` filesystem and `statvfs`, zero external dependencies except ncurses.

![Platform](https://img.shields.io/badge/platform-Linux-blue)
![C++17](https://img.shields.io/badge/C++-17-green)
![TUI](https://img.shields.io/badge/TUI-ncurses-cyan)
![CI](https://github.com/riteshdhurwey/SysMon/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/license-MIT-orange)

---
![demo](https://github.com/user-attachments/assets/5478a0c6-1843-4cf4-b846-2fd02d8b3707)

---
> ⭐ If you found this useful, consider starring the repo
---

## Overview

SysMon is a `htop`-inspired terminal dashboard built from scratch in C++17. It bypasses shell utilities entirely — no `top`, no `ps`, no `df` — and reads kernel data directly, the same way production monitoring tools do.

Built as a deep-dive into Linux systems programming: `/proc` parsing, delta sampling for CPU%, `statvfs` for disk, ncurses windowed layout, and stable PID-tracked process selection across live re-sorts.

---

## Features

### System Metrics
- **CPU** — real-time total%, user, system, idle, iowait, IRQ, softIRQ via `/proc/stat`
- **Memory** — used/total with usage bar via `/proc/meminfo` (`MemAvailable`, not `MemFree`)
- **Disk** — used/total GB via `statvfs()` matching `df -h` exactly (`f_bavail` vs `f_bfree`)
- **Uptime** — live system uptime from `/proc/uptime`
- **Network I/O** — RX/TX bytes per interface from `/proc/net/dev`
- Refreshes every **1 second**

### Process Management
- Full process list — PID, name, CPU%, state, memory (RSS) ,Process runtime ,total/zombie count to process list
- Scrollable with ↑↓ arrow keys
- Sort by CPU%, Memory, or PID (toggle ascending/descending)
- Kill process — press `K`, confirm with `Y/N`
- **PID-stable selection** — highlight follows the correct process across re-sorts and live updates
- Kernel thread filtering — hides zero-memory kernel threads

### TUI (ncurses)
- Window-based layout — separate window per stat section
- Color-coded bars — green / yellow / red based on usage thresholds
- Pause-on-scroll — list freezes while navigating, resumes after 1s idle
- Help overlay — press `H` for keyboard shortcuts
- Terminal resize support — windows rebuild on `SIGWINCH`

---

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| ↑ / ↓ | Scroll process list |
| C | Sort by CPU% |
| M | Sort by Memory |
| P | Sort by PID |
| K | Kill selected process |
| H | Toggle help overlay |
| Q | Quit |
| Alt+C | Toggle CPU detail view |

## Launch Flags

| Flag | Action |
|-----|--------|
| `--cpu` | Start in CPU detail view |

---

## How It Works

### CPU Usage — Delta Sampling
`/proc/stat` exposes cumulative CPU ticks. A single snapshot is meaningless. SysMon takes two snapshots across a time delta and computes:

```
cpu_usage% = (1 - idle_delta / total_delta) * 100
```

`idle_delta` includes both `idle` and `iowait` — the same method used by `htop`.

### Memory
```
used = MemTotal - MemAvailable
```
Uses `MemAvailable` (not `MemFree`) — accounts for reclaimable page cache, giving an accurate picture of truly available memory.

### Disk
```
used  = (f_blocks - f_bfree) * f_frsize
free  = f_bavail * f_frsize
total = used + free
```
Linux reserves ~5% of disk blocks for root. Using `f_bavail` (available to non-root) instead of `f_bfree` matches `df -h` output exactly.

### Per-Process CPU%
Two reads of `/proc/[pid]/stat` across a 500ms interval:
```
process_cpu% = (proc_delta / total_cpu_delta) * 100
```

### Stable Process Selection
Process list re-sorts every second. SysMon tracks the **selected PID**, not the list index — re-syncing the cursor position after every update so the highlight follows the correct process even as sort order changes.

---

## Project Structure

```
sysmon/
├── include/
│   ├── CPUUtils.h           # /proc/stat reader (shared)
│   ├── cpu.h                # CPUMonitor class
│   ├── cpu_detail.h         # CPUDetail class (per-core, freq, temp, scheduler)
│   ├── ResourceMonitor.h    # RAM & Disk combined
│   ├── network_monitor.h    # Network stats
│   ├── process.h            # ProcessMonitor class
│   ├── systemMonitor.h      # Top-level monitor
│   ├── sysinfo.h            # Time, uptime utilities
│   └── ui.h                 # ncurses TUI class
├── src/
│   ├── main.cpp
│   ├── cpu.cpp
│   ├── cpu_detail.cpp       # Per-core usage, freq, temp, cache, scheduler
│   ├── ResourceMonitor.cpp
│   ├── network_monitor.cpp
│   ├── process.cpp
│   ├── systemMonitor.cpp
│   ├── sysinfo.cpp
│   └── ui.cpp
└── tests/
    ├── test_cpu.cpp          # CPUMonitor unit + integration tests
    ├── test_cpu_detail.cpp   # Per-core, freq, temp, scheduler tests
    ├── test_resource.cpp     # Memory & Disk tests
    └── test_process.cpp      # ProcessMonitor + sort correctness tests
```

---
# Installation Guide

## Prerequisites

Make sure you have the following installed:

### Ubuntu/Debian
```bash
sudo apt install build-essential cmake libncurses-dev
```

### Arch Linux
```bash
sudo pacman -S base-devel cmake ncurses
```

### Fedora
```bash
sudo dnf install gcc-c++ cmake ncurses-devel
```

---

## Install
```bash
# Clone the repository
git clone https://github.com/riteshdhurwey/SysMon.git

# Build
mkdir build
cd build
cmake ..
make

# Install
sudo make install sysmon
```

---

## Uninstall
```bash
cd build
sudo make uninstall 
```

---

## Usage
```bash
sysmon
```

---

## Running Tests

### Install Google Test

#### Ubuntu/Debian
```bash
sudo apt install libgtest-dev
```

#### Arch Linux
```bash
sudo pacman -S gtest
```

#### Fedora
```bash
sudo dnf install gtest-devel
```

### Run Tests
```bash
cd build
make test

./test_all
```

> **Note:** `sudo` is required for install/uninstall as it copies binary to `/usr/local/bin`

## /proc Files Used

| File | Purpose |
|------|---------|
| `/proc/stat` | System-wide CPU tick counters |
| `/proc/meminfo` | Memory totals and availability |
| `/proc/uptime` | System uptime in seconds |
| `/proc/[pid]/stat` | Per-process CPU time (utime + stime) |
| `/proc/[pid]/statm` | Per-process resident memory |
| `/proc/[pid]/comm` | Process name |
| `/proc/net/dev` | Network RX/TX per interface |
| `statvfs()` | Disk block counts and sizes |

---

## Roadmap

- [x] Real-time CPU, Memory, Disk, Network monitoring
- [x] ncurses TUI with windowed layout
- [x] Scrollable, sortable process list
- [x] Kill process feature
- [x] System uptime display
- [x] GTest suite (7 passing tests)
- [x] GitHub Actions CI

### CPU
- [x] `--cpu` flag — detailed CPU stats (per-core usage, frequency)
- [ ] Per-core CPU breakdown in TUI
- [ ] CPU temperature via `/sys/class/thermal`

### Memory
- [ ] `--mem` flag — detailed memory breakdown (cached, buffers, swap usage)
- [ ] Swap usage monitoring

### Disk
- [ ] `--disk` flag — per-partition breakdown
- [ ] Disk read/write speed via `/proc/diskstats`
- [ ] I/O wait per process

### Network
- [ ] `--net` flag — per-interface detailed stats (packets, errors, drops)
- [ ] Network speed graph over time

### Process
- [ ] `--process` flag — detailed info for a specific PID
- [ ] User column in process list
- [ ] Filter processes by name or user

### General
- [ ] `--interval N` flag — custom refresh rate in seconds
- [ ] `--top N` flag — show only top N processes
- [ ] Metric logging to CSV
- [ ] Bug fixes and performance improvements



## Contributing

Contributions are welcome! Check out the [Contributing Guide](CONTRIBUTING.md) for how to get started and a list of features you can work on.

## Author

**Ritesh Dhurwey**
[LinkedIn](https://linkedin.com/in/ritesh-dhurwey-100383216) · [GitHub](https://github.com/riteshdhurwey)
