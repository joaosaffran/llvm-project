// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 5
// RUN: %clang_cc1 %s -emit-llvm -o - -triple=aarch64-none-elf | FileCheck %s
// REQUIRES: aarch64-registered-target

#include <stdatomic.h>

// CHECK-LABEL: define dso_local void @clear_relaxed(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] monotonic, align 1
// CHECK-NEXT:    ret void
//
void clear_relaxed(char *ptr) {
  __atomic_clear(ptr, memory_order_relaxed);
}

// CHECK-LABEL: define dso_local void @clear_seq_cst(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] seq_cst, align 1
// CHECK-NEXT:    ret void
//
void clear_seq_cst(char *ptr) {
  __atomic_clear(ptr, memory_order_seq_cst);
}

// CHECK-LABEL: define dso_local void @clear_release(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] release, align 1
// CHECK-NEXT:    ret void
//
void clear_release(char *ptr) {
  __atomic_clear(ptr, memory_order_release);
}

// CHECK-LABEL: define dso_local void @clear_dynamic(
// CHECK-SAME: ptr noundef [[PTR:%.*]], i32 noundef [[ORDER:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ORDER_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store i32 [[ORDER]], ptr [[ORDER_ADDR]], align 4
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[ORDER_ADDR]], align 4
// CHECK-NEXT:    switch i32 [[TMP1]], label %[[MONOTONIC:.*]] [
// CHECK-NEXT:      i32 3, label %[[RELEASE:.*]]
// CHECK-NEXT:      i32 5, label %[[SEQCST:.*]]
// CHECK-NEXT:    ]
// CHECK:       [[MONOTONIC]]:
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] monotonic, align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE:.*]]
// CHECK:       [[RELEASE]]:
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] release, align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE]]
// CHECK:       [[SEQCST]]:
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] seq_cst, align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE]]
// CHECK:       [[ATOMIC_CONTINUE]]:
// CHECK-NEXT:    ret void
//
void clear_dynamic(char *ptr, int order) {
  __atomic_clear(ptr, order);
}

// CHECK-LABEL: define dso_local void @test_and_set_relaxed(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 monotonic, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_relaxed(char *ptr) {
  __atomic_test_and_set(ptr, memory_order_relaxed);
}

// CHECK-LABEL: define dso_local void @test_and_set_consume(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 acquire, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_consume(char *ptr) {
  __atomic_test_and_set(ptr, memory_order_consume);
}

// CHECK-LABEL: define dso_local void @test_and_set_acquire(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 acquire, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_acquire(char *ptr) {
  __atomic_test_and_set(ptr, memory_order_acquire);
}

// CHECK-LABEL: define dso_local void @test_and_set_release(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 release, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_release(char *ptr) {
  __atomic_test_and_set(ptr, memory_order_release);
}

// CHECK-LABEL: define dso_local void @test_and_set_acq_rel(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 acq_rel, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_acq_rel(char *ptr) {
  __atomic_test_and_set(ptr, memory_order_acq_rel);
}

// CHECK-LABEL: define dso_local void @test_and_set_seq_cst(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 seq_cst, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_seq_cst(char *ptr) {
  __atomic_test_and_set(ptr, memory_order_seq_cst);
}

