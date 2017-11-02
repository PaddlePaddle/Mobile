#include <stdio.h>
#include <stdlib.h>
#include <paddle/capi.h>
#include <vector>

static const char* paddle_error_string(paddle_error status) {
  switch (status) {
    case kPD_NULLPTR:
      return "nullptr error";
    case kPD_OUT_OF_RANGE:
      return "out of range error";
    case kPD_PROTOBUF_ERROR:
      return "protobuf error";
    case kPD_NOT_SUPPORTED:
      return "not supported error";
    case kPD_UNDEFINED_ERROR:
      return "undefined error";
  };
}

#define CHECK(stmt)                                             \
  do {                                                          \
    paddle_error __err__ = stmt;                                \
    if (__err__ != kPD_NO_ERROR) {                              \
      const char* str = paddle_error_string(__err__);           \
      fprintf(stderr, "%s (%d) in " #stmt "\n", str, __err__);  \
      exit(__err__);                                            \
    }                                                           \
  } while (0)

void* read_config(const char* filename, long* size) {
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
      fprintf(stderr, "Open %s error\n", filename);
      return NULL;
  }
  fseek(file, 0L, SEEK_END);
  *size = ftell(file);
  fseek(file, 0L, SEEK_SET);
  void* buf = malloc(*size);
  fread(buf, 1, *size, file);
  fclose(file);
  return buf;
}

int main() {
  // Initalize Paddle
  char* argv[] = {"--use_gpu=False"};
  CHECK(paddle_init(1, (char**)argv));

  const char* merged_model = "models/vgg_ssd_net.paddle";

  // Step 1: Reading merged model.
  long size;
  void* buf = read_config(merged_model, &size);

  // Step 2: 
  //    Create a gradient machine for inference.
  paddle_gradient_machine machine;
  CHECK(paddle_gradient_machine_create_for_inference_with_parameters(&machine, buf, size));

  // Step 3: Prepare input Arguments
  paddle_arguments in_args = paddle_arguments_create_none();

  // There is only one input of this network.
  CHECK(paddle_arguments_resize(in_args, 1));

  // Create input matrix.
  // Set the value
  size_t IMAGE_HEIGHT = 300;
  size_t IMAGE_WIDTH = 300;
  size_t IMAGE_CHANNEL = 3;
  paddle_matrix mat = paddle_matrix_create(/* sample_num */ 1,
                                           /* size */ IMAGE_CHANNEL * IMAGE_HEIGHT * IMAGE_WIDTH,
                                           /* useGPU */ false);
  CHECK(paddle_arguments_set_value(in_args, 0, mat));

  // Get First row.
  paddle_real* array;
  CHECK(paddle_matrix_get_row(mat, 0, &array));

  std::vector<float> means({104, 117, 124});
  size_t index = 0;
  for (size_t c = 0; c < IMAGE_CHANNEL; ++c) {
    for (size_t h = 0; h < IMAGE_HEIGHT; ++h) {
      for (size_t w = 0; w < IMAGE_WIDTH; ++w) {
        array[index] = index % 255 - means[c];
        index++;
      }
    }
  }

  // Step 4: Do inference.
  paddle_arguments out_args = paddle_arguments_create_none();
  {
    CHECK(paddle_gradient_machine_forward(machine,
                                          in_args,
                                          out_args,
                                          /* isTrain */ false));
  }

  // Step 5: Get the result.
  paddle_matrix probs = paddle_matrix_create_none();
  CHECK(paddle_arguments_get_value(out_args, 0, probs));

  uint64_t height = 0;
  uint64_t width = 0;
  CHECK(paddle_matrix_get_shape(probs, &height, &width));
  CHECK(paddle_matrix_get_row(probs, 0, &array));

  printf("Prob: %lld x %lld\n", height, width);
  // for (uint64_t i = 0; i < height; i++) {
  //   int maxid = 0;
  //   for (uint64_t j = 0; j < width; j++) {
  //     if (array[i * width + j] > array[i * width + maxid]) {
  //       maxid = j;
  //     }
  //   }
  //   printf("max probs of %d-th: %d - %f\n", i, maxid, array[i * width + maxid]);
  //   // printf("\n");
  // }
  // printf("\n");

  // Step 6: Release the resources.
  CHECK(paddle_gradient_machine_destroy(machine));
  CHECK(paddle_arguments_destroy(in_args));
  CHECK(paddle_matrix_destroy(mat));
  CHECK(paddle_arguments_destroy(out_args));
  CHECK(paddle_matrix_destroy(probs));

  return 0;
}
