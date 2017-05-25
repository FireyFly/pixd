#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <err.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIN(X,Y) ((X) < (Y)? (X) : (Y))
typedef uint8_t u8;

//-- Options ----------------
struct offset_range { off_t start, end; };

size_t option_columns = 64;
struct offset_range option_range  = { 0, -1 };

//-- Hexdump impl -----------
uint32_t colormap[256] = {
  0x000000, 0x560000, 0x640000, 0x750000, 0x870000, 0x9b0000, 0xb00000, 0xc60000, 0xdd0000, 0xf50000, 0xff0f0f, 0xff2828, 0xff4343, 0xff5e5e, 0xff7979, 0xfe9595,
  0x4c1600, 0x561900, 0x641e00, 0x752300, 0x872800, 0x9b2e00, 0xb03400, 0xc63b00, 0xdd4200, 0xf54900, 0xff570f, 0xff6928, 0xff7b43, 0xff8e5e, 0xffa179, 0xfeb595,
  0x4c3900, 0x564000, 0x644b00, 0x755700, 0x876500, 0x9b7400, 0xb08400, 0xc69400, 0xdda600, 0xf5b800, 0xffc30f, 0xffc928, 0xffd043, 0xffd65e, 0xffdd79, 0xfee495,
  0x4c4c00, 0x565600, 0x646400, 0x757500, 0x878700, 0x9b9b00, 0xb0b000, 0xc6c600, 0xdddd00, 0xf5f500, 0xffff0f, 0xffff28, 0xffff43, 0xffff5e, 0xffff79, 0xfffe95,
  0x324c00, 0x395600, 0x426400, 0x4e7500, 0x5a8700, 0x679b00, 0x75b000, 0x84c600, 0x93dd00, 0xa3f500, 0xafff0f, 0xb7ff28, 0xc0ff43, 0xc9ff5e, 0xd2ff79, 0xdbfe95,
  0x1f4c00, 0x235600, 0x296400, 0x307500, 0x388700, 0x409b00, 0x49b000, 0x52c600, 0x5cdd00, 0x66f500, 0x73ff0f, 0x82ff28, 0x91ff43, 0xa1ff5e, 0xb1ff79, 0xc1fe95,
  0x004c00, 0x005600, 0x006400, 0x007500, 0x008700, 0x009b00, 0x00b000, 0x00c600, 0x00dd00, 0x00f500, 0x0fff0f, 0x28ff28, 0x43ff43, 0x5eff5e, 0x79ff79, 0x95fe95,
  0x004c19, 0x00561c, 0x006421, 0x007527, 0x00872d, 0x009b33, 0x00b03a, 0x00c642, 0x00dd49, 0x00f551, 0x0fff5f, 0x28ff70, 0x43ff81, 0x5eff93, 0x79ffa6, 0x95feb8,
  0x004c4c, 0x005656, 0x006464, 0x007575, 0x008787, 0x009b9b, 0x00b0b0, 0x00c6c6, 0x00dddd, 0x00f5f5, 0x0ffffe, 0x28fffe, 0x43fffe, 0x5efffe, 0x79ffff, 0x95fffe,
  0x00394c, 0x004056, 0x004b64, 0x005775, 0x006587, 0x00749b, 0x0084b0, 0x0094c6, 0x00a6dd, 0x00b8f5, 0x0fc3ff, 0x28c9ff, 0x43d0ff, 0x5ed6ff, 0x79ddff, 0x95e4fe,
  0x00264c, 0x002b56, 0x003264, 0x003a75, 0x004387, 0x004d9b, 0x0058b0, 0x0063c6, 0x006edd, 0x007af5, 0x0f87ff, 0x2893ff, 0x43a1ff, 0x5eaeff, 0x79bcff, 0x95cafe,
  0x00134c, 0x001556, 0x001964, 0x001d75, 0x002187, 0x00269b, 0x002cb0, 0x0031c6, 0x0037dd, 0x003df5, 0x0f4bff, 0x285eff, 0x4372ff, 0x5e86ff, 0x799aff, 0x95b0fe,
  0x19004c, 0x1c0056, 0x210064, 0x270075, 0x2d0087, 0x33009b, 0x3a00b0, 0x4200c6, 0x4900dd, 0x5100f5, 0x5f0fff, 0x7028ff, 0x8143ff, 0x935eff, 0xa679ff, 0xb895fe,
  0x33004c, 0x390056, 0x420064, 0x4e0075, 0x5a0087, 0x67009b, 0x7500b0, 0x8400c6, 0x9300dd, 0xa300f5, 0xaf0fff, 0xb728ff, 0xc043ff, 0xc95eff, 0xd279ff, 0xdb95fe,
  0x4c004c, 0x560056, 0x640064, 0x750075, 0x870087, 0x9b009b, 0xb000b0, 0xc600c6, 0xdd00dd, 0xf500f5, 0xfe0fff, 0xfe28ff, 0xfe43ff, 0xfe5eff, 0xfe79ff, 0xfe95fe,
  0x4c0032, 0x560039, 0x640042, 0x75004e, 0x87005a, 0x9b0067, 0xb00075, 0xc60084, 0xdd0093, 0xf500a3, 0xff0faf, 0xff28b7, 0xff43c0, 0xff5ec9, 0xff79d2, 0xffffff,
};

