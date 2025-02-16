#include "bsp.h"
#include "iob_soc_tester_system.h"
#include "iob_soc_tester_periphs.h"
#include "iob_soc_tester_conf.h"
#include "iob-uart.h"
#include "printf.h"
#include "iob_str.h"

char *send_string = "Sending this string as a file to console.\n"
                    "The file is then requested back from console.\n"
                    "The sent file is compared to the received file to confirm " 
                    "correct file transfer via UART using console.\n"
                    "Generating the file in the firmware creates an uniform "
                    "file transfer between pc-emul, simulation and fpga without"
                    " adding extra targets for file generation.\n";

// copy src to dst
// return number of copied chars (excluding '\0')
int string_copy(char *dst, char *src) {
    if (dst == NULL || src == NULL) {
        return -1;
    }
    int cnt = 0;
    while(src[cnt] != 0){
        dst[cnt] = src[cnt];
        cnt++;
    }
    dst[cnt] = '\0';
    return cnt;
}

// 0: same string
// otherwise: different
int compare_str(char *str1, char *str2, int str_size) {
    int c = 0;
    while(c < str_size) {
        if (str1[c] != str2[c]){
            return str1[c] - str2[c];
        }
        c++;
    }
    return 0;
}

int main()
{
char pass_string[] = "Test passed!";
char fail_string[] = "Test failed!";

  //init uart
  uart_init(UART0_BASE,FREQ/BAUD);   

  //test puts
  uart_puts("\n\n\nHello from Tester!\n\n\n");

  uart_puts("\n\n\nDefault Tester firmware running...\n\n\n");

  //test file send
  char *sendfile = malloc(1000);
  int send_file_size = 0;
  send_file_size = string_copy(sendfile, send_string);
  uart_sendfile("Sendfile.txt", send_file_size, sendfile);

  //test file receive
  char *recvfile = malloc(10000);
  int file_size = 0;
  file_size = uart_recvfile("Sendfile.txt", recvfile);

  //compare files
  if (compare_str(sendfile, recvfile, send_file_size)) {
      printf("FAILURE: Send and received file differ!\n");
  } else {
      printf("SUCCESS: Send and received file match!\n");
  }

  free(sendfile);
  free(recvfile);

//#ifdef USE_EXTMEM
//  if(memory_access_failed)
//      uart_sendfile("test.log", iob_strlen(fail_string), fail_string);
//      uart_finish();
//#endif
  uart_sendfile("test.log", iob_strlen(pass_string), pass_string);

  uart_finish();
}
