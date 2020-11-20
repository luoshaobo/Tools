/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     test_classes.h
 *  \brief    Volvo On Test Classes
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#ifndef VOC_TEST_CLASSES_HH_
#define VOC_TEST_CLASSES_HH_



#include <fstream>
#include <openssl/ssl.h>
#include <random>
#include <string>
#include <vector>

//uncomment the line below to generate signal files
#define SAVE_SIGNAL_FILES 1

namespace fsm
{

// Here follows some certificates and code to load them
// what we are doing here is mocking the FoundationServiceManager
// certificate_store so we can control it

//TODO: these are copied from Iulians ccm unittest in FSM
//      make common ut utilities package somehow?

X509* ut_GetCertFromPEM(const char* pPemData);

X509_REQ* ut_GetCsrFromPEM(const char* pPemData);

bool X509_REQ_eq(X509_REQ* a, X509_REQ* b);

EVP_PKEY* ut_GetKeyFromPEM(const char* pPemData);

X509_STORE* ut_GetCertStore(X509* pCerts[], size_t certCount);

//copy of test/certs/ca/certs
const std::string ROOT_CERT_PEM =
"-----BEGIN CERTIFICATE-----\n"
"MIIFRjCCAy6gAwIBAgIJAKbVUQrMg5dbMA0GCSqGSIb3DQEBCwUAMDAxCzAJBgNV\n"
"BAYTAlNFMQ8wDQYDVQQIDAZTd2VkZW4xEDAOBgNVBAMMB1Jvb3QgQ0EwHhcNMTcw\n"
"MjE5MTY0NTIxWhcNMjcwMjE3MTY0NTIxWjAwMQswCQYDVQQGEwJTRTEPMA0GA1UE\n"
"CAwGU3dlZGVuMRAwDgYDVQQDDAdSb290IENBMIICIjANBgkqhkiG9w0BAQEFAAOC\n"
"Ag8AMIICCgKCAgEAy1ClzzxKplsBBsmQ3VFNk48hCGqeZbwSJds5xyPj/DypMrjC\n"
"wvEULN4tJGMac8q726Rri231KiDTxSda+4sHsB9xVTiCMNYKkg42m82rlgvE3Q0I\n"
"rPrJKyqA4+04KjWVLZUTQ4j/Z8zGlI/TLRJHJRF5tnpCrLu4KggJpt1BNTu2u6Tq\n"
"uYBvW7pal3ioVzFnH+rl26+uzsmXZhkvM8RQA8ec0mSpeBQuP6HRIAmTkzSrhw1a\n"
"PHvMitUW9at0aoHIep7+tlD1viJ24ST6YjM/oIFtoJ5oUeAgN5R1W+98WeKPnRcs\n"
"8t4ypD3bP+eyXhDXlSYVuHQpoJZFBm19xGVVRWTsTEeBKwI9k+kIpTtk+0aWX7qZ\n"
"l/+nyiFwFfgOxyKv/0dr5/axvSaj66b/iirnuSgbwUSE3NTtE38xQVkzxMIHcMT+\n"
"MA7xOMIvtdwAsdqHsbtluC/V2NldcZIvb9ocm4zvQ7x1IoN+PeRM6LSuTUrYxN4N\n"
"R5TL1mwiVUH0OlNX7sLkG9neslWpTD9Ns4bDvg6SrLwlDnPU9pG1HrGpPPNEHZ7Z\n"
"25uuVxqqG+P+CB5gwARbw3otw34TRfZiVXbYZs7m0HGlYV+IM8yMxRY6F9pVDLN0\n"
"IuVdSPu5wwDkK2AAcEPw+QJJeviVVRdl4A37fgTNt1ZVTTs5wv1ke/vSWFUCAwEA\n"
"AaNjMGEwHQYDVR0OBBYEFHAjBKV6X0QFcq1WLrbTxYTN+ZXUMB8GA1UdIwQYMBaA\n"
"FHAjBKV6X0QFcq1WLrbTxYTN+ZXUMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/\n"
"BAQDAgGGMA0GCSqGSIb3DQEBCwUAA4ICAQAxJVfTeiK+HPtRh8UncVp279uHjZnA\n"
"P8rouyi2KUDRr2xV5HejaJMS0FhPRolcP7hgOpYJ7hbCWpdOUMPOUO9jdMsqODFo\n"
"H4dwgqfKWH2Llz8vUePOVMnlp59VHZG1M0K2hruoOamSCliO2OgXoUsKZxwDDemN\n"
"t0PIdiPRKbKYxQdUKqNdKFJM7OGUJrXg01nyUSraOwNru9qRgKOOLLsg8TWdCEH8\n"
"acgj+JzGikddOlvzo2QlcJSVhi1kp5xatrOF3BMkft8OkPd0gQWsUeSx8CALk8Mi\n"
"XlBYfcUz6Y0XblEP4/O/VLCUpj1cwSfhtTE5du+MH4BTGmSg90iE+KQBMaKEb7Q8\n"
"DU5Y2UrY3lvCPe7UdbOLT8EYfyDQjam0NQcHJJC5utkpepb3uBhj1CgyKnNUbLtq\n"
"15v/Ky42Fk3LSCujBpM3JaKVyMYhXcTpwahF+h1Rg7a1JHedQfeecNwhcRapQaTD\n"
"T9tgXIfRoOYkycdZ24l1t6YCk1ikw6ASpLOTx0r4xiNcroK9t7MWLiRWPCGLR7K0\n"
"tL0wS8T3SWVMy4R7p9c87NtjWC73YfwnhEfU8vYdYA4uYCK6UgJwl8tXMAjxU4t7\n"
"8M+uk33IHo4umaHrlTv1UK8Aw8OmOQ7uCEPPu3OZzuCkfZvXp/38jzkixNrqetVx\n"
"pz3co3SS2XtL4Q==\n"
"-----END CERTIFICATE-----\n";

//copy of test/certs/ca/private/ca.key.pem
const std::string ROOT_KEY_PEM =
"-----BEGIN RSA PRIVATE KEY-----\n"
"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\n"
"-----END RSA PRIVATE KEY-----\n";


//copy of test/certs/tcam/1
const std::string CLIENT_ONE_CERT_PEM =
"-----BEGIN CERTIFICATE-----\n"
"MIIEoTCCAomgAwIBAgICEAAwDQYJKoZIhvcNAQELBQAwMDELMAkGA1UEBhMCU0Ux\n"
"DzANBgNVBAgMBlN3ZWRlbjEQMA4GA1UEAwwHUm9vdCBDQTAeFw0xNzAyMTkxNzAx\n"
"MzJaFw0xOTAyMTkxNzAxMzJaMC8xCzAJBgNVBAYTAlNFMQ8wDQYDVQQIDAZTd2Vk\n"
"ZW4xDzANBgNVBAMMBlRDQU0gMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
"ggEBAMA+hGISRLNKtulyRbk6QxqNhZo67LaTyE2826qqziIAIQsk+Tej3pKK/cyE\n"
"GLYmWOE0VIVmktBdb7eTHH1tnPD0e4k97ic9nGDDQzqw1140iPsfqe1twIUvlq8x\n"
"3Zzub3kfVNw3aPWM3u/j5L5PdIhaBLEu+NfVC/IEKWdYuJ7q85qQeA/meWsCZSJl\n"
"haAhUITQRuCNWn9RHVi1XFfFYvsRI3jh/qCqEJSlJ/CKXDpSdRhUB1ivipLtTOtp\n"
"dkwyhrjw/YBgA9KEabLhPcsPEUVefarlaqnXCmIHBpa8vDfpSBTydQVrsZzEYDWI\n"
"0vq6yi0+vR0ZiJP7eWsNF5YLN9UCAwEAAaOBxTCBwjAJBgNVHRMEAjAAMBEGCWCG\n"
"SAGG+EIBAQQEAwIFoDAzBglghkgBhvhCAQ0EJhYkT3BlblNTTCBHZW5lcmF0ZWQg\n"
"Q2xpZW50IENlcnRpZmljYXRlMB0GA1UdDgQWBBRGu/+3iRaqPq2Nd/QwqgClqzLz\n"
"fzAfBgNVHSMEGDAWgBRwIwSlel9EBXKtVi6208WEzfmV1DAOBgNVHQ8BAf8EBAMC\n"
"BeAwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMEMA0GCSqGSIb3DQEBCwUA\n"
"A4ICAQDDNF3EKvMLfxb/wdfLYiKoWnqQBAuvhtz4C77mi9/8rGyp7t/eaG+BBOrh\n"
"OFsyL243CQzK3/dcN2xOGnX4fNOVh3c51YfufmcEEuOauPNKN5xhx3n0V0FWHmmS\n"
"SbOxmrbefJxnFatn6JVohDWrtesEOToGdRlDq90Z5okJ2nVk4GN3LYSwIhsT7Ak9\n"
"zMxX+fxZGTo0klX8y4KEZDBPM/XnXyXOj4X/jr1E9xApjc7SW99n17d/5WKnIh2c\n"
"z3bmDv+jIgdPuNXljHgO1P7X+GuOXmuFM00KlTlY2bIr75Qs74GQgc8HnnBzxHMT\n"
"O2Zm2+a9NHjexSf3h2sGYP1b90gHTQkqnjP/ossIgPC/ZGy4zpS47k7f8nr704Wp\n"
"QmkfJf7x6Jhn6rn4htguzwKbPsWQauriIc4mg6L8zFpM7TLJNyRPbIix0yX93sXR\n"
"Bseg5cfjxLxtwlaOLA8tc5AmSZT+ktwymv9Rl0Y74MlRAwKTVihWKTRKyLNFyOGB\n"
"GZV/5X3gphTs4TvCOpOEJsPh1ugbUjJnnKga1LMl8M/z6hdZXWXZtM3D72H25dJw\n"
"DiSt4XBu4BSpKZtGssUBLk7VhbRiKs1L/dhFiNs0VUKx0lJr/ipBSQVAOkNUgUoC\n"
"/A17AwuhFrsfTPZgg2aeRA1iH749T43ndHgz6DsS047OTyivGQ==\n"
"-----END CERTIFICATE-----\n";

//copy of test/certs/tcam/1
const std::string CLIENT_ONE_KEY_PEM =
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEpAIBAAKCAQEAwD6EYhJEs0q26XJFuTpDGo2FmjrstpPITbzbqqrOIgAhCyT5\n"
"N6Pekor9zIQYtiZY4TRUhWaS0F1vt5McfW2c8PR7iT3uJz2cYMNDOrDXXjSI+x+p\n"
"7W3AhS+WrzHdnO5veR9U3Ddo9Yze7+Pkvk90iFoEsS7419UL8gQpZ1i4nurzmpB4\n"
"D+Z5awJlImWFoCFQhNBG4I1af1EdWLVcV8Vi+xEjeOH+oKoQlKUn8IpcOlJ1GFQH\n"
"WK+Kku1M62l2TDKGuPD9gGAD0oRpsuE9yw8RRV59quVqqdcKYgcGlry8N+lIFPJ1\n"
"BWuxnMRgNYjS+rrKLT69HRmIk/t5aw0Xlgs31QIDAQABAoIBABLxliX72t04DZIv\n"
"eoNoJFinfXTXPaA4K9LNO3Az6KAdFyR7/jrnYPVb0OqoUa8RVEzM1U9X7juuGWYj\n"
"jvklJgPwc4PwoFkpjxFR7HwKFQ7pal4knqAQ4pm9OMIGzem67S0oUzpoE2Wu/gDx\n"
"mgbO2ZLeG/xBvq+2BLlSg6n1aivumu7f9lvT5m+H2w9rku4Uur2O9Yt6kRNeENFp\n"
"vaFaA/JwP3f4zAMYYTurTaS8gdko+23hfnm85gOOII1iWyNIlVFDtjfM3UwxrIuQ\n"
"bh5QkLf+u49M5J2ht8ITYYJhUPhdBwKKicYNyqSV9hXfTV3h4F+wCSeWZDLd/v8x\n"
"NGsJ57kCgYEA/2HEJu62N6IoXkwpKLn+zri1t7U8E8Ennkp0w5Zxz3XTbpmmCBr7\n"
"+TDsLGE28DhX+GYgvoM/uHRDpZZwlhRZ/DS5nY2im0q1F19/vSZjhHAMpzAMkJyq\n"
"aAOG3jSuppD4c30rDvCJZTm2ZWKrXdxkt3p21X8HijfAaZynus4/oQMCgYEAwLWh\n"
"iQgLxuu5VtxQuW8lkQIG7tg53cbWezEI5lQXMQjrfssWESpAv8gSXDPUOy76OdPb\n"
"vlwAmUHHYUQ1MDo8QN63Cd4pvhzSES++q74r8flqHZgVgWTYswbFJiwwrVSRPeVV\n"
"ibwbHZiLsZL9Cqz+YKY8+FxdKTOhf7RiNORnMEcCgYEAheWDi4oS/Z5GnhI+bjz+\n"
"jPIpTHV4Vz8pf4O4LqgdadF0FoP2mj9TpzZwTWB3BoH0FaeuOVTlPteudA4pX3ay\n"
"K1qVizasb9E+m0+V0N1kxhZde/bkT5nken6G4tAqVeiiayLQRZf14Z3dtNCD2sVo\n"
"BQ7vaUad9GkAuB7LjlT+5XMCgYB+7aMt/Ky1IsH6ojZRTRHzRuCGJMlIeBUBIzT3\n"
"cF+uHm/uHZm1SJ1JNGBTdpfNO4341MAhy7thNmA6gpB96X2SRJ9O0pOv8o+yXj46\n"
"JJIRWPQ59dgEWAYbNNRZ5jVRlSftGsaSZ+F8RuOL3ILqqzMJObVi6IM8BhPTZ15f\n"
"9Ci7NQKBgQDekhuAY56+y7BjLwmJkqFo6pbe/BUn/YYNEZp2d3aA1NjxGAS2S6qc\n"
"MdlRc9G0rnCfzT+ApIbFIExPfteH/HBuwYM5U2cBSdav3nPmDxJov/JJX4CxXI9q\n"
"tZ+st1M/wnNMZKYln+GI4De21ZpOyDOSt+EATC9m4grsLUVe6froWw==\n"
"-----END RSA PRIVATE KEY-----\n";

//copy of test/certs/clients/1
const std::string CLIENT_TWO_CERT_PEM =
"-----BEGIN CERTIFICATE-----\n"
"MIIEojCCAoqgAwIBAgICEAEwDQYJKoZIhvcNAQELBQAwMDELMAkGA1UEBhMCU0Ux\n"
"DzANBgNVBAgMBlN3ZWRlbjEQMA4GA1UEAwwHUm9vdCBDQTAeFw0xNzAyMTkxNzA1\n"
"MDNaFw0xOTAyMTkxNzA1MDNaMDAxCzAJBgNVBAYTAlNFMQ8wDQYDVQQIDAZTd2Vk\n"
"ZW4xEDAOBgNVBAMMB0NsaWVudDEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQDCphYhOkTTpHwoDnM4rU5V7tNS6ltb48aEFgjS4sEsNIBn9AxjSkq8jH8Y\n"
"umgX+pMzOkCWpDqBbgjXlsJNwfbLoIIF0d/sJd5Qdbq9+UhjjXIkXJoZA8+9GJp+\n"
"KpZzmf2HYp/raLeTjGxkn6Ko4XklLl0yr8c5qrrApHcSm+E18jTlhqnA9gGorGvY\n"
"I+CBUObKovSh+L7UbTr9c8CjWzIUITYEPLCYB8nyPzKF1goJZ6l8Z5vHyEMKSRO8\n"
"vEdV60c1DvkxJUjLl3vTnfxTy73VFasSdVJ76NntF1HeT11d1RkcpolCNETNYeAt\n"
"JPxwrvpYM7EXtBd+iRfHk0dtyU7xAgMBAAGjgcUwgcIwCQYDVR0TBAIwADARBglg\n"
"hkgBhvhCAQEEBAMCBaAwMwYJYIZIAYb4QgENBCYWJE9wZW5TU0wgR2VuZXJhdGVk\n"
"IENsaWVudCBDZXJ0aWZpY2F0ZTAdBgNVHQ4EFgQUoawV97l9LVcemUbkuoyjTAwQ\n"
"hYAwHwYDVR0jBBgwFoAUcCMEpXpfRAVyrVYuttPFhM35ldQwDgYDVR0PAQH/BAQD\n"
"AgXgMB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcDBDANBgkqhkiG9w0BAQsF\n"
"AAOCAgEATwmosCjh1jSp2J9aPFclvmHW70/htaBkjRMfiUmktOfrk/be+egaPKjl\n"
"V9EiHZolNP8QYMFZ7iVGz++xIR+EKz56THWoneqJFnTgdMVkzsApW/iAn5UCZGcZ\n"
"TTKzL6L80Ea140NeiLT+z0HtNW0WYRlW9A9Ia+HuP1viQ8zxKt5pFRazh5vIDSmQ\n"
"9QL1d0BVK1xpM9fWUfVlMDoRgOKCkkkjKwjGKDYpJ+Bbm6yDzKycq8/koP5pYd5/\n"
"NVIWMSg5kSa35x7Zf/DlzmP7rnXJltnxJtZTiMkqu5iT7Ft3my3xvOddxHl5hXNb\n"
"MxXRYL1e7XrVsbZ+6ChmOb2rULR1/ypzGfVwzHnaG8tNQV8tZDdN6gKGR9v5s5pK\n"
"k8W5KCBM2gGkRZDUw1LojYbJ6TXwNHQXd2jFktFMH7uiNYZymFrx4t+idXcmM4jR\n"
"dvMOEibng3H4FeCo3iMgXKOxy8rONjl0b1rVzXYRGpBYjRQMzABOz4/dFjEyCgZu\n"
"cs+w0w0+aFCN9zRdE5GiP/z5YmtFw+TbZ9rhViUlghAn1BDyLHFV2oqHaMa2NMe9\n"
"5Ycbd1FcJHdIHD7xJi5C23L7fbaKE6LPgqy82YBoEfQH+8iVFJHYRwozlIfHRcLC\n"
"Jv/7iXzOxNqiZgIAj/xbw3GApqTz6NwyVEDG+ER26Z5Rqp4H57o=\n"
"-----END CERTIFICATE-----\n";

//copy of test/certs/client/1
const std::string CLIENT_TWO_KEY_PEM =
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEowIBAAKCAQEAwqYWITpE06R8KA5zOK1OVe7TUupbW+PGhBYI0uLBLDSAZ/QM\n"
"Y0pKvIx/GLpoF/qTMzpAlqQ6gW4I15bCTcH2y6CCBdHf7CXeUHW6vflIY41yJFya\n"
"GQPPvRiafiqWc5n9h2Kf62i3k4xsZJ+iqOF5JS5dMq/HOaq6wKR3EpvhNfI05Yap\n"
"wPYBqKxr2CPggVDmyqL0ofi+1G06/XPAo1syFCE2BDywmAfJ8j8yhdYKCWepfGeb\n"
"x8hDCkkTvLxHVetHNQ75MSVIy5d70538U8u91RWrEnVSe+jZ7RdR3k9dXdUZHKaJ\n"
"QjREzWHgLST8cK76WDOxF7QXfokXx5NHbclO8QIDAQABAoIBAHJV3OnnsEYheEyP\n"
"MF86balMgBVUVBXF1fYAe25kg9PTgxmM4TnxibT3UjDUThDHv2936DiUf0lHuSnq\n"
"BHhoVu+FeNqyTTYJiGz6upact0jC7Skuu9W1x3Dr4tP5cCa3bZT4+bbt6+dftMvm\n"
"eUV6LA8LDPOZDQ1ynABMmyTzK3bjqI9rUmGJjJbEOoTU3YE7haC5aGMOIA/VXS2n\n"
"eH3bDjEi4C3jpPErrmexj6F/q1r4yaIoquhAT81UL6orGQ0KABPByJ/UnKVThNt4\n"
"9yoEf9S7YL5RQpv12uP9SBrj1IPCVqJ8jQn2yXiceV1pqEl6aKN3e9HlbU2KdKYX\n"
"1YNAI4ECgYEA5jYIVrx8zyizbgGXyJKVxw7YUsioCXcncgMjLFKwPZZktdtizFLn\n"
"boXls3fhRIbZCvipUsUDOW9PF50rZ8KPE5UhSF2Npd9HhMYKRfwDkU6IVqq86B7s\n"
"yB7phGOsIJFGgNngAYO+lqBCEntJt2p7nSChV86iD4ZHt6Y0J+alJ3MCgYEA2HQz\n"
"1PQdYs2+/qEIJCDJH4u0cLceCn+amzSXZr19+U30kKWG/JqP3irOmEBJznu4TDGx\n"
"biN3avpI9EK2hIyHvRv6she1BsRpOH8DvBGWGaGcFT6+J6siAk/TtQ8X9oYuO6kB\n"
"X3swZATdgLOJ8Q2heYil7mWT2GOP4nevl14jrwsCgYAkgPzhhBHmZ3XMQ8RlsZi+\n"
"b8QtnFPI7HP8yDKTnKkFmwttKtSHwb2g8OpzgtbdnuRbsQJodYUFXnkULX/sfBVj\n"
"7YnRh8QmQTl9/Ek0v7lxGRrVY02BISY6tWwh6gwpPeAIHoQ+uT1HdTIE7DfMNb5U\n"
"NBBhdAqOm6RToq8yhIg7sQKBgEt4wPXYpEJmINlhxpfqQpZYPPMsNjJM6diCDuRI\n"
"4BrEPqkMvjNnpRA+9sgJanKvmsQec5JGlYXcfQGGHWO8KDwc/F2KfJk+dL25M9Eo\n"
"hKCtQ+H4fY8RUYti0BuKhr1zbb1poUvshSPqulz8vPUq3omkxs6H5nQFyw6Y0sXB\n"
"kkmfAoGBANzuRnZtYvqUqBq1DbElJ+3Ma0GjE8ha17f/MpYTRjtkQrOsWJvvDiMq\n"
"cKB5P2QGqFJXY5qTz5n2xJ9FdCwO8hyhP1NRxHYLcdLdQTKcU5aGEmcJF0JDdcMI\n"
"nqZGvtCLCLsK66KfqV3d5FP3+Ohtr5MdlvoBqEAlz6vlhSvYXA6F\n"
"-----END RSA PRIVATE KEY-----\n";

//copy of test/certs/ca/csr/client1.csr.pem
const std::string CLIENT_TWO_CSR_PEM =
"-----BEGIN CERTIFICATE REQUEST-----\n"
"MIICdTCCAV0CAQAwMDELMAkGA1UEBhMCU0UxDzANBgNVBAgMBlN3ZWRlbjEQMA4G\n"
"A1UEAwwHQ2xpZW50MTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMKm\n"
"FiE6RNOkfCgOczitTlXu01LqW1vjxoQWCNLiwSw0gGf0DGNKSryMfxi6aBf6kzM6\n"
"QJakOoFuCNeWwk3B9sugggXR3+wl3lB1ur35SGONciRcmhkDz70Ymn4qlnOZ/Ydi\n"
"n+tot5OMbGSfoqjheSUuXTKvxzmqusCkdxKb4TXyNOWGqcD2Aaisa9gj4IFQ5sqi\n"
"9KH4vtRtOv1zwKNbMhQhNgQ8sJgHyfI/MoXWCglnqXxnm8fIQwpJE7y8R1XrRzUO\n"
"+TElSMuXe9Od/FPLvdUVqxJ1Unvo2e0XUd5PXV3VGRymiUI0RM1h4C0k/HCu+lgz\n"
"sRe0F36JF8eTR23JTvECAwEAAaAAMA0GCSqGSIb3DQEBCwUAA4IBAQAN8tPWqy74\n"
"CLZIaC4F17Wnzsn0bIvhE4uMMdhJvubpez7l2IBTtiGfVps3HtHESo5JwvBfvSrE\n"
"1ENc2o8uVYLuygp0gKsfsFA5uRaA5tZ7rhU5xxouGrsF5ycDIR3izjyRuMOPbGlO\n"
"KZvTzxFu840PNSP/mfk1+UUMGUkq5M+3fDW/6FoV6u27ybe7QxgTCqg5Hmvbkmgg\n"
"nEZJFhKQOCn/q9DBS8kbytgvznVvP8/uPiTTG0JTELh8tSzpx9hq8qnSIFB1k5DV\n"
"QIJ2d+RBJD0FG/wCqdAVFfy/978z1EX9/hbIjFnmClwnYlSW/2QPP6dFKWBwnVji\n"
"VFnBR2Wpy++2\n"
    "-----END CERTIFICATE REQUEST-----\n";

static const std::string* volatile user_cert = &CLIENT_ONE_CERT_PEM;
static const std::string* volatile user_key = &CLIENT_ONE_KEY_PEM;

void SetCertsForEncode();

void SetCertsForDecode();

void SetDefaultCerts();

X509_STORE* GetCarRoot();

X509* GetCarCert();

EVP_PKEY* GetCarKey();
X509* GetMClientCert();

} // namespace

