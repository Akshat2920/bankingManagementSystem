
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct loan {
  int account;
  double amount;
  bool status;
};
struct Employee {
    int account_id;
    char password[50];
    char employee_name[50];
    loan assigned_loan_requests[100];
};
struct Customer {
    int account_id;
    char password[50];
    char customer_name[50];
    double bank_balance;
    char loan_status[20];
};

void modify_customer_info(int account_id, const char* new_customer_name, const char* new_password, double new_bank_balance) {
    FILE* file_read = fopen("../database/customer_db.txt", "r");
    FILE* temp_file = fopen("../database/temp_db.txt", "w");
    if (file_read == NULL || temp_file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file_read)) {
        struct Customer customer;
        sscanf(line, "%d,%49[^,],%49[^,],%lf,%19s", &customer.account_id, customer.password, customer.customer_name, &customer.bank_balance, customer.loan_status);
        if (customer.account_id == account_id) {
            strcpy(customer.customer_name, new_customer_name);
            strcpy(customer.password, new_password);
            customer.bank_balance = new_bank_balance;
            fprintf(temp_file, "%d,%s,%s,%.2f,%s\n", customer.account_id, customer.password, customer.customer_name, customer.bank_balance, customer.loan_status);
        } else {
            fprintf(temp_file, "%d,%s,%s,%.2f,%s\n", customer.account_id, customer.password, customer.customer_name, customer.bank_balance, customer.loan_status);
        }
    }
    fclose(file_read);
    fclose(temp_file);
    rename("../database/temp_db.txt", "../database/customer_db.txt");
}

void to_client(int sock, char* arg){
  send(sock, arg, strlen(arg), 0);
  printf("%s\n", arg);
  memset(arg, 0, sizeof(arg));
}
bool auth_login(int account_id, char* password){
  FILE* file_read = fopen("../database/employee_db.txt", "r");
  if(file_read == NULL) {
    perror("read");
    return false;
  }
  char line[256];
  while (fgets(line, sizeof(line), file_read)) {
    int id;
    char pass[50];
    if (sscanf(line, "%d,%49[^,],", &id, pass) == 2) {
      if (id == account_id && strcmp(pass, password) == 0) {
        fclose(file_read);
        printf("found\n");
        return true;
      }
    }
  }
  fclose(file_read);
  printf("not found\n");
  return false;
}

void emp_logged_in(int ac, int new_socket){
  char emp_buffer[1024] = {0};
  double amt; int ac2;
  strcpy(emp_buffer, "LOGIN_SUCCESS_E");
  send(new_socket, emp_buffer, strlen(emp_buffer),0);
  memset(emp_buffer, 0, sizeof(emp_buffer));
  int cchoice;
  read(new_socket, &cchoice, sizeof(cchoice));
  printf("recieved %d\n", cchoice);
  switch(cchoice) {
    case 1:
      const char* new_customer_name; const char* new_password; double new_bank_balance;
      to_client(new_socket, "GET_NEW_DETAILS");
      read(new_socket, &ac2, sizeof(ac2));
      read(new_socket, new_customer_name, sizeof(new_customer_name));
      read(new_socket, new_password, sizeof(new_password));
      read(new_socket, &new_bank_balance, sizeof(new_bank_balance));
      modify_customer_info(new_customer_name, new_password, new_bank_balance);
    case 2:

      
    default:
      strcpy(emp_buffer, "Invalid choice\n");
      break;
  }
}
void handle_employee(int new_socket) {
    char buffer[1024] = {0};
    int choice;
    read(new_socket, &choice, sizeof(choice));
    switch(choice) {
        case 1:
            strcpy(buffer, "LOGIN_CUST");
            to_client(new_socket, buffer);
            int account_id;
            read(new_socket, &account_id, sizeof(account_id));
            printf("recieved id: %d\n", account_id);
            char password_old[50];
            read(new_socket, password_old, sizeof(password_old));
            printf("recieved pass: %s\n", password_old);
            if(auth_login(account_id, password_old)) {
              emp_logged_in(account_id, new_socket);
            }
            else {
              strcpy(buffer, "LOGIN_FAIL");
              to_client(new_socket, buffer);
            }
            break;
        case 2:
            strcpy(buffer, "Exiting...\n");
            break;
        default:
            strcpy(buffer, "Invalid choice\n");
    }
    send(new_socket, buffer, strlen(buffer), 0);
}
