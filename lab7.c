#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  int line_number;
  int value;
} Input;
typedef struct {
  int line_number;
  int doubled_value;
} IntermediateInput;
typedef struct {
  int doubled_value;
  int line_numbers[1024];
  int count;
} Output;
void map(Input *in, IntermediateInput *out);
void groupByKey(IntermediateInput *in, Output *our_arr, int *result_count);
void reduce(Output out);
int main(void) {
  printf("Enter values (one per line). Type 'end' to finish:\n");
  Input inputs[1024];
  int n_in = 0;
  while (1) {
    char temp[64];
    if (scanf("%63s", temp) != 1) {
      break;
    }
    if (strcmp(temp, "end") == 0) {
      break;
    }
    int v = atoi(temp);
    inputs[n_in].line_number = n_in + 1;
    inputs[n_in].value = v;
    n_in++;
  }
  if (n_in == 0) {
    return 0;
  }
  IntermediateInput inter[1024];
  for (int i = 0; i < n_in; i++) {
    map(&inputs[i], &inter[i]);
  }
  Output outputs[1024];
  int n_out = 0;
  for (int i = 0; i < n_in; i++) {
    groupByKey(&inter[i], outputs, &n_out);
  }
  for (int i = 0; i < n_out; i++) {
    reduce(outputs[i]);
  }
  return 0;
}

void map(Input *input, IntermediateInput *intermediate_input) {
  intermediate_input->line_number = input->line_number;
  intermediate_input->doubled_value = input->value * 2;
}
void groupByKey(IntermediateInput *input, Output *output_array,
                int *result_count) {
  for (int i = 0; i < *result_count; i++) {
    if (output_array[i].doubled_value == input->doubled_value) {
      output_array[i].line_numbers[output_array[i].count++] =
          input->line_number;
      return;
    }
  }
  Output *o = &output_array[*result_count];
  o->doubled_value = input->doubled_value;
  o->count = 0;
  o->line_numbers[o->count++] = input->line_number;
  (*result_count)++;
}
void reduce(Output output) {
  printf("(%d, [", output.doubled_value);
  for (int i = 0; i < output.count; i++) {
    if (i)
      printf(", ");
    printf("%d", output.line_numbers[i]);
  }
  printf("])\n");
}