void hexdump(FILE *f, const char *filename) {
  u8 buf[BUFSIZ];

  // Seek to start; fall back to a consuming loop for non-seekable files
  if (fseeko(f, option_range.start, SEEK_SET) < 0) {
    off_t remaining = option_range.start;
    while (remaining != 0 && !feof(f) && !ferror(f)) {
      size_t n = fread(buf, 1, MIN(remaining, BUFSIZ), f);
      remaining -= n;
      if (n == 0) break;
    }
    if (ferror(f)) err(1, "(while seeking) %s", filename);
  }

  off_t count = (option_range.end - option_range.start);
  off_t read = 0;
  while (read != count && !feof(f) && !ferror(f)) {
    size_t to_read = option_range.end == -1? BUFSIZ : MIN(count - read, BUFSIZ);
    size_t n = fread(buf, 1, to_read, f);

    for (size_t i = 0; i < n; i += option_columns*2) {
      u8 *p = &buf[i];

      // Offset
      size_t offset = option_range.start + read + i;
      printf("%5zx%03zx ", offset >> 12, offset & 0xFFF);

      // Print two rows
      const char *prev_fmt = NULL;
      for (size_t j = 0; j < option_columns; j++) {
        #define EXPAND(v) ((v)>>16 & 0xFF), ((v)>>8 & 0xFF), ((v)>>0 & 0xFF)
        if (i + j >= n) {
          break;
        } else if (i + j + option_columns >= n) {
          printf("\x1B[0m\x1B[38;2;%d;%d;%dm▀",
                 EXPAND(colormap[buf[i + j]]));
        } else {
          printf("\x1B[38;2;%d;%d;%dm\x1B[48;2;%d;%d;%dm▀",
                 EXPAND(colormap[buf[i + j]]),
                 EXPAND(colormap[buf[i + j + option_columns]]));
        }
        #undef EXPAND
      }

      printf("\x1B[m\n");
    }

    read += n;
  }

  if (ferror(f)) err(1, "(while reading) %s", filename);
}

//-- Entry point ------------
/** Parses a range "start-end" (both ends optional) or "start+size" (neither
 *  optional) into a `struct offset_range` instance. */
struct offset_range parse_range(const char *str) {
  struct offset_range res = { 0, -1 };
  const char *first = str, *delim = str + strcspn(str, "+-"), *second = delim + 1;
  if (*delim == '\0') errx(1, "no delimiter in range %s", str);

  char *end;
  if (first != delim) {
    res.start = strtoimax(first, &end, 0);
    if (!isdigit(*first) || end != delim) errx(1, "invalid start value in range %s", str);
  }
  if (*second != '\0') {
    res.end = strtoimax(second, &end, 0);
    if (!isdigit(*second) || *end != '\0') errx(1, "invalid end/size value in range %s", str);
  }

  if (*delim == '+') {
    if (first == delim) errx(1, "start unspecified in range %s", str);
    if (*second == '\0') errx(1, "size unspecified in range %s", str);
    res.end += res.start;
  }

  if (res.end < res.start && res.end != -1) errx(1, "end was less than start in range %s", str);
  return res;
}

int main(int argc, char *argv[]) {
  // Parse options
  int opt;
  while (opt = getopt(argc, argv, "g:pr:w:"), opt != -1) {
    switch (opt) {
      case 'r': option_range = parse_range(optarg); break;
      case 'w': option_columns = atol(optarg); break;
      default:
        fprintf(stderr, "usage: pixd [-r range] [-w width]\n");
        return 1;
    }
  }
  argc -= optind;
  argv += optind;

  // Parse PIXD_COLORS
  char *colors_var = getenv("PIXD_COLORS");
  if (colors_var != NULL) {
    colors_var = strdup(colors_var);
    if (colors_var == NULL) errx(1, "strdup");

    size_t n = 0;
    for (char *p = colors_var, *token; token = strtok(p, " \v\r\n"), token != NULL; p = NULL) {
      char *end;
      uint32_t color = strtol(token, &end, 16);
      if (strlen(token) != 6 || *end != '\0') {
        errx(1, "error in PIXD_COLORS: '%s' is not a valid hex colour", token);
      }

      if (n == 256) {
        warnx("more than 256 values in PIXD_COLORS; ignoring excess");
        break;
      }

      colormap[n++] = color;
    }

    if (n != 256) errx(1, "error in PIXD_COLORS: expected 256 colours but found %zu", n);
  }

  // Hexdump files
  if (argc == 0) {
    hexdump(stdin, "(stdin)");
  } else {
    for (size_t i = 0; i < argc; i++) {
      FILE *f = fopen(argv[i], "r");
      if (f == NULL) {
        warn("%s", argv[i]);
        continue;
      }

      if (argc > 1) {
        printf("%s====> %s%s%s <====\n", i > 0? "\n" : "", "\x1B[1m", argv[i], "\x1B[m");
      }

      hexdump(f, argv[i]);
      fclose(f);
    }
  }

  free(colors_var);
  return 0;
}
