#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define GETOPTS "o:i:a:p"
static char *progname;
static struct option long_opts[] = {
  {"outeriterations", required_argument, NULL, 'o'},
  {"inneriterations", required_argument, NULL, 'i'},
  {"arraysize", required_argument, NULL, 'a'},
  {"pagesize", required_argument, NULL, 'p'},
  {NULL, 0, NULL, 0}
};

void print_usage(void) {
  fprintf(stderr, "%s: a program to fiddle with various RAM pages\n", progname);
  fprintf(stderr, "Usage: %s [OPTIONS]\n", progname);
  fprintf(stderr, "-o --outeriterations     Outer Loop Iterations\n");
  fprintf(stderr, "-i --inneriterations     Outer Loop Iterations\n");
  fprintf(stderr, "-a --arraysize           Length of Array to allocate (bytes)\n");
  fprintf(stderr, "-p --pagesize            RAM Memory Page Size (bytes)\n");
}

void initial_scan(int n, double a, double *v) {
  for(int i = 0; i < n; ++i) {
    v[i] = a;
  }
}

void saxpy(int n, double a, double *x, double *y) {
  for (int i = 0; i < n; ++i) {
    y[i] = a*x[i] + y[i];
  }
}

void stepping_scan(int pages, int pagesize, int step, int start, double a, double *x, double *y) {
  // Linear Page Scan
  for (int page = start; page < pages; page+=step) {
    int offset = page * pagesize;
    saxpy(pagesize, a, x+offset, y+offset);
  }
}

int main(int argc, char *argv[]) {
  progname = argv[0];
  int outer = 0;
  int inner = 0;
  int arraysize = 0;
  int pagesize = 0;
  int optc = 0;
  int show_usage = 0;
  while ((optc = getopt_long(argc, argv, GETOPTS, long_opts, NULL)) != -1) {
    switch (optc) {
      case 'o':
        outer = atoi(optarg);
        break;
      case 'i':
        inner = atoi(optarg);
        break;
      case 'a':
        arraysize = atoi(optarg);
        break;
      case 'p':
        pagesize = atoi(optarg);
        break;
      default:
          show_usage = 1;
    }
  }
  if (show_usage || inner < 1 || outer < 1 || arraysize < 1 || pagesize < 1 || optind < argc) {
    print_usage();
    return EXIT_FAILURE;
  }
  printf("Starting\n");

  int doubles = arraysize / sizeof(double);
  int pages = arraysize / pagesize;
  double *x = malloc(arraysize);
  double *y = malloc(arraysize);
  for (int i = 0; i < outer; ++i) {
    // Linear Touch Scan
    for (int j = 0; j <inner; ++j) {
      initial_scan(doubles, 2.0, x);
      initial_scan(doubles, 0.5, y);
    }
    // Linear Page Scan
    for (int j = 0; j <inner; ++j) {
      stepping_scan(pages, pagesize, 1, 0, 1.25, x, y);
    }
    // Braided Stepwise Scan
    for (int j = 0; j <inner; ++j) {
      stepping_scan(pages, pagesize, 4, 0, 1.25, x, y);
      stepping_scan(pages, pagesize, 4, 1, 1.25, x, y);
      stepping_scan(pages, pagesize, 4, 2, 1.25, x, y);
      stepping_scan(pages, pagesize, 4, 3, 1.25, x, y);
    }
  }
  free(x);
  free(y);
  return EXIT_SUCCESS;
}
