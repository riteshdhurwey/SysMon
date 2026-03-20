# SysMon 🖥️

A lightweight, real-time Linux system monitor written in **modern C++17** — reads directly from the `/proc` filesystem and `statvfs`, zero external dependencies except ncurses.

![Platform](https://img.shields.io/badge/platform-Linux-blue)
![C++17](https://img.shields.io/badge/C++-17-green)
![TUI](https://img.shields.io/badge/TUI-ncurses-cyan)
![CI](https://github.com/riteshdhurwey/SysMon/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/license-MIT-orange)

---

[![SysMon Demo](https://asciinema.org/a/UJdGYqylhvy7JUit.svg)](https://asciinema.org/a/UJdGYqylhvy7JUit)

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
- Full process list — PID, name, CPU%, state, memory (RSS)
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
│   ├── ResourceMonitor.h    # RAM & Disk combined
│   ├── process.h            # ProcessMonitor class
│   ├── systemMonitor.h      # Top-level monitor
│   ├── sysinfo.h
│   └── ui.h                 # ncurses TUI class
├── src/
│   ├── main.cpp
│   ├── cpu.cpp
│   ├── ResourceMonitor.cpp
│   ├── process.cpp
│   ├── systemMonitor.cpp
│   ├── sysinfo.cpp
│   └── ui.cpp
└── tests/
    ├── test_cpu.cpp          # CPUMonitor unit + integration tests
    ├── test_resource.cpp     # Memory & Disk tests
    └── test_process.cpp      # ProcessMonitor + sort correctness tests
```

---

## Build & Run

### Requirements
- Linux (kernel 2.6+)
- g++ with C++17 support
- ncurses (`libncurses-dev`)

### Install ncurses

```bash
# Debian/Ubuntu
sudo apt install libncurses-dev

# Arch
sudo pacman -S ncurses
```

### Build & Run

```bash
git clone https://github.com/riteshdhurwey/SysMon.git
cd SysMon
make
./sysmon
```

### Run Tests

```bash
# CPU tests
g++ tests/test_cpu.cpp src/cpu.cpp -Iinclude -o test_cpu && ./test_cpu

# Memory & Disk tests
g++ tests/test_resource.cpp src/ResourceMonitor.cpp -Iinclude -o test_resource && ./test_resource

# Process tests
g++ tests/test_process.cpp src/process.cpp src/CPUUtils.cpp -Iinclude -o test_process && ./test_process
```

---

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
- [x] Unit + integration test suite
- [ ] Per-core CPU breakdown
- [ ] User column in process list
- [ ] `--interval` and `--top N` CLI flags
- [ ] Metric logging to CSV

---

## Author

**Ritesh Dhurwey**
[LinkedIn](https://linkedin.com/in/ritesh-dhurwey-100383216) · [GitHub](https://github.com/riteshdhurwey)
