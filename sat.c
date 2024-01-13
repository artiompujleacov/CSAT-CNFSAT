#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int clauses = 1;

typedef struct {
  char type[4];
  int id;
  int inputs[60];
  int nr_inputs;
} Gate;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <input_file> <output_file>\n", argv[0]);
    exit(1);
  }
  FILE *f = fopen(argv[1], "r");
  int n, m;
  fscanf(f, "%d %d\n", &n, &m);
  Gate *gates = malloc(sizeof(Gate) * (m + 1));
  while (!feof(f)) {
    char type[4];
    int inputs[60], id, cont = 0;
    fscanf(f, "%s", type);
    if (strcmp(type, "NOT") == 0) {
      fscanf(f, "%d %d", &inputs[0], &id);
      strcpy(gates[id].type, type);
      gates[id].id = id;
      gates[id].inputs[0] = inputs[0];
      gates[id].nr_inputs = 1;
      clauses = clauses + 2;
    } else {
      int i;
      for (i = 0; i < 60; i++) {
        if (fscanf(f, "%d", &inputs[i]) != 1) {
          break;
        }
        cont++;
      }
      id = inputs[i - 1];
      inputs[i - 1] = 0;
      strcpy(gates[id].type, type);
      gates[id].id = id;
      memcpy(gates[id].inputs, inputs, sizeof(inputs));
      gates[id].nr_inputs = cont - 1;
      clauses += cont;
    }
  }
  fclose(f);
  FILE *fp = fopen(argv[2], "w");
  fprintf(fp, "p cnf %d %d\n", m, clauses);
  for (int i = 0; i <= m; i++) {
    if (strcmp(gates[i].type, "NOT") == 0) {
      fprintf(fp, "%d %d 0\n", -gates[i].id, -gates[i].inputs[0]);
      fprintf(fp, "%d %d 0\n", gates[i].id, gates[i].inputs[0]);
    } else if (strcmp(gates[i].type, "AND") == 0) {
      fprintf(fp, "%d ", -gates[i].id);
      fprintf(fp, "%d 0\n", gates[i].id);
      for (int k = 0; k < gates[i].nr_inputs; k++) {
        fprintf(fp, "%d %d 0\n", -gates[i].id, gates[i].inputs[k]);
      }
    } else if (strcmp(gates[i].type, "OR") == 0) {
      for (int j = 0; j < gates[i].nr_inputs; j++) {
        fprintf(fp, "%d ", gates[i].inputs[j]);
      }
      fprintf(fp, "%d 0\n", -gates[i].id);
      for (int k = 0; k < gates[i].nr_inputs; k++) {
        fprintf(fp, "%d %d 0\n", gates[i].id, -gates[i].inputs[k]);
      }
    }
  }
  fprintf(fp, "%d 0\n", m);
  fclose(fp);
  free(gates);
  return 0;
}
