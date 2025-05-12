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
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix
  // checks to see if the matrices passed in are null
  if (a_matrix == NULL || b_matrix == NULL) {
      return -1;
  }
  //instantiate the rows and cols
  uint32_t b_num_cols = b_matrix->cols;
  uint32_t a_num_cols = a_matrix->cols;
  uint32_t b_num_rows = b_matrix->rows;
  uint32_t a_num_rows = a_matrix->rows;
  uint32_t end = b_num_cols * b_num_rows;

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

    for (uint32_t i = 0; i < b_num_rows; i++) {
        for (uint32_t j = 0; j < b_num_cols; j++) {
            flipped_matrix.data[i * b_num_cols + j] = b_matrix->data[(b_num_rows - i - 1) * b_num_cols + (b_num_cols - j - 1)];
        }
    }

  //tried out swapping the elements another way since i wasnt sure if our current way would work
     for (uint32_t i = 0; i < (*output_matrix)->rows; i++) { //iterate through the rows of the output matrix 
      for (uint32_t j = 0; j < (*output_matrix)->cols; j++) { //iterate through the columns of the output matrix
          int32_t result = 0; //running total
          for (uint32_t k = 0; k < b_num_rows; k++) { //iterates through the number of rows in b
              for (uint32_t m = 0 ; m < b_num_cols; m++) { //iterate through the number of cols in b
                  int32_t treflip = i + k; //inner most loop is essentially doing the dot product for me
                  int32_t switchflip = j + m;

                  result += a_matrix->data[treflip * a_num_cols + switchflip] * flipped_matrix.data[k * b_num_cols + m];
              }
          }
          (*output_matrix)->data[i * (*output_matrix)->cols + j] = result;
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
