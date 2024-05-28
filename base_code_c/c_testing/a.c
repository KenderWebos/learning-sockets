#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("No se han especificado argumentos.\n");
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    printf("Argumento %d: %s\n", i, argv[i]);
  }

  return 0;
}