/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     keystore_stub.h
 *  \brief    Keystore stub version
 *  \author   Imran Siddique
 *
 *  \addtogroup unittest
 *  \{
 */

#ifndef KEYSTORE_USER_MANAGER_STUB_H
#define KEYSTORE_USER_MANAGER_STUB_H

#include "keystore.h"

#include <string>
#include <vector>
#include <map>

namespace fsm {

/**
 * \brief Reads keys and certificates from the string and store it globally.
 */
void GenerateCerts();

/**
 * \brief Free all stored certificates and keys.
 */
void FreeCerts();

// Certificate Id for client certificate
static const int client_cert_id = 1;
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


// List of users in usermanager
static const int client_user_id = 1;
static const std::vector<int> users = {client_user_id};

// List of certificates ids
static const std::vector<int> cert_ids = {client_cert_id};

// Pair user with associated certificate
static std::map<int, int> users_certs_pair = {{client_user_id, client_cert_id}};

} // namespace fsm

/** \}    end of addtogroup */

#endif // KEYSTORE_USER_MANAGER_STUB_H
