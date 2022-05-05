
#include "util.h"


extern int system_call();

#define SYS_WRITE 4
#define SYS_READ 3
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_ISEEK 19

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define O_RDRW 0x002
#define O_RDONLY 0x000

char my_fgetc(int file_descriptor){
  char c;
  system_call(SYS_READ, file_descriptor, &c, 1);
  return c;
}

int main (int argc , char* argv[], char* envp[])
{
  /*
    TODO add debug mode
  */
  int file_descriptor, file_len, arg, num_of_words, space_flag, i;
  num_of_words = 0;
  space_flag = 0;
  arg = 1;

  file_descriptor = system_call(5, argv[arg], O_RDONLY, 0777);
  file_len = system_call(SYS_ISEEK, file_descriptor, 0, 2);
  system_call(SYS_ISEEK, file_descriptor, 0, 0);

  /* removing padding spaces and \n */
  for(i = 0; i < file_len; i++){
    char c = my_fgetc(file_descriptor);
    if(c != ' ' && c != '\n'){
      break;
    }
  }
  if(i == file_len){
    /* no words at all */
    system_call(SYS_WRITE, STDOUT, "0", 1);
  } else {
    system_call(SYS_ISEEK, file_descriptor, -1, 1);
    for(--i; i < file_len; i++){
      char c = my_fgetc(file_descriptor);
      if(c == ' ' || c == '\n'){
        if(space_flag == 0){
          num_of_words++;
          space_flag = 1;
        }
      } else if(space_flag == 1){
        space_flag = 0;
      }
    }


    char* num_str = itoa(num_of_words);
    system_call(SYS_WRITE, STDOUT, num_str, strlen(num_str));
  }

    system_call(SYS_CLOSE, file_descriptor);
  return 0;
}
