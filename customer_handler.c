#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Customer {
    int account_id;
    char password[50];
    char customer_name[50];
    double bank_balance;
    char loan_status[20]; // Values can be "not applied", "in process", "accepted"
};
void to_client(int sock, char* arg){
  send(sock, arg, strlen(arg), 0);
  printf("%s\n", arg);
  memset(arg, 0, sizeof(arg));
}

int create_customer(const char* customer_name, const char* password) {
    struct Customer new_customer;
    FILE* file_read = fopen("database/customer_db.txt", "r");
    int last_customer_id = 0;
    char line[256];
    while(fgets(line, sizeof(line), file_read)) {
        char* token = strtok(line, ",");
        last_customer_id = atoi(token);
    }
    fclose(file_read);
    new_customer.account_id = last_customer_id + 1;
    strcpy(new_customer.customer_name, customer_name);
    strcpy(new_customer.password, password);
    new_customer.bank_balance = 0;
    strcpy(new_customer.loan_status, "not applied");
    FILE* file = fopen("database/customer_db.txt", "a");
    if (file != NULL) {
        fprintf(file, "%d,%s,%s,%.2f,%s\n", new_customer.account_id, new_customer.password, new_customer.customer_name, new_customer.bank_balance, new_customer.loan_status);
        fclose(file);
    }
    return new_customer.account_id;
}

bool auth_login(int account_id, char* password){
  FILE* file_read = fopen("database/customer_db.txt", "r");
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
int get_customer_info(int account_id, char* buffer, size_t buffer_size) {
    FILE* file = fopen("database/customer_db.txt", "r");
    if (file == NULL) {
        return 0;
    }
    char line[256];
    int file_account_id;
    char password[50], customer_name[50], loan_status[20];
    double bank_balance;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,%49[^,],%49[^,],%lf,%19[^\n]", &file_account_id, password, customer_name, &bank_balance, loan_status);
        if (file_account_id == account_id) {

            snprintf(buffer, buffer_size,
                     "Account ID: %d\nCustomer Name: %s\nBank Balance: %.2f\nLoan Status: %s\n",
                     file_account_id, customer_name, bank_balance, loan_status);
            fclose(file);
            return 1;
        }
    }

    fclose(file); // Close the file if customer is not found
    return 0; // Customer not found
}

int withdraw_money(int account_id, double amount) {
    FILE* file = fopen("database/customer_db.txt", "r+");
    if (file == NULL) {
        return 0; // File opening failed
    }
    char line[256];
    int file_account_id;
    char  password[50], customer_name[50], loan_status[20];
    double bank_balance;
    long int position;
    FILE* temp_file = fopen("database/temp_db.txt", "w");
    if (temp_file == NULL) {
        fclose(file);
        return 0; // Temporary file creation failed
    }
    int account_found = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d,%49[^,],%49[^,],%lf,%19[^\n]", &file_account_id, password, customer_name, &bank_balance, loan_status);
        if (file_account_id == account_id) {
            if (bank_balance >= amount) {
                bank_balance -= amount; // Subtract the amount from the balance
                account_found = 1;
            } else {
                fclose(file);
                fclose(temp_file);
                remove("database/temp_db.txt"); // Cleanup
                return -1; // Insufficient funds
            }
        }
        fprintf(temp_file, "%d,%s,%s,%.2f,%s\n", file_account_id, password, customer_name, bank_balance, loan_status);
    }
    fclose(file);
    fclose(temp_file);
    if (!account_found) {
        remove("database/temp_db.txt"); // Cleanup if account not found
        return 0; // Account not found
    }
    remove("database/customer_db.txt");
    rename("database/temp_db.txt", "database/customer_db.txt");
    return 1; // Withdrawal successful
}
void cust_logged_in(int ac, int new_socket){
  char cust_buffer[1024] = {0};
  double amt; int ac2;
  strcpy(cust_buffer, "LOGIN_SUCCESS");
  send(new_socket, cust_buffer, strlen(cust_buffer),0);
  memset(cust_buffer, 0, sizeof(cust_buffer));
  int cchoice;
  read(new_socket, &cchoice, sizeof(cchoice));
  printf("recieved %d\n", cchoice);
  switch(cchoice) {
    case 1:
      get_customer_info(ac, cust_buffer, sizeof(cust_buffer));
      send(new_socket, cust_buffer, strlen(cust_buffer), 0);
      memset(cust_buffer, 0, sizeof(cust_buffer));
      break;
    case 2:
      strcpy(cust_buffer, "GET_AMMOUNT");
      send(new_socket, cust_buffer, strlen(cust_buffer), 0);
      read(new_socket, &amt, sizeof(amt));
      withdraw_money(ac, amt);
      memset(cust_buffer, 0, sizeof(cust_buffer));
      break;
    case 3:
      strcpy(cust_buffer, "GET_AMMOUNT");
      send(new_socket, cust_buffer, strlen(cust_buffer), 0);
      read(new_socket, &amt, sizeof(amt));
      withdraw_money(ac, amt*-1);
      memset(cust_buffer, 0, sizeof(cust_buffer));
      break;
    case 4:
      strcpy(cust_buffer, "TRANSFER");
      send(new_socket, cust_buffer, strlen(cust_buffer), 0);
      read(new_socket, &amt, sizeof(amt));
      read(new_socket, &ac2, sizeof(ac2));
      withdraw_money(ac, amt);
      withdraw_money(ac2, amt*-1);
      memset(cust_buffer, 0, sizeof(cust_buffer));
      break;
    default:
      strcpy(cust_buffer, "Invalid choice\n");
      break;
  }
}
void handle_customer(int new_socket) {
    char buffer[1024] = {0};
    int choice;
    read(new_socket, &choice, sizeof(choice));
    switch(choice) {
        case 1:
            strcpy(buffer, "CREATE_USER");
            send(new_socket, buffer, strlen(buffer), 0);
            char customer_name[1024];
            read(new_socket, customer_name, sizeof(customer_name));
            char password_new[1024];
            read(new_socket, password_new, sizeof(password_new));
            int id = create_customer(customer_name, password_new);
            break;
        case 2:
            strcpy(buffer, "LOGIN_CUST");
            to_client(new_socket, buffer);
            int account_id;
            read(new_socket, &account_id, sizeof(account_id));
            printf("recieved id: %d\n", account_id);
            char password_old[50];
            read(new_socket, password_old, sizeof(password_old));
            printf("recieved pass: %s\n", password_old);
            if(auth_login(account_id, password_old)) {
              cust_logged_in(account_id, new_socket);
            }
            else {
              strcpy(buffer, "LOGIN_FAIL");
              to_client(new_socket, buffer);
            }
            break;
        case 3:
            strcpy(buffer, "Exiting...\n");
            break;
        default:
            strcpy(buffer, "Invalid choice\n");
    }
    send(new_socket, buffer, strlen(buffer), 0);
}

