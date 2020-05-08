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
  static char catbuf[2048];
  catbuf[0] = '\0';
  strcat(catbuf,s1);
  strcat(catbuf,s2);
  return catbuf;
}

FILE *open_file(char *fname) {
  FILE *file = fopen(fname, "w+");
  if(file == NULL) exit(EXIT_FAILURE);
  return file;
}

bool endswith(char *s, char *ending) {
  size_t ending_len = strlen(ending);
  size_t s_len = strlen(s);

  if(ending_len > s_len) return false;
  return strcmp(s + (s_len - ending_len), ending) == 0;
}

bool startswith(char *s, char *start) {
  return strncmp(s, start, strlen(start)) == 0;
}

char *read_line(FILE *f) {
  static char buffer[2048];
  return fgets(buffer, 2048, f);
}


void before_x(FILE *xsys, char *outdir) {
  FILE *out = open_file(cat(outdir,"_before.xsys"));
  fprintf(out, "XSTART()\n");
  fclose(out);
}

void after_x(FILE *xsys, char *outdir) {
  FILE *out = open_file(cat(outdir,"_after.xsys"));
  fprintf(out, "XEND()\n\n");

  for(char *line; (line = read_line(xsys));) {
    if(!startswith(line, "XPROP")) continue;

    strtok(line, " \r\n"); // Skip first token
    char *prop = strtok(NULL, " \r\n");
    fprintf(out, "#ifdef %s\n", prop);
    fprintf(out, "#undef %s\n", prop);
    fprintf(out, "#endif\n\n");
  }

  fclose(out);
}

void defaults_x(FILE *xsys, char *outdir) {
  FILE *out = open_file(cat(outdir,"_defaults.xsys"));

  for(char *line; (line = read_line(xsys));) {
    if(!startswith(line, "XIMPL")) continue;

    strtok(line, " \r\n"); // Skip first token
    char *impl = strtok(NULL, " \r\n");
    fprintf(out, "#ifndef %s\n", impl);
    fprintf(out, "#define %s(...)\n", impl);
    fprintf(out, "#endif\n\n");
  }

  fclose(out);
}

void reset_x(FILE *xsys, char *outdir) {
  FILE *out = open_file(cat(outdir,"_reset.xsys"));

  for(char *line; (line = read_line(xsys));) {
    if(!startswith(line, "XIMPL")) continue;

    strtok(line, " \r\n");
    char *impl = strtok(NULL, " \r\n");
    fprintf(out, "#undef %s\n\n", impl);
  }

  fclose(out);
}

void all_x(FILE *xsys, char *indir, char *outdir) {
  FILE *out = open_file(cat(outdir,"_all.xsys"));

  DIR *dir = opendir(indir);
  if(dir == NULL) {
    fprintf(stderr, "Cannot open directory\n");
    exit(EXIT_FAILURE);
  }

  fprintf(out, "#include \"_defaults.xsys\"\n\n");
  for(struct dirent *d; (d = readdir(dir)) != NULL;) {
    if(
        isfile(cat(indir,d->d_name)) &&
        !startswith(d->d_name, "_") &&
        endswith(d->d_name, ".x")
    ) {
      fprintf(out, "#include \"_before.xsys\"\n");
      fprintf(out, "#include \"%s\"\n", d->d_name);
      fprintf(out, "#include \"_after.xsys\"\n\n");
    }
  }

  fprintf(out, "#include \"_reset.xsys\"\n");
  fclose(out);
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <indir> <outdir>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  FILE *xsys = fopen(cat(argv[1],"_config.xsys"), "r");
  if(xsys == NULL) {
    fprintf(stderr, "Cannot open %s_config.xsys\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  before_x(xsys, argv[2]);
  fseek(xsys, 0, SEEK_SET);

  after_x(xsys, argv[2]);
  fseek(xsys, 0, SEEK_SET);

  defaults_x(xsys, argv[2]);
  fseek(xsys, 0, SEEK_SET);

  reset_x(xsys, argv[2]);
  fseek(xsys, 0, SEEK_SET);

  all_x(xsys, argv[1], argv[2]);
  fseek(xsys, 0, SEEK_SET);
}