#include "signals/basic_car_control_signal.h"
#include "signals/bcc_cl_001_signal.h"
#include "signals/bcc_cl_002_signal.h"
#include "signals/bcc_cl_010_signal.h"
#include "signals/bcc_cl_011_signal.h"
#include "signals/bcc_cl_020_signal.h"
#include "signals/ca_cat_001_signal.h"
#include "signals/ca_del_001_signal.h"
#include "signals/ca_cat_002_signal.h"
#include "signals/ca_del_002_signal.h"
#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm.h"

namespace voc_test_classes
{

static const int client_user_id = 1;
static const std::vector<int> users = {client_user_id};

void DumpMessageFile(std::string name, const char* data, size_t data_size);
void DumpMessageFile(std::string name, const std::vector<unsigned char>& data);
void DumpPubKey(std::string name, EVP_PKEY* key);
bool MakeKeysAndSecret(EVP_PKEY** tcam_key,
                       EVP_PKEY** peer_key,
                       std::vector<unsigned char>& secret);

//returns true if time stamps are identical, false otherwise
bool TimeEqual(struct tm *tm1, struct tm *tm2);

using namespace volvo_on_call;

//Encode class for BCC CL 001, used for BCC CL 001 testing
class TestBccCl001Signal: public BccCl001Signal, public fsm::CCMEncodable {
public:

