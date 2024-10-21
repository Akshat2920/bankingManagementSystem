#include "headers/client_handler.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "headers/customer_handler.h"

void client_handler(int connfd) {
  char wbuffer[] = "Login as:\n1. Customer\n2. Exit\n";
  char rbuffer[2] = {0};
  char invalid_choice[] = "Invalid choice, please try again\n";
  write(connfd, wbuffer, strlen(wbuffer));
  ssize_t n = read(connfd, rbuffer, sizeof(rbuffer) - 1);
  if (n < 0) {
    perror("read");
    return;
  }
  int choice = rbuffer[0] - '0';
  switch(choice) {
    case 1:
      customer_handler(connfd);
      break;
    case 2:
      write(connfd, "exiting\n", 9);
      close(connfd);
      return;
    default:
      write(connfd, invalid_choice, strlen(invalid_choice));
  }
}
