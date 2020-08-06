#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>

#define MIN(a, b)  ( (a) < (b) ? (a) : (b) )

static uint16_t read_ui16(const unsigned char *data)
{
  return *(uint16_t *)data;
}

static uint32_t read_ui32(const unsigned char *data)
{
  return *(uint32_t *)data;
}

static void print_offset(const uint32_t offset)
{
  printf("%04X:%04X   ", (offset >> 16) & 0xffff, offset & 0xffff);
}

static void print_hex_string(const unsigned char *data, const size_t len)
{
  fputs("  ", stdout);
  for(size_t i = 0; i < MIN(16, len); i++)
  {
    unsigned char c = data[i];
    putchar(isprint(c) ? c : '.');
  }
  putchar('\n');
}

static void print_hex(const unsigned char *file_data, const unsigned char *data, const size_t len)
{
  size_t len_round_up = ((len + 15) / 16) * 16;
  for(size_t i = 0; i < len_round_up; i++)
  {
    if(i % 16 == 0)
      print_offset(data + i - file_data);

    if(i < len)
      printf("%02X", data[i]);
    else
      fputs("  ", stdout);

    if(i % 16 == 15)
      print_hex_string(data + i - 15, MIN(16, len - i + 15));
    else if(i % 4 == 3)
      fputs("   ", stdout);
    else
      putchar(' ');
  }
}

int main(int argc, char *argv[])
{
  if(argc != 5)
  {
    fprintf(stderr, "Usage: %s <input file> <output PFM> <offset> <width>\n", argv[0]);
    exit(1);
  }

  const char *filename_in = argv[1];
  const char *filename_out = argv[2];
  size_t offset = atol(argv[3]);
  size_t width = atol(argv[4]);

  // read the file and get its size
  FILE *fd = fopen(filename_in, "rb");
  if(!fd)
  {
    fprintf(stderr, "error opening input file\n");
    exit(1);
  }

  fseek(fd, 0, SEEK_END);
  size_t file_size = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  unsigned char *file_data = (unsigned char *)malloc(file_size);

  if(fread(file_data, 1, file_size, fd) != file_size)
  {
    fprintf(stderr, "error reading file\n");
    free(file_data);
    exit(1);
  }

  fclose(fd);

  const size_t mystery_header_size = 1496/*128 + 8 * width*/;

  if(offset + mystery_header_size > file_size)
  {
    fprintf(stderr, "offset is pointing behind EOF\n");
    exit(1);
  }

  const size_t rest_size = file_size - offset - mystery_header_size;

  const unsigned char *mystery_header = file_data + offset;
  const unsigned char *start = mystery_header + mystery_header_size;

//   const uint16_t width = read_ui16(mystery_header + 12);
//   const uint16_t height = read_ui16(mystery_header + 14);
//   const size_t height = (rest_size + width - 1) / width;
  const size_t height = 228;
  const unsigned char *mystery_footer = start + (size_t)width * height;
  const size_t mystery_footer_size = file_data + file_size - mystery_footer;


  printf("rest: %ld, %ld x %ld\n", rest_size, width, height);

  // header of the depth data
  printf("mystery header size: %ld\n", mystery_header_size);

  puts("mystery header:");
  print_hex(file_data, mystery_header, mystery_header_size);

  // yes, this reads over the end of the header ...
//   putchar('\n');
//   puts("offset   as 16 bit     as 32 bit      32 bit hex");
//   for(size_t i = 0; i < mystery_header_size; i++)
//     printf("%6ld: %10u    %10u      0x%08x\n", i, read_ui16(mystery_header + i), read_ui32(mystery_header + i), read_ui32(mystery_header + i));

//   if(start + (size_t)width * height > file_data + file_size)
//   {
//     fprintf(stderr, "premature end of file\n");
//     exit(1);
//   }

  // data following the depth data
  printf("mystery footer size: %ld\n", mystery_footer_size);

  puts("mystery footer:");
  print_hex(file_data, mystery_footer, mystery_footer_size);

  // yes, this reads over the end of the data ...
//   putchar('\n');
//   puts("offset   as 16 bit     as 32 bit      32 bit hex");
//   for(size_t i = 0; i < mystery_footer_size; i++)
//     printf("%6ld: %10u    %10u      0x%08x\n", i, read_ui16(mystery_footer + i), read_ui32(mystery_footer + i), read_ui32(mystery_footer + i));

  // write the footer
//   fd = fopen("mystery_footer.data", "wb");
//   if(!fd)
//   {
//     fprintf(stderr, "error opening footer file\n");
//     exit(1);
//   }
//   fwrite(mystery_footer, 1, mystery_footer_size, fd);
//   fclose(fd);

  // write the depth map
  fd = fopen(filename_out, "wb");
  if(!fd)
  {
    fprintf(stderr, "error opening output file\n");
    exit(1);
  }

  const size_t missing_size = offset + mystery_header_size + width * height - file_size;

  const int header_size = fprintf(fd, "P5\n%ld %ld\n255\n", width, height);
  fwrite(start, 1, width * height, fd);
  if(missing_size > 0)
  {
    fseek(fd, width * height + header_size - 1, SEEK_SET);
    fputc(0, fd);
  }
  fclose(fd);

  return 0;
}
