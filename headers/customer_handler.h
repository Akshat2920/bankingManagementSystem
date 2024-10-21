#ifndef SS_PROJECT_HEADERS_CUSTOMER_HANDLER_H_
#define SS_PROJECT_HEADERS_CUSTOMER_HANDLER_H_

#include <stdint.h>

typedef struct {
  int account_id;
  char name[50];
  float balance;
  unsigned long password_hash[65];
} customerRecord;

int login_customer(int connfd, int* account_id);
int auth_customer(int connfd, int* account_id);
void view_account_balance(int connfd, int account_id);
int fetch_account_balance(int account_id, int* balance);
void lock_customer(int fd, int account_id);
void unlock_customer(int fd, int account_id);
unsigned long hash_password(const char* password);
void deposit_money(int connfd);
void withdraw_money(int connfd);
void transfer_funds(int connfd);
void apply_for_loan(int connfd);
void change_password(int connfd);
void add_feedback(int connfd);
void view_transaction_history(int connfd);
void customer_handler(int connfd);

#endif  // SS_PROJECT_HEADERS_CUSTOMER_HANDLER_H_
