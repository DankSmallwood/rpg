#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool isfile(char *f) {
  struct stat s;
  stat(f, &s);
  return S_ISREG(s.st_mode);
}

char *cat(char *s1, char *s2) {
  char *s = calloc(1, strlen(s1) + strlen(s2) + 1);
  strcat(s,s1);
  strcat(s,s2);
  return s;
}

bool endswith(char *s, char *ending) {
  size_t ending_len = strlen(ending);
  size_t s_len = strlen(s);

  if(ending_len > s_len) return false;
  return strcmp(s + (s_len - ending_len), ending) == 0;
}

int main(int argc, char *argv[]) {
  if(argc != 2) { 
    fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
    return EXIT_FAILURE;
  }

  DIR *dir = opendir(argv[1]);
  if(!dir) {
    fprintf(stderr, "Could not open %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  char *output_name = cat(argv[1], "/_all.x");
  FILE *output = fopen(output_name, "w+");
  if(!output) {
    fprintf(stderr, "Could not open output file %s\n", output_name);
    return EXIT_FAILURE;
  }
  free(output_name);

  fprintf(output, "#include \"_defaults.x\"\n");
  for(struct dirent *d; (d = readdir(dir)) != NULL;) {
    char *fname = cat(argv[1], d->d_name);
    if(
        isfile(fname) &&
        d->d_name[0] != '_' &&
        endswith(d->d_name, ".x")
      ) {
      fprintf(output, "#include \"%s\"\n", d->d_name);
    }

    free(fname);
  }
  fprintf(output, "#include \"_reset.x\"\n");

  return EXIT_SUCCESS;
}
