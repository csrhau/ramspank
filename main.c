#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define GETOPTS "o:i:a:p:"
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

void initial_scan(int n, double a, double *arr) {
  for(int i = 0; i < n; ++i) {
    arr[i] = a;
  }
}

void saxpy(int start, int end, double a, double *xarr, double *yarr) {
  for (int i = start; i < end; ++i) {
    yarr[i] = a*xarr[i] + yarr[i];
  }
}

void stepping_scan(int pages,
                   int page_doubles,
                   int page_step, 
                   int start_page, 
                   double a, 
                   double *xarr, 
                   double *yarr) {
  for (int page = start_page; page < pages; page+=page_step) {
    int page_start = page * page_doubles;
    int page_end = page_start + page_doubles;
    saxpy(page_start, page_end, a, xarr, yarr);
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
  if (pagesize % sizeof(double) != 0
   || arraysize % sizeof(double) != 0) {
    fprintf(stderr, "Error! pagesize(%d) and arraysize(%d) must be multiples of sizeof(double)(%lu)\n",
        pagesize, arraysize, sizeof(double));
    return EXIT_FAILURE;
  }
  int doubles = arraysize / sizeof(double);
  int page_doubles = pagesize / sizeof(double);
  int pages = doubles / page_doubles;
  double *xarr = (double *) malloc(doubles * sizeof(double));
  double *yarr = (double *) malloc(doubles * sizeof(double));
  if (!xarr || !yarr) {
    fprintf(stderr, "Unable to allocate arrays! Exiting\n");
    return EXIT_FAILURE;
  } else {
    printf("Allocated two arrays of length %d\n", doubles);
  }
  for (int i = 0; i < outer; ++i) {
    // Linear Touch Scan
    printf("Linear Touch Scan\n");
    for (int j = 0; j <inner; ++j) {
      initial_scan(doubles, 2.0, xarr);
      initial_scan(doubles, 0.5, yarr);
    }
    // Linear Page Scan
    printf("Linear Page Scan\n");
    for (int j = 0; j <inner; ++j) {
      stepping_scan(pages, page_doubles, 1, 0, 1.24, xarr, yarr);
    }
    printf("Braided Page Scan\n");
    // Braided Stepwise Scan
    for (int j = 0; j <inner; ++j) {
      stepping_scan(pages, page_doubles, 4, 0, 1.24, xarr, yarr);
      stepping_scan(pages, page_doubles, 4, 1, 1.24, xarr, yarr);
      stepping_scan(pages, page_doubles, 4, 2, 1.24, xarr, yarr);
      stepping_scan(pages, page_doubles, 4, 3, 1.24, xarr, yarr);
    }
  }
  free(xarr);
  free(yarr);
  return EXIT_SUCCESS;
}
