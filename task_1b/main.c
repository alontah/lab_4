
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

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDRW 0x002
#define O_CREAT 0x100


#define EOF 0

char my_fgetc(int file_descriptor){
  char c;
  system_call(SYS_READ, file_descriptor, &c, 1);
  return c;
}

void my_printf(int file_descriptor, char* str){
  system_call(SYS_WRITE, file_descriptor, str, strlen(str));
}

int main (int argc , char* argv[], char* envp[])
{
  /*
    TODO add debug mode
  */
  int arg, num_of_words, space_flag;
  int input_descriptor = STDIN;
  int output_descriptor = STDOUT;
  int debug = 0;

  for(arg = 1; arg < argc; arg++){
    if(strncmp(argv[arg], "-D", 2) == 0){
      debug = 1;
    } else if(strncmp(argv[arg], "-i", 2) == 0){
      input_descriptor = system_call(5, argv[arg]+2, O_RDONLY, 0777);
      if(input_descriptor <= 0){
        my_printf(STDOUT, "no such file");
      }
    } else if(strncmp(argv[arg], "-o", 2) == 0){
      output_descriptor = system_call(5, argv[arg]+2, O_CREAT | O_WRONLY, 0777);
    }
  }

  num_of_words = 0;
  space_flag = 1;


  while(1){
    char c = my_fgetc(input_descriptor);
    if(c == EOF || (input_descriptor == STDIN && c == '\n')){
      break;
    }
    if(debug) my_printf(STDERR, &c);

    if(c != ' ' && c != '\n'){
      if(space_flag == 1){
        num_of_words++;
        space_flag = 0;
      }
    } else {
      space_flag = 1;
    }
  }




    char* num_str = itoa(num_of_words);
    my_printf(output_descriptor, num_str);

    if(input_descriptor != STDIN){
      system_call(SYS_CLOSE, input_descriptor);
      if(debug){
        my_printf(STDERR, "input closed");
        my_printf(STDOUT, "\n");
      }
    }

    if(output_descriptor != STDOUT){
      system_call(SYS_CLOSE, output_descriptor);
      if(debug){
        my_printf(STDERR, "output closed");
        my_printf(STDOUT, "\n");
      }
    }


  return 0;
}
