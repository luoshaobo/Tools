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

#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/crypto/include/internal/evp_int.h>

#include <openssl/ec.h>

#if 0
#include "voc_framework/signal_sources/libcrypto-compat.h"
#endif

#include "dlt/dlt.h"
#include "voc_framework/signal_sources/sms_certificate.h"
//g++ -std=c++11 -g -fpermissive helloworld.cpp -I . -I ./openssl/include/ -L./openssl -lssl -lcrypto -ldl -lpthread


DLT_IMPORT_CONTEXT(dlt_libfsm);
namespace fsm
{
  uint8 ascii_to_hex (char *data)
  {
      uint8 hi_nibble;
      uint8 lo_nibble;

      hi_nibble = (uint8)(data[0] - 0x30);
      if (hi_nibble > 30) /* low case to upper case */
          hi_nibble = (uint8)(hi_nibble - 0x20);
      if (hi_nibble > 9) /* A..F */
          hi_nibble = (uint8)(hi_nibble - 0x07);
      if (hi_nibble > 0xf) /* bigger than F or f */
          hi_nibble = 0;

      lo_nibble = (uint8)(data[1] - 0x30);
      if (lo_nibble > 30) /* low case to upper case */
          lo_nibble = (uint8)(lo_nibble - 0x20);
      if (lo_nibble > 9) /* A..F */
          lo_nibble = (uint8)(lo_nibble - 0x07);
      if (lo_nibble > 0xf) /* bigger than F or f */
          lo_nibble = 0;

      return (uint8)((hi_nibble << 4) + lo_nibble);
  }   /* End of ascii_to_hex() */

