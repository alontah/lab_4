
#include "util.h"


extern int system_call();

#define SYS_WRITE 4
#define SYS_READ 3
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_ISEEK 19


#define O_RDRW 0x002



int main (int argc , char* argv[], char* envp[])
{
  int file_descriptor, file_len, new_name_len, name_len, i, out_index;
  char* name = "Shira";
  char* new_name = "Mira";

  name_len = 6;
  new_name_len = 5;
  file_descriptor = system_call(SYS_OPEN, "greeting", O_RDRW, 0777);
  file_len = system_call(SYS_ISEEK, file_descriptor, 0, 2);
  system_call(SYS_ISEEK, file_descriptor, 0, 0);
  char buffer[file_len];
  system_call(SYS_READ, file_descriptor, &buffer, file_len);
  char out_buffer[file_len];
  out_index = 0;

  for(i = 0; i < file_len - name_len; i++, out_index++){
    if(strncmp(name, &buffer[i], 5) == 0){
      strcat(&out_buffer[out_index], new_name);
      i += name_len-1;
      out_index += new_name_len-1;
    } else {
      out_buffer[out_index] = buffer[i];
    }
  }
  system_call(SYS_ISEEK, file_descriptor, 0, 0);
  system_call(SYS_WRITE, file_descriptor, out_buffer, out_index);
  system_call(SYS_CLOSE, file_descriptor);
  return 0;
}
