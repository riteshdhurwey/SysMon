# SysMon 🖥️

A lightweight, real-time Linux system monitor built in **modern C++17** with zero external runtime dependencies (except ncurses).

> Built for **accurate metrics, stable process behavior, and clean terminal UX** .

![Platform](https://img.shields.io/badge/platform-Linux-blue)
![C++17](https://img.shields.io/badge/C++-17-green)
![TUI](https://img.shields.io/badge/TUI-ncurses-cyan)
![CI](https://github.com/riteshdhurwey/SysMon/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/license-MIT-orange)

---

![demo](https://github.com/user-attachments/assets/5478a0c6-1843-4cf4-b846-2fd02d8b3707)

**What to notice in the demo:**

* Selection **sticks to the same PID** even while sorting changes
* CPU usage updates smoothly using **delta sampling** (no spikes/jumps)
* Clean window layout with **stable rendering (minimal flicker)**

> ⭐ If you find this useful, consider starring the repo

---

## Why SysMon?

* **Stable process tracking** — selection follows the same PID even during live sorting and updates
* **Accurate CPU measurement** — delta-based calculation avoids misleading spikes
* **Correct disk usage** — matches real system availability (`df -h` behavior)
* **Clean, predictable layout** — structured ncurses UI for readability
* **Direct kernel data access** — reads from `/proc` and `statvfs`, no shell utilities

**Summary:** SysMon is built for **correctness, stability, and clarity**, not just displaying stats.

## Overview

SysMon is a terminal-based system monitor built from scratch in C++17. It reads directly from Linux kernel interfaces instead of relying on external tools like `top`, `ps`, or `df`.

Focus areas:

* `/proc` parsing
* CPU delta sampling
* Memory correctness using `MemAvailable`
* Disk accounting using `statvfs`
* Real-time UI rendering with ncurses

---

## Core Features

### System Metrics

* CPU usage (user, system, idle, iowait, IRQ)
* Memory usage using accurate availability model
* Disk usage aligned with `df -h`
* System uptime
* Network I/O per interface

### Process Management

* Full process list (PID, CPU%, memory, state)
* Sorting (CPU / Memory / PID)
* Scrollable navigation
* Kill process support (`K`)
* Kernel thread filtering
* **PID-stable selection across re-sorts**

### TUI

* Window-based layout
* Color-coded usage bars
* Pause-on-scroll behavior
* Help overlay (`H`)
* Terminal resize handling

---

## Performance

* Refresh interval: 1s
* CPU usage: ~3–5% (idle system)
* Memory usage: ~5–6 MB
* Consistent real-time updates with low overhead
- Direct kernel reads (`/proc`, `statvfs`), no subprocess calls

---

## How It Works (Key Concepts)

### CPU Usage

```
cpu_usage% = (1 - idle_delta / total_delta) * 100
```

### Memory

```
used = MemTotal - MemAvailable
```

### Disk

```
used  = (f_blocks - f_bfree) * f_frsize
free  = f_bavail * f_frsize
```

### Process CPU%

```
process_cpu% = (proc_delta / total_cpu_delta) * 100
```

---

## Installation

### Requirements

* C++17 compiler
* CMake
* ncurses

### Build

```bash
git clone https://github.com/riteshdhurwey/SysMon.git
cd SysMon
mkdir build && cd build
cmake ..
make
```

### Install

```bash
sudo make install
```

### Run

```bash
sysmon
```

---

## Project Structure

```
sysmon/
├── include/
├── src/
└── tests/
```

Modular design with separate components for CPU, memory, process, network, and UI.

---

## Roadmap

* Detailed memory view
* Disk I/O stats
* Network graphs
* Process filtering
* Custom refresh intervals
* Metrics export

---

## Contributing

Contributions are welcome. See `CONTRIBUTING.md`.

---

## Author

**Ritesh Dhurwey**
[LinkedIn](https://linkedin.com/in/ritesh-dhurwey-100383216) · [GitHub](https://github.com/riteshdhurwey)
