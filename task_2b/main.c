#include "util.h"


extern int system_call();

#define SYS_WRITE 4
#define SYS_READ 3
#define SYS_EXIT 1
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_ISEEK 19
#define SYS_GETDENTS 141

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDRW 0x002
#define O_CREAT 0x100

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WH 14

#define EOF 0


struct linux_dirent {
               unsigned long  d_ino;     /* Inode number */
               unsigned long  d_off;     /* Offset to next linux_dirent */
               unsigned short d_reclen;  /* Length of this linux_dirent */
               char           d_name[];  /* Filename (null-terminated) */
                                 /* length is actually (d_reclen - 2 -
                                    offsetof(struct linux_dirent, d_name)) */
               /*
               char           pad;       // Zero padding byte
               char           d_type;    // File type (only since Linux
                                         // 2.6.4); offset is (d_reclen - 1)
               */
};

char my_fgetc(int file_descriptor){
  char c;
  system_call(SYS_READ, file_descriptor, &c, 1);
  return c;
}

void my_printf(int file_descriptor, char* str){
  system_call(SYS_WRITE, file_descriptor, str, strlen(str));
}

void print_dirent_type(struct linux_dirent *dirent, char* buffer, int buffer_index){
  my_printf(STDOUT, ", file type: ");
  char d_type = *(buffer + buffer_index + dirent->d_reclen - 1);
  char* d_type_msg = (d_type == DT_REG) ?  "regular" :
                     (d_type == DT_DIR) ?  "directory" :
                     (d_type == DT_FIFO) ? "FIFO" :
                     (d_type == DT_SOCK) ? "socket" :
                     (d_type == DT_LNK) ?  "symlink" :
                     (d_type == DT_BLK) ?  "block dev" :
                     (d_type == DT_CHR) ?  "char dev" : "???";

  my_printf(STDOUT, d_type_msg);
}

void print_debug(struct linux_dirent *dirent, char* buffer, int buffer_index){
  my_printf(STDERR, "file name: ");
  my_printf(STDERR, dirent->d_name);
  my_printf(STDERR, " file length: ");
  my_printf(STDERR, itoa(dirent->d_reclen));
  print_dirent_type(dirent, buffer, buffer_index);

  my_printf(STDERR, "\n");
}

int main (int argc , char* argv[], char* envp[])
{

  int arg, prefix_len;
  int debug = 0;
  int has_prefix = 0;
  int BUFFER_SIZE = 8912;
  int buffer_index = 0;
  char buffer[BUFFER_SIZE];
  char* prefix;


  for(arg = 1; arg < argc; arg++){
    if(strncmp(argv[arg], "-D", 2) == 0){
      debug = 1;
    } else if(strncmp(argv[arg], "-p", 2) == 0){
      prefix = argv[arg] + 2;
      has_prefix = 1;
      prefix_len = strlen(prefix);
    }
  }

  int file_descriptor = system_call(SYS_OPEN, ".", O_RDONLY, 0777);
  if(file_descriptor <= 0) system_call(SYS_EXIT, 0x55);

  int num_of_bytes_to_read = system_call(SYS_GETDENTS, file_descriptor, buffer, BUFFER_SIZE);

  while(buffer_index < num_of_bytes_to_read){
    struct linux_dirent *dirent = (struct linux_dirent *) (buffer + buffer_index);
    if(!has_prefix || strncmp(prefix, dirent->d_name, prefix_len) == 0){
      my_printf(STDOUT, "file name: ");
      my_printf(STDOUT, dirent->d_name);
      print_dirent_type(dirent, buffer, buffer_index);
      my_printf(STDOUT, "\n");
    }
    if(debug){
      print_debug(dirent, buffer, buffer_index);
    }
    buffer_index += dirent->d_reclen;
  }

  return 0;
}
