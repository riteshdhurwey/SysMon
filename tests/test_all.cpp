#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "cpu.h"
#include "ResourceMonitor.h"
#include "process.h"

// ── CPU ──────────────────────────────────────────────────────────────
TEST(CPUTest, UsageInValidRange) {
    CPUMonitor monitor;
    monitor.update();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    monitor.update();

    const CPUUsage& usage = monitor.getUsage();
    EXPECT_GE(usage.total, 0.0);
    EXPECT_LE(usage.total, 100.0);
}

TEST(CPUTest, IdleAndUserAreNonNegative) {
    CPUMonitor monitor;
    monitor.update();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    monitor.update();

    const CPUUsage& usage = monitor.getUsage();
    EXPECT_GE(usage.idle, 0.0);
    EXPECT_GE(usage.user, 0.0);
}

// ── Memory ───────────────────────────────────────────────────────────
TEST(MemoryTest, TotalIsPositive) {
    ResourceMonitor monitor;
    monitor.update();
    EXPECT_GT(monitor.getMemory().totalKB, 0);
}

TEST(MemoryTest, UsedDoesNotExceedTotal) {
    ResourceMonitor monitor;
    monitor.update();
    const MemoryUsage& mem = monitor.getMemory();
    EXPECT_GE(mem.usedKB, 0);
    EXPECT_LE(mem.usedKB, mem.totalKB);
}

// ── Disk ─────────────────────────────────────────────────────────────
TEST(DiskTest, UsagePercentInValidRange) {
    ResourceMonitor monitor;
    monitor.update();
    const DiskUsage& disk = monitor.getDisks();
    EXPECT_GE(disk.percent, 0.0);
    EXPECT_LE(disk.percent, 100.0);
}

// ── Process ──────────────────────────────────────────────────────────
TEST(ProcessTest, ListIsNotEmpty) {
    ProcessMonitor monitor;
    monitor.update();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    monitor.update();
    EXPECT_FALSE(monitor.getProcesses().empty());
}

TEST(ProcessTest, SortByPIDIsOrdered) {
    ProcessMonitor monitor;
    monitor.update();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    monitor.update();

    monitor.sortBy(SortBy::PID);
    const auto& procs = monitor.getProcesses();
    for (size_t i = 1; i < procs.size(); i++) {
        // descending — each PID should be >= next
        EXPECT_GE(procs[i-1].pid, procs[i].pid);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}