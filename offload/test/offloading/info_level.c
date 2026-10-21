// Verify that LIBOMPTARGET_DEBUG type/level filters control the user-facing
// LIBOMPTARGET_INFO messages (the consolidated info/debug gate), and that the
// detail level selects between the summary and detailed lines.
//
// RUN: %libomptarget-compile-generic -gline-tables-only
// RUN: env LIBOMPTARGET_DEBUG=DataTransfer %libomptarget-run-generic 2>&1 | \
// RUN:   %fcheck-generic -check-prefix=DT
// RUN: env LIBOMPTARGET_DEBUG=PluginKernel %libomptarget-run-generic 2>&1 | \
// RUN:   %fcheck-generic -check-prefix=PK
// RUN: env LIBOMPTARGET_DEBUG=KernelArgs %libomptarget-run-generic 2>&1 | \
// RUN:   %fcheck-generic -check-prefix=KA
// RUN: env LIBOMPTARGET_DEBUG=KernelArgs:2 %libomptarget-run-generic 2>&1 | \
// RUN:   %fcheck-generic -check-prefix=KA2
// RUN: env LIBOMPTARGET_INFO=-1 %libomptarget-run-generic 2>&1 | \
// RUN:   %fcheck-generic -check-prefix=FULL

#include <stdio.h>

#define N 64

int main() {
  int A[N];
  int s = 3;

  // DataTransfer only: data-copy messages, but no kernel-args header or launch.
  // DT: info: Copying data from host to device
  // DT-NOT: info: Entering OpenMP kernel
  // DT-NOT: info: Launching kernel

  // PluginKernel only: the launch message (level 1), no data-copy messages.
  // PK: info: Launching kernel
  // PK-NOT: info: Copying data

  // KernelArgs at the default level: the region header (level 1) but not the
  // per-argument detail lines (level 2).
  // KA: info: {{.*}}OpenMP kernel{{.*}}with {{[0-9]+}} arguments:
  // KA-NOT: info: firstprivate(

  // KernelArgs at level 2: the region header and the per-argument detail lines.
  // KA2: info: {{.*}}OpenMP kernel{{.*}}with {{[0-9]+}} arguments:
  // KA2: info: firstprivate(

  // The legacy numeric bitmask still enables every category.
  // FULL: info: Entering OpenMP kernel
  // FULL: info: Copying data from host to device
  // FULL: info: Launching kernel
#pragma omp target map(tofrom : A) firstprivate(s)
  { A[0] = s; }

  printf("A[0] = %d\n", A[0]);
  // DT: A[0] = 3
  // PK: A[0] = 3
  // KA: A[0] = 3
  // KA2: A[0] = 3
  // FULL: A[0] = 3
  return 0;
}
