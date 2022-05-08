section .data
    msg: db "Hello, Infected File",10 ,0	; format string

section .text
global _start
global system_call
global code_start
global code_end
global infection
global infector
extern main
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop

system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:

infection:
  push    ebp             ; Save caller state
  mov     ebp, esp
  pushad                  ; Save some more caller state

  mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...
  AND     eax, 1          ; check LSB
  cmp     eax, 1
  JE      end_infection

  mov eax, 4 ;SYS_WRITE opcode is 4
  mov ebx, 1 ;write to stdout
  mov ecx, msg ;string to write
  mov edx, 20 ; string len
  int 0x80 ; enter kernel mode
end_infection:
  popad                   ; Restore caller state (registers)
  mov esp, ebp
  pop ebp
  ret
code_end:


infector:
  push    ebp             ; Save caller state
  mov     ebp, esp
  pushad                  ; Save some more caller state

  ;open file
  mov eax, 5 ;SYS_OPEN opcode is 5
  mov ebx, [ebp+8] ;file path arguments
  mov ecx, 0x401 ; O_APPEND
  mov edx, 0777
  int 0x80 ; enter kernel mode

  ;append from code_start to code_end
  mov ebx, eax ;save file_descriptor at ebx
  mov eax, 4 ;SYS_WRITE opcode is 4
  mov ecx, code_start ; first byte to write
  mov edx, code_end - code_start ; num of bytes to write
  int 0x80 ; enter kernel mode

  ;close file
  mov eax, 6 ;SYS_CLOSE opcode number is 6
  int 0x80 ; enter kernel mode


  ;end infector
  popad                   ; Restore caller state (registers)
  mov esp, ebp
  pop ebp
  ret