    //Encoding of BCC CL 001
    void* GetPackedPayload();

    //setting of the Advert type
    void SetAdvert(Advert advert);


    /*
     * BELOW IS THE IMPLEMENTATION OF MANDATORY METHODS
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

    const char* GetOid ();

    fs_VersionInfo GetContentVersion ();

    //constructor
    TestBccCl001Signal (fsm::CCMTransactionId& transaction_id,
                        uint16_t sequence_number);

    //Factory function for the test signal
    static std::shared_ptr<TestBccCl001Signal> CreateTestBccCl001Signal(
                                                              fsm::CCMTransactionId& transaction_id,
                                                              uint16_t sequence_number);

};//TestBccCl001Signal


//Decode class for BCC CL 002, used for BCC CL 002 testing
class TestBccCl002Signal: public BccCl002Signal, public fsm::CCMDecodable
{
public:
    bool valid = false;

    static std::shared_ptr<TestBccCl002Signal> CreateTestBccCl002Signal(ccm_Message* ccm,
                                                                        fsm::TransactionId& transaction_id);

    TestBccCl002Signal (ccm_Message* ccm, fsm::TransactionId& transaction_id);

    bool UnpackPayload(BCC_CL_002_t* asn1c_bcc_CL_002);
};

//Encode class for BCC CL 001, used for BCC CL 001 testing
class TestBccCl010Signal: public BccCl010Signal, public fsm::CCMEncodable {
public:

    //Encoding of BCC CL 010
    void* GetPackedPayload();

    /*
     * BELOW IS THE IMPLEMENTATION OF MANDATORY METHODS
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

    const char* GetOid ();

    fs_VersionInfo GetContentVersion ();

    fs_Encoding GetPreferedEncoding ();

    //constructor
    TestBccCl010Signal (fsm::CCMTransactionId& transaction_id,
                        uint16_t sequence_number);

    //Factory function for the test signal
    static std::shared_ptr<TestBccCl010Signal> CreateTestBccCl010Signal(
                                                              fsm::CCMTransactionId& transaction_id,
                                                              uint16_t sequence_number);

};//TestBccCl010Signal

//Decode class for BCC-CL-011, used for BCC-CL-011 testing
class TestBccCl011Signal: public BccCl011Signal, public fsm::CCMDecodable
{
public:
    bool valid = false;

    static std::shared_ptr<TestBccCl011Signal> CreateTestBccCl011Signal(ccm_Message* ccm,
                                                                        fsm::TransactionId& transaction_id);

    TestBccCl011Signal (ccm_Message* ccm, fsm::TransactionId& transaction_id);

    bool UnpackPayload(BCC_CL_011_t* asn1c_bcc_cl_011);

    bool CompareLocationData(std::shared_ptr<BccCl011Signal> signal_a);
};

//Decode class for BCC-CL-020, used for BCC-CL-020 testing
class TestBccCl020Signal: public BccCl020Signal, public fsm::CCMDecodable
{
public:
    bool valid = false;

    static std::shared_ptr<TestBccCl020Signal> CreateTestBccCl020Signal(ccm_Message* ccm,
                                                                        fsm::TransactionId& transaction_id);

    TestBccCl020Signal (ccm_Message* ccm, fsm::TransactionId& transaction_id);

    bool UnpackPayload(BCC_CL_020_t* asn1c_bcc_cl_020);

    bool CompareLocationData(std::shared_ptr<BccCl020Signal> signal_a);

    ccm_storeAndForwardOptions GetStoreAndForwardOptions();

};

//Decode class for CA-CAT-001, used for CA-CAT-001 testing
class TestCaCat001Signal: public CaCat001Signal, public fsm::CCMDecodable
{
public:
    bool valid = false;

    static std::shared_ptr<TestCaCat001Signal> CreateTestCaCat001Signal(ccm_Message* ccm,
                                                                        fsm::TransactionId& transaction_id);

    TestCaCat001Signal (ccm_Message* ccm, fsm::TransactionId& transaction_id);

    bool UnpackPayload(CA_CAT_001_t* asn1c_ca_cat_001);

    /**
     * \brief List of decoded certs stored as X509*
     */
    std::vector<X509 *> received_certs_;

