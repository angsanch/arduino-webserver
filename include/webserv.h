#ifndef WEBSERV_H_
# define WEBSERV_H_

# include "config.h"
# include <SPI.h>
# include <SdFat.h>
# include <Ethernet.h>

# ifndef BUFF_SIZE
#  define BUFF_SIZE 128
# endif
# ifndef CLIENTS
#  define CLIENTS 3
# endif
# ifndef FORMAT_SIZE
#  define FORMAT_SIZE 5
# endif
# ifndef BASE_PATH
#  define BASE_PATH "/htdocs"
# endif
# ifndef CYCLES_FILE
#  define CYCLES_FILE "/cycles"
# endif
# ifndef CONTENT_TYPE_DIR
#  define CONTENT_TYPE_DIR "/ctype/"
# endif
# ifndef DEFAULT_CONTENT_TYPE
#  define DEFAULT_CONTENT_TYPE "text/plain"
# endif
# ifndef DEFAULT_FILE
#  define DEFAULT_FILE "index.htm"
# endif
# ifndef IP_FILE
#  define IP_FILE "/ip"
# endif

enum http_methods {
  NOT,
  GET,
  POST,
  HEAD,
};

typedef struct server_client_information {
  EthernetClient client;
  File32 file;
  unsigned char method;
  unsigned int code;
  char format[FORMAT_SIZE];
  bool active;
  bool used;
  bool external_header;
  unsigned int id;
} client;

typedef struct server_information {
  SdFat sd;
  File32 header;
  client clients[CLIENTS];
  client lobby;
  char buff[BUFF_SIZE];
  unsigned int send_cycles;
} enviroment;

extern enviroment env;

void null_terminate(void *buf, size_t len);
unsigned int active_clients(void);
int recieve_client(client *c);
void handshake(client *c);
void send_payload(void);
void end_client(client *c);
int send_file(EthernetClient *client, char *path);

void report_entry(client *c);
void report_serve(client *c);
void report_exit(client *c);
void report_post(client *c);

#endif
