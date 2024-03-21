#include <arm_neon.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define N 4
#define LOOPS 1

int main() {

  int are_equal_1 = 1;
  int are_equal_2 = 1;
  int count = 0;
  struct timeval tv1, tv2, tv3, tv4, tv5, diff1, diff2, diff3, diff4;

  // Declare and allocate memory for vectorA
  float vectorA[N] __attribute__((aligned(16)));

  // Declare and allocate memory for matrix B
  float B[N][N] __attribute__((aligned(16)));

  // Declare and allocate memory for resultVector1_neon (Vector x Matrix)
  // this will store the result from NEON 1x4
  float resultVector1_neon[N] __attribute__((aligned(16)));

  // Declare and allocate memory for resultVector2_neon (Matrix x Vector)
  // this will store the result from NEON 4x1
  float resultVector2_neon[N] __attribute__((aligned(16)));

  // Declare and allocate memory for resultVector1_scalar (Vector x Matrix)
  // scalar version
  float resultVector1_scalar[N] __attribute__((aligned(16)));

  // Declare and allocate memory for resultVector2_scalar (Matrix x Vector)
  // scalar version
  float resultVector2_scalar[N] __attribute__((aligned(16)));

  // fill vector
  for (int i = 0; i < N; i++) {
    vectorA[i] = (float)i;
  }

  // fill matrix
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      B[i][j] = (float)count++;
    }
  }

  gettimeofday(&tv1, NULL);

  for (int j = 0; j < LOOPS; j++) {
    // SCALAR VERSION for Matrix x Vector
    for (int i = 0; i < N; i++) {
      resultVector1_scalar[i] = 0.0f;
      for (int j = 0; j < N; j++) {
        resultVector1_scalar[i] += B[i][j] * vectorA[j];
      }
    }
  }
  gettimeofday(&tv2, NULL);

  // NEON VERSION for Matrix x Vector
  float32x4_t result1_neon;
  float32x4_t BVec = vdupq_n_f32(0.0f);
  float32x4_t AVec = vld1q_f32(vectorA);

  for (int j = 0; j < LOOPS; j++) {
    for (int i = 0; i < N; i++) {
      result1_neon = vdupq_n_f32(0.0f);
      for (int j = 0; j < N; j += 4) {
        // Load 4 elements of B[i][j] into BVec
        BVec = vld1q_f32(&B[i][j]);
        // Multiply and accumulate
        result1_neon = vmlaq_f32(result1_neon, BVec, AVec);
      }
      // Sum the 4 elements in sumVec and store the result
      resultVector1_neon[i] = vaddvq_f32(result1_neon);
    }
  }
  gettimeofday(&tv3, NULL);

  // SCALAR VERSION for Vector x Matrix
  for (int k = 0; k < LOOPS; k++) {
    for (int i = 0; i < N; i++) {
      resultVector2_scalar[i] = 0.0f;
      for (int j = 0; j < N; j++) {
        resultVector2_scalar[i] += vectorA[j] * B[j][i];
      }
    }
  }
  gettimeofday(&tv4, NULL);

  // NEON VERSION for Vector x Matrix
  float32x4_t result2_neon = vdupq_n_f32(0.0f);

  for (int k = 0; k < LOOPS; k++) {
    result2_neon = vdupq_n_f32(0.0f);

    for (int i = 0; i < N; i++) {
      float32x4_t B_row = vld1q_f32(&B[i][0]);
      float32x4_t A_value = vdupq_n_f32(vectorA[i]);
      result2_neon = vmlaq_f32(result2_neon, B_row, A_value);
    }

    // now the result2_neon vector contains the result and I need to save it
    // into resultVector2_neon float array
    vst1q_f32(resultVector2_neon, result2_neon);
  }
  gettimeofday(&tv5, NULL);

  // compare results to check if correct
  for (int i = 0; i < 4; i++) {
    if (resultVector1_scalar[i] != resultVector1_neon[i]) {
      are_equal_1 = 0;
      break;
    }
  }
  for (int i = 0; i < 4; i++) {
    if (resultVector2_scalar[i] != resultVector2_neon[i]) {
      are_equal_2 = 0;
      break;
    }
  }
  if (are_equal_1) {
    printf("Matrix x Vector (1) correct\n");
  } else {
    printf("Matrix x Vector (1) wrong\n");
  }
  if (are_equal_2) {
    printf("Vector x Matrix (2) correct\n");
  } else {
    printf("Vector x Matrix (2) wrong\n");
  }

  diff1.tv_sec = tv2.tv_sec - tv1.tv_sec;
  diff1.tv_usec = tv2.tv_usec + (1000000 - tv1.tv_usec);
  diff2.tv_sec = tv3.tv_sec - tv2.tv_sec;
  diff2.tv_usec = tv3.tv_usec + (1000000 - tv2.tv_usec);
  diff3.tv_sec = tv4.tv_sec - tv3.tv_sec;
  diff3.tv_usec = tv4.tv_usec + (1000000 - tv3.tv_usec);
  diff4.tv_sec = tv5.tv_sec - tv4.tv_sec;
  diff4.tv_usec = tv5.tv_usec + (1000000 - tv4.tv_usec);

  printf("Scalar Matrix x Vector: %ld sec, usec: %d\n", diff1.tv_sec,
         diff1.tv_usec);
  printf("NEON Matrix x Vector: %ld sec, usec: %d\n", diff2.tv_sec,
         diff2.tv_usec);
  printf("Scalar Vector x Matrix: %ld sec, usec: %d\n", diff3.tv_sec,
         diff3.tv_usec);
  printf("NEON Vector x Matrix: %ld sec, usec: %d\n", diff4.tv_sec,
         diff4.tv_usec);
}