  int asc_str_2_hex_str(unsigned char *asc_str, unsigned char *hex_str)
  {
    int asc_str_len = strlen((char *)asc_str);
    int i=0;

    if((asc_str_len % 2) != 0)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "asc_str_2_hex_str error, asc_str_len = %d \n" , asc_str_len);
      return -1;
    }

    for(i = 0; i < (asc_str_len / 2); i++)
    {
        hex_str[i] = (uint8)ascii_to_hex((char *)&asc_str[i*2]);
    }
    return (asc_str_len / 2);
  }

  void hex_to_ascii(uint8 hex_val, char *ascii_val)
  {
      uint8 hex_val_nibble;

      hex_val_nibble = (uint8) (hex_val & (uint8) 0xf0);
      hex_val_nibble >>= 4;
      /* add hex 30 to the integer value */
      ascii_val[0] = (char)(hex_val_nibble + 0x30);
      if (ascii_val[0] > 0x39) /* If the integer value bigger than 9 */
      {                        /* ie if the retchar is more than hex 39 */
          ascii_val[0] += 7;   /* add another 7 to the retchar */
      }
      hex_val_nibble = (uint8) (hex_val & (uint8) 0x0f);
      ascii_val[1] = (char)(hex_val_nibble + 0x30);
      if (ascii_val[1] > 0x39) /* If the integer value bigger than 9 */
      {                        /* ie if the retchar is more than hex 39 */
          ascii_val[1] += 7;   /* add another 7 to the retchar */
      }
      ascii_val[2] = 0; /* Null terminate */
      return;
  }

  int print_hex(unsigned char *in_hex, unsigned int in_len)
  {
    std::string hex_str;
    hex_to_hexstr(in_hex, in_len, hex_str);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s\n", hex_str.c_str());
    return 0;
  }

  int hex_to_hexstr(const unsigned char *in_hex, unsigned int in_len,
                      std::string &out_hexstr)
  {
    out_hexstr.clear();
    unsigned int i = 0;
    for(i=0; i< in_len; i++)
    {
      char asc[3];
      hex_to_ascii(in_hex[i], asc);
      asc[2]=0;
      out_hexstr.append(asc);
    }
    return 0;
  }


  int gen_digest(const unsigned char *message, int message_len, unsigned char *digest, unsigned int *digest_len)
  {
    int ret = -1;
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    do
    {
      EVP_MD_CTX_init(md_ctx);

      if (!EVP_DigestInit(md_ctx, EVP_sha256()))
      {
          DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), EVP_DigestInit fail \n", __FUNCTION__);
          break;
      }

      if (!EVP_DigestUpdate(md_ctx, (const void *)message, message_len))
      {
          DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), EVP_DigestUpdate fail \n", __FUNCTION__);
          break;
      }

      if(!EVP_DigestFinal(md_ctx, digest, digest_len))
      {
          DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), EVP_DigestFinal fail \n", __FUNCTION__);
          break;
      }
      ret = 0;
    }while(0);
    //EVP_MD_CTX_cleanup(md_ctx);
	EVP_MD_CTX_reset(md_ctx);
    EVP_MD_CTX_free(md_ctx);
    return ret;
  }

  int write_2_file(char *file_name, char *buf, int buf_len)
  {
    FILE *file = fopen(file_name, "wb");
    if(file == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error, open file \n", __FUNCTION__);
        return -1;
    }

    fwrite (buf , 1, buf_len, file);
    fclose (file);
    return 0;
  }

  int write_hex_str_2_file(char *file_name, unsigned char *hex_str)
  {

    unsigned char hex[512];
    std::memset(hex, 0, sizeof(hex));
    int hex_len =asc_str_2_hex_str(hex_str, hex);
    if(hex_len <= 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), asc_str_2_hex_str error \n", __FUNCTION__);
        return -1;
    }

    FILE *file = fopen(file_name, "wb");
    if(file == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, " error, open file:%s \n" , file_name);
        return -1;
    }

    fwrite (hex , 1, hex_len, file);
    fclose (file);
    return 0;
  }

  std::recursive_mutex t_sms_certificate::m_mutex;

  t_sms_certificate::t_sms_certificate()
  {
    m_ec_key=NULL;
  }
  t_sms_certificate::~t_sms_certificate()
  {
    this->fini();
  }

  int t_sms_certificate::set_cert_name(const std::string &cert_name)
  {
    m_cert_name = cert_name;
    return 0;
  }

  int t_sms_certificate::init()
  {
    std::unique_lock<std::recursive_mutex> lk(m_mutex);
    int ret = 0;
    this->fini();

    if(m_cert_name.empty())
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!!, cert name is null \n", __FUNCTION__);
      return -1;
    }
    BIO *bio_file = BIO_new_file(m_cert_name.c_str(), "r");
    if (bio_file == NULL) {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), BIO_new_file error:%s \n",
            __FUNCTION__, m_cert_name.c_str());
        return -1;
    }
    std::shared_ptr<BIO*> auto_free_bio(&bio_file, [](BIO **p){
                  if(*p)
                  {
                    BIO_free(*p);
                    *p = NULL;
                  }
              }
          );
    X509 *usrCert = PEM_read_bio_X509(bio_file, NULL, NULL, NULL);
    if (usrCert == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! format conver error\n", __FUNCTION__);
        return -1;
    }
    std::shared_ptr<X509*> auto_free_x509(&usrCert, [](X509 **p)
        {
          if(*p)
          {
            X509_free(*p);
            *p = NULL;
          }
        }
      );
    EVP_PKEY * pubKey = X509_get_pubkey(usrCert);
    if(pubKey == NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), X509_get_pubkey error \n", __FUNCTION__);
        return -1;
    }
    std::shared_ptr<EVP_PKEY*> auto_free_avp_pkey(&pubKey, [](EVP_PKEY **p)
              {
                  if(*p)
                  {
                    EVP_PKEY_free(*p);
                    *p = NULL;
                  }
              }
          );
    if (pubKey->type == EVP_PKEY_EC)
    {
        m_ec_key = EVP_PKEY_get1_EC_KEY(pubKey);
        if (!m_ec_key)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), get key fail \n", __FUNCTION__);
            return -1;
        }
        if (!EC_KEY_check_key(m_ec_key))
        {
            EC_KEY_free(m_ec_key);
            m_ec_key=NULL;
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), EC_KEY_check_key fail \n", __FUNCTION__);
            return -1;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), pubkey is not EVP_PKEY_EC \n", __FUNCTION__);
        return -1;
    }

    return ret;
  }

  int t_sms_certificate::fini()
  {
    int ret =0;
    std::unique_lock<std::recursive_mutex> lk(m_mutex);
    if (m_ec_key)
    {
        EC_KEY_free(m_ec_key);
        m_ec_key = NULL;
    }
    return ret;
  }
#if 0
  int t_sms_certificate::verify(const unsigned char *msg, int msg_len,
              const unsigned char *signature, int sig_len)
  {
	  return 1;
  }
#endif

int t_sms_certificate::verify(const unsigned char *msg, int msg_len,
              const unsigned char *signature, int sig_len)
  {

    std::unique_lock<std::recursive_mutex> lk(m_mutex);
    {
      std::string temp_str;
      hex_to_hexstr(msg, msg_len, temp_str);
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "verified msg is:%s \n", temp_str.c_str());
      hex_to_hexstr(signature, sig_len, temp_str);
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "verified signature is:%s \n", temp_str.c_str());
    }
    if(msg_len <= (k_service_id_len + k_protocol_id_len))
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "error!!! msg_len is invalid,%d \n", msg_len);
      return -1;
    }
    if(sig_len != 66)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "error!!! sig_len is invalid,%d \n", sig_len);
      return -1;
    }

    int ret=0;
    if(m_ec_key==NULL)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!! ec_key is null \n", __FUNCTION__);
      return -1;
    }
    unsigned char digest[512];
    unsigned int dgst_len = 0;
    ret = gen_digest(msg, msg_len, digest, &dgst_len);
    if (ret != 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), gen_digest , error \n", __FUNCTION__);
        return -1;
    }

    BIGNUM *r = BN_new();
    if(r==NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), r is null , error \n", __FUNCTION__);
        return -1;
    }
    //std::shared_ptr<BIGNUM*> auto_free_r(&r , [](BIGNUM **p) {
    //      if(*p)
    //      {
    //        BN_free(*p);
    //        *p=NULL;
    //      }
    //    });
    BIGNUM *s = BN_new();
    if(s==NULL)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), s is null , error \n", __FUNCTION__);
        return -1;
    }
    //std::shared_ptr<BIGNUM*> auto_free_s(&s , [](BIGNUM **p) {
    //      if(*p)
    //      {
    //        BN_free(*p);
    //        *p=NULL;
    //      }
    //    });

    //get signature
    unsigned char r_hex[33];
    std::memset(r_hex, 0, sizeof(r_hex));
    std::memcpy(r_hex, signature + 2, 32);
    std::string temp_str;
    hex_to_hexstr(r_hex, 32,  temp_str);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "r = %s \n", temp_str.c_str());
    ret = BN_hex2bn(&r, (const char *)temp_str.c_str());
    if(ret == 0)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!!, get r failed \n", __FUNCTION__);
      return -1;
    }

    unsigned char s_hex[33];
    std::memset(s_hex, 0, sizeof(s_hex));
    std::memcpy(s_hex, signature + 2 + 32, 32);
    hex_to_hexstr(s_hex, 32,  temp_str);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, " s = %s \n", temp_str.c_str());
    ret = BN_hex2bn(&s, (const char *)temp_str.c_str());
    if(ret == 0)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error!!!, get s failed \n", __FUNCTION__);
      return -1;
    }

    ECDSA_SIG *ecdsa_sig = ECDSA_SIG_new();
    if(ecdsa_sig == NULL)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error, ecdsa_sig is null \n", __FUNCTION__);
      return -1;
    }

    std::shared_ptr<ECDSA_SIG*> auto_free_ecdsa_sig(&ecdsa_sig , [](ECDSA_SIG **p) {
          if(*p)
          {
            ECDSA_SIG_free(*p);
            *p=NULL;
          }
        });

    ret = ECDSA_SIG_set0(ecdsa_sig, r, s);
    if(ret != 1)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), error, ECDSA_SIG_set0, ret = %d \n",
            __FUNCTION__, ret);
      return -1;
    }

    ret = ECDSA_do_verify(digest, dgst_len,
                      ecdsa_sig, m_ec_key);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "ECDSA_do_verify, ret = %d \n", ret);
    if(ret == 1) //verify ok
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), verify ok \n", __FUNCTION__);
    }
    else if(ret == 0)
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), verify error \n", __FUNCTION__);
    }
    else
    {
      DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR, "%s(), verify failed \n", __FUNCTION__);
    }

    return ret;
  }

  bool t_sms_certificate::is_available()
  {
    std::unique_lock<std::recursive_mutex> lk(m_mutex);
    if(m_ec_key== NULL)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
}

