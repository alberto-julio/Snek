#include <omp.h>
#include <x86intrin.h>

#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int32_t dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  int32_t result = 0;
  int32_t t1;
  int32_t t2;
  for (int i=0; i < n; i++) {
      t1 = vec1[i];
      t2 = vec2[i];
      result += (t1 * t2);
  }

  return result;
}

// Computes the convolution of two matrices
int convolve(matrix_t* a_matrix, matrix_t* b_matrix, matrix_t** output_matrix) {
  if (a_matrix == NULL || b_matrix == NULL) {
      return -1;
  }
  //instantiate the rows and cols
  uint32_t b_num_cols = b_matrix->cols;
  uint32_t a_num_cols = a_matrix->cols;
  uint32_t b_num_rows = b_matrix->rows;
  uint32_t a_num_rows = a_matrix->rows;

  //instantiate output
  uint32_t output_size_cols = (a_num_cols + 1) - b_num_cols;
  uint32_t output_size_rows = (a_num_rows + 1) - b_num_rows;
  *output_matrix = (matrix_t*) malloc(sizeof(matrix_t));
  (*output_matrix)->data = (int32_t*) malloc(sizeof(int32_t) * output_size_cols * output_size_rows);
  (*output_matrix)->rows = output_size_rows;
  (*output_matrix)->cols = output_size_cols;
  if ((*output_matrix) == NULL) {
      return -1;
  }

  matrix_t flipped_matrix = {
        .rows = b_num_rows,
        .cols = b_num_cols,
        .data = (int32_t*)malloc(b_num_rows * b_num_cols * sizeof(int32_t))
    };
    if (flipped_matrix.data == NULL) {
        free((*output_matrix)->data);
        free(*output_matrix);
        return -1; // Memory allocation failed
    }
    #pragma omp parallel for collapse(2)
    for (uint32_t i = 0; i < b_num_rows; i++) {
        for (uint32_t j = 0; j < b_num_cols; j++) {
            flipped_matrix.data[i * b_num_cols + j] = b_matrix->data[(b_num_rows - i - 1) * b_num_cols + (b_num_cols - j - 1)];
        }
    }
    __m256i* avx_flipped_matrix = (__m256i*) flipped_matrix.data;

  //tried out swapping the elements another way since i wasnt sure if our current way would work
    #pragma omp parallel for collapse(2)
     for (uint32_t i = 0; i < (*output_matrix)->rows; i++) { //iterate through the rows of the output matrix 
      for (uint32_t j = 0; j < (*output_matrix)->cols; j++) { //iterate through the columns of the output matrix
          __m256i result = _mm256_setzero_si256();
          #pragma omp parallel for collapse(2)
          for (uint32_t k = 0; k < b_num_rows; k+=1) { //iterates through the number of rows in b
                for (uint32_t m = 0; m < b_num_cols / 8 * 8; m+=8) { //iterate through the number of cols in b
                  // int32_t treflip = i + k; //inner most loop is essentially doing the dot product for me
                  // int32_t switchflip = j + m;

                  //__m256i a_values = _mm256_loadu_si256((__m256i*) (&a_matrix->data[(i+k) * a_num_cols + (j+m)]));
                  // __m256i b_values = _mm256_loadu_si256((__m256i*) (&flipped_matrix.data[k * b_num_cols + m]));
                  __m256i product = _mm256_mullo_epi32(_mm256_loadu_si256((__m256i*) (&a_matrix->data[(i+k) * a_num_cols + (j+m)])), _mm256_loadu_si256((__m256i*) (&flipped_matrix.data[k * b_num_cols + m])));
                  result = _mm256_add_epi32(result, product);
                }             
          }
          int32_t output[8];
          _mm256_storeu_si256((__m256i*)output, result);
          for(uint32_t w = 0; w < b_num_rows; w++) {
              for (uint32_t x = b_num_cols / 8 * 8; x < b_num_cols; x++) {
                  // int32_t treflip = i + w; 
                  // int32_t switchflip = j + x;
                  output[0] += a_matrix->data[(i + w) * a_num_cols + (j+x)] * flipped_matrix.data[w * b_num_cols + x];
             }
          }
          // Store the result in the output_matrix
          (*output_matrix)->data[i * output_size_cols + j] = output[0] + output[1] + output[2] + output[3] + output[4] + output[5] + output[6] + output[7];
      }
  }
  if (output_matrix == NULL) {
      return -1;
  }
  return 0;
}

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  if (read_matrix(get_a_matrix_path(task), &a_matrix))
    return -1;
  if (read_matrix(get_b_matrix_path(task), &b_matrix))
    return -1;

  if (convolve(a_matrix, b_matrix, &output_matrix))
    return -1;

  if (write_matrix(get_output_matrix_path(task), output_matrix))
    return -1;

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}
