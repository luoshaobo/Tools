#ifndef __SMS_CERTIFICATE__H__
#define __SMS_CERTIFICATE__H__
#include <iostream>
#include <string>
#include <memory.h>
#include <string.h>
#include <memory>
#include <typeinfo>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstring>
#include <mutex>

#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#define k_service_id_len 3
#define k_protocol_id_len 3
#define k_sig_len 64

namespace fsm
{
  typedef unsigned char uint8;

  uint8 ascii_to_hex (char *data);

  int asc_str_2_hex_str(unsigned char *asc_str, unsigned char *hex_str);

  void hex_to_ascii(uint8 hex_val, char *ascii_val);

  int print_hex(unsigned char *in_hex, unsigned int in_len);

  int hex_to_hexstr(const unsigned char *in_hex, unsigned int in_len,
                      std::string &out_hexstr);

  int gen_digest(const unsigned char *message, int message_len, unsigned char *digest, unsigned int *digest_len);

  int write_2_file(char *file_name, char *buf, int buf_len);

  int write_hex_str_2_file(char *file_name, unsigned char *hex_str);

  class t_sms_certificate
  {
  public:
    t_sms_certificate();
    virtual ~t_sms_certificate();
    int set_cert_name(const std::string &cert_name);
    int init();
    int fini();
    int verify(const unsigned char *msg, int msg_len,
              const unsigned char *signature, int sig_len);
    bool is_available();
  private:
    std::string m_cert_name;
    EC_KEY *m_ec_key;;
    static std::recursive_mutex m_mutex;

  };
}
#endif