// CHECK-LABEL: define dso_local void @test_and_set_dynamic(
// CHECK-SAME: ptr noundef [[PTR:%.*]], i32 noundef [[ORDER:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ORDER_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store i32 [[ORDER]], ptr [[ORDER_ADDR]], align 4
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[ORDER_ADDR]], align 4
// CHECK-NEXT:    switch i32 [[TMP1]], label %[[MONOTONIC:.*]] [
// CHECK-NEXT:      i32 1, label %[[ACQUIRE:.*]]
// CHECK-NEXT:      i32 2, label %[[ACQUIRE]]
// CHECK-NEXT:      i32 3, label %[[RELEASE:.*]]
// CHECK-NEXT:      i32 4, label %[[ACQREL:.*]]
// CHECK-NEXT:      i32 5, label %[[SEQCST:.*]]
// CHECK-NEXT:    ]
// CHECK:       [[MONOTONIC]]:
// CHECK-NEXT:    [[TMP2:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 monotonic, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP2]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE:.*]]
// CHECK:       [[ACQUIRE]]:
// CHECK-NEXT:    [[TMP3:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 acquire, align 1
// CHECK-NEXT:    [[TOBOOL1:%.*]] = icmp ne i8 [[TMP3]], 0
// CHECK-NEXT:    store i1 [[TOBOOL1]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE]]
// CHECK:       [[RELEASE]]:
// CHECK-NEXT:    [[TMP4:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 release, align 1
// CHECK-NEXT:    [[TOBOOL2:%.*]] = icmp ne i8 [[TMP4]], 0
// CHECK-NEXT:    store i1 [[TOBOOL2]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE]]
// CHECK:       [[ACQREL]]:
// CHECK-NEXT:    [[TMP5:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 acq_rel, align 1
// CHECK-NEXT:    [[TOBOOL3:%.*]] = icmp ne i8 [[TMP5]], 0
// CHECK-NEXT:    store i1 [[TOBOOL3]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE]]
// CHECK:       [[SEQCST]]:
// CHECK-NEXT:    [[TMP6:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 seq_cst, align 1
// CHECK-NEXT:    [[TOBOOL4:%.*]] = icmp ne i8 [[TMP6]], 0
// CHECK-NEXT:    store i1 [[TOBOOL4]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    br label %[[ATOMIC_CONTINUE]]
// CHECK:       [[ATOMIC_CONTINUE]]:
// CHECK-NEXT:    [[TMP7:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP7]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_dynamic(char *ptr, int order) {
  __atomic_test_and_set(ptr, order);
}

// CHECK-LABEL: define dso_local void @test_and_set_array(
// CHECK-SAME: ) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[X:%.*]] = alloca [10 x i32], align 4
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    [[ARRAYDECAY:%.*]] = getelementptr inbounds [10 x i32], ptr [[X]], i64 0, i64 0
// CHECK-NEXT:    [[TMP0:%.*]] = atomicrmw volatile xchg ptr [[ARRAYDECAY]], i8 1 seq_cst, align 4
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP0]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP1:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP1]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_array() {
  volatile int x[10];
  __atomic_test_and_set(x, memory_order_seq_cst);
}

// These intrinsics accept any pointer type, including void and incomplete
// structs, and always access the first byte regardless of the actual type
// size.

struct incomplete;

// CHECK-LABEL: define dso_local void @clear_int(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] monotonic, align 4
// CHECK-NEXT:    ret void
//
void clear_int(int *ptr) {
  __atomic_clear(ptr, memory_order_relaxed);
}
// CHECK-LABEL: define dso_local void @clear_void(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] monotonic, align 1
// CHECK-NEXT:    ret void
//
void clear_void(void *ptr) {
  __atomic_clear(ptr, memory_order_relaxed);
}
// CHECK-LABEL: define dso_local void @clear_incomplete(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    store atomic i8 0, ptr [[TMP0]] monotonic, align 1
// CHECK-NEXT:    ret void
//
void clear_incomplete(struct incomplete *ptr) {
  __atomic_clear(ptr, memory_order_relaxed);
}

// CHECK-LABEL: define dso_local void @test_and_set_int(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 monotonic, align 4
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_int(int *ptr) {
  __atomic_test_and_set(ptr, memory_order_relaxed);
}
// CHECK-LABEL: define dso_local void @test_and_set_void(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 monotonic, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_void(void *ptr) {
  __atomic_test_and_set(ptr, memory_order_relaxed);
}
// CHECK-LABEL: define dso_local void @test_and_set_incomplete(
// CHECK-SAME: ptr noundef [[PTR:%.*]]) #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[PTR_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[ATOMIC_TEMP:%.*]] = alloca i8, align 1
// CHECK-NEXT:    store ptr [[PTR]], ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[PTR_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = atomicrmw xchg ptr [[TMP0]], i8 1 monotonic, align 1
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i8 [[TMP1]], 0
// CHECK-NEXT:    store i1 [[TOBOOL]], ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[TMP2:%.*]] = load i8, ptr [[ATOMIC_TEMP]], align 1
// CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[TMP2]] to i1
// CHECK-NEXT:    ret void
//
void test_and_set_incomplete(struct incomplete *ptr) {
  __atomic_test_and_set(ptr, memory_order_relaxed);
}
