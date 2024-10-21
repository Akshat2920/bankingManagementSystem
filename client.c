#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 8080

void login_customer(){
  printf("1. view account info\n2. withdraw money\n3. Deposit money\n 4.Send money\n5. Exit\n");
}
void customer_base(){
  printf("1. Create new account\n2. Login\n3. Exit\n");
}
void login_employee(){
  printf("1. Modify customer detaile\n2. Process Loan Applications\n3. Approve Reject loans\n 4. View Assigned loans\n");
}
void employee_base(){
  printf("1. Login\n2. Exit\n");
}
void to_serv(int sock){
  char arg[100];
  scanf("%s", arg);
  send(sock, arg, strlen(arg), 0);
  memset(arg, 0, sizeof(arg));
}
void choose(int sock){
  int choicet;
  scanf("%d", &choicet);
  send(sock, &choicet, sizeof(choicet), 0);
}

int main() {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    int choice = 0;
    char buffer[1024] = {0};
    char name[50];
    char pass[50];
    int id = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Client connected to server\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    printf("Enter your choice: ");
    scanf("%d", &choice);
    send(sock, &choice, sizeof(choice), 0);
    printf("Choice sent to server\n");
    memset(buffer, 0, sizeof(buffer));
    while(1){
      valread = read(sock, buffer, 1024);
      if(strcmp(buffer, "CUSTOMER_BASE") == 0) {
        customer_base();
        printf("Enter your choice: ");
        choose(sock);
        memset(buffer, 0, sizeof(buffer));
      
      } else if (strcmp(buffer, "CREATE_USER") == 0) {
        printf("Enter name: ");
        to_serv(sock);
        printf("Enter password: ");
        to_serv(sock);
        memset(buffer, 0, sizeof(buffer));
      
      } else if (strcmp(buffer, "LOGIN_CUST") == 0){
        printf("Enter account id: ");
        scanf("%d", &id);
        send(sock, &id, sizeof(id), 0);
        printf("Enter password: ");
        to_serv(sock);
       memset(buffer, 0, sizeof(buffer));
      
      } else if (strcmp(buffer, "LOGIN_SUCCESS") == 0){
        printf("logged in\n");
        login_customer();
        choose(sock);
        int out = read(sock, buffer, 1024);;
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
      
      } else if (strcmp(buffer, "LOGIN_FAIL") == 0){
        printf("incorrect id or password\nTry again");
        memset(buffer, 0, sizeof(buffer));
      
      } else if (strcmp(buffer, "GET_AMMOUNT") == 0) {
        double amt;
        printf("Enter ammount: ");
        scanf("%lf", &amt);
        send(sock, &amt, sizeof(amt), 0);
        memset(buffer, 0, sizeof(buffer));
        
      } else if (strcmp(buffer, "GET_AMMOUNT") == 0) {
        double amt;
        printf("Enter ammount: ");
        scanf("%lf", &amt);
        send(sock, &amt, sizeof(amt), 0);
        printf("enter reciever account id: ");
        choose(sock);
        memset(buffer, 0, sizeof(buffer));

      } else if(strcmp(buffer, "EMPLOYEE_BASE") == 0) {
        employee_base();
        printf("Enter your choice: ");
        choose(sock);
        memset(buffer, 0, sizeof(buffer)); 
    
      } else if (strcmp(buffer, "LOGIN_SUCCESS_E") == 0){
        printf("logged in\n");
        login_employee();
        choose(sock);
        int out = read(sock, buffer, 1024);;
        printf("%s", buffer);
        memset(buffer, 0, sizeof(buffer));
      
      } else if (strcmp(buffer, "GET_NEW_DETAILS") == 0){
        int ac;
        const char* new_customer_name; const char* new_password; double new_bank_balance;
        printf("enter account no:");
        choose(sock);
        printf("enter new customer name: ");
        to_serv(sock);
        printf("Enter new pasword: ");
        to_serv(sock);
        printf("enter new bank balance: ");
        to_serv(sock);
        memset(buffer, 0, sizeof(buffer));
      
      } else { break; }
    }
  memset(buffer, 0, sizeof(buffer));
  return 0;
}