    ~TestCaCat001Signal();

};//TestCaCat001Signal

//Encode class for CA CAT 002, used for CA CAT 002 testing
class TestCaCat002Signal: public CaCat002Signal, public fsm::CCMEncodable {

public:

    void* GetPackedPayload();

    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

    const char* GetOid ();

    fs_VersionInfo GetContentVersion ();

    //constructor
    TestCaCat002Signal (fsm::CCMTransactionId& transaction_id,
                        uint16_t sequence_number);

     //destructor
     ~TestCaCat002Signal ();

    //Factory function for the test signal
    static std::shared_ptr<TestCaCat002Signal> CreateTestCaCat002Signal(
                                                              fsm::CCMTransactionId& transaction_id,
                                                              uint16_t sequence_number);
};//TestCaCat002Signal

//Decode class for CA-DEL-001, used for CA-DEL-001 testing
class TestCaDel001Signal: public CaDel001Signal, public fsm::CCMDecodable
{
public:
    bool valid = false;

    static std::shared_ptr<TestCaDel001Signal> CreateTestCaDel001Signal(ccm_Message* ccm,
                                                                        fsm::TransactionId& transaction_id);

    TestCaDel001Signal (ccm_Message* ccm, fsm::TransactionId& transaction_id);

};//TestCaDel001Signal

//Encode class for CA DEL 002, used for CA DEL 002 testing
class TestCaDel002Signal: public CaDel002Signal, public fsm::CCMEncodable {

public:

    void* GetPackedPayload();

    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

    const char* GetOid ();

    fs_VersionInfo GetContentVersion ();

    //constructor
    TestCaDel002Signal (fsm::CCMTransactionId& transaction_id,
                        uint16_t sequence_number);

     //destructor
     ~TestCaDel002Signal ();

    //Factory function for the test signal
    static std::shared_ptr<TestCaDel002Signal> CreateTestCaDel002Signal(
                                                              fsm::CCMTransactionId& transaction_id,
                                                              uint16_t sequence_number);
};//TestCaDel002Signal

std::shared_ptr<fsm::Signal> CreateTestSignalFromCCM (char* binary_ccm, size_t num_bytes);


} // namespace voc_test_classes

/** \}    end of addtogroup */
#endif
