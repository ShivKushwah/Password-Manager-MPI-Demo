/*
 * Copyright (C) 2011-2019 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


// App.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <map>
#include "../Enclave1/Enclave1_u.h"
#include "../Enclave2/Enclave2_u.h"
#include "sgx_eid.h"
#include "sgx_urts.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


#define UNUSED(val) (void)(val)
#define TCHAR   char
#define _TCHAR  char
#define _T(str) str
#define scanf_s scanf
#define _tmain  main

extern std::map<sgx_enclave_id_t, uint32_t>g_enclave_id_map;


sgx_enclave_id_t e1_enclave_id = 0;
sgx_enclave_id_t e2_enclave_id = 0;

#define ENCLAVE1_PATH "libenclave1.so"
#define ENCLAVE2_PATH "libenclave2.so"

void ocall_print(const char* str) {
    printf("%s\n", str);
}

void waitForKeyPress()
{
    char ch;
    int temp;
    printf("\n\nHit a key....\n");
    temp = scanf_s("%c", &ch);
    (void) temp;
}

uint32_t load_enclaves()
{
    uint32_t enclave_temp_no;
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;

    enclave_temp_no = 0;

    ret = sgx_create_enclave(ENCLAVE1_PATH, SGX_DEBUG_FLAG, NULL, NULL, &e1_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(e1_enclave_id, enclave_temp_no));

    ret = sgx_create_enclave(ENCLAVE2_PATH, SGX_DEBUG_FLAG, NULL, NULL, &e2_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(e2_enclave_id, enclave_temp_no));


    return SGX_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
    uint32_t ret_status;
    sgx_status_t status;

    UNUSED(argc);
    UNUSED(argv);

    if(load_enclaves() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    printf("\nAvailable Enclaves");
    printf("\nEnclave1 - EnclaveID %" PRIx64, e1_enclave_id);
    printf("\nEnclave2 - EnclaveID %" PRIx64, e2_enclave_id);
    
    do
    {
        printf("\n");
        printf("\n");

        printf("ENCLAVE1 Sending Ping---------\n");
        //Test Create session between Enclave1(Source) and Enclave2(Destination)
        status = Enclave1_test_create_session(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave1_test_create_session Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nSecure Channel Establishment between Source (E1) and Destination (E2) Enclaves successful !!!");
            }
            else
            {
                printf("\nSession establishment and key exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                break;
            }
        }

        //Test Enclave to Enclave call between Enclave1(Source) and Enclave2(Destination)
        status = Enclave1_test_enclave_to_enclave_call(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave1_test_enclave_to_enclave_call Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nEnclave to Enclave Call between Source (E1) and Destination (E2) Enclaves successful !!!");
            }
            else
            {
                printf("\n\nEnclave to Enclave Call failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                break;
            }
        }
        //Test message exchange between Enclave1(Source) and Enclave2(Destination)
        status = Enclave1_test_message_exchange(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id, "ping", 4);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave1_test_message_exchange Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nMessage Exchange between Source (E1) and Destination (E2) Enclaves successful !!!");
            }
            else
            {
                printf("\n\nMessage Exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                break;
            }
        }

        
    

        //Test Closing Session between Enclave1(Source) and Enclave2(Destination)
        status = Enclave1_test_close_session(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave1_test_close_session Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nClose Session between Source (E1) and Destination (E2) Enclaves successful !!!");
            }
            else
            {
                printf("\n\nClose session failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                break;
            }
        }
        printf("\n");
        printf("\n");
        printf("ENCLAVE2 Sending Pong---------\n");
        //Test Create session between Enclave2(Source) and Enclave1(Destination)
        status = Enclave2_test_create_session(e2_enclave_id, &ret_status, e2_enclave_id, e1_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave2_test_create_session Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nSecure Channel Establishment between Source (E2) and Destination (E1) Enclaves successful !!!");
            }
            else
            {
                printf("\nSession establishment and key exchange failure between Source (E2) and Destination (E1): Error code is %x", ret_status);
                break;
            }
        }



        status = Enclave2_test_message_exchange(e2_enclave_id, &ret_status, e2_enclave_id, e1_enclave_id, "pong", 4);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave2_test_message_exchange Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nMessage Exchange between Source (E2) and Destination (E1) Enclaves successful !!!");
            }
            else
            {
                printf("\n\nMessage Exchange failure between Source (E2) and Destination (E1): Error code is %x", ret_status);
                break;
            }
        }

        //Test Closing Session between Enclave1(Source) and Enclave2(Destination)
        status = Enclave2_test_close_session(e2_enclave_id, &ret_status, e2_enclave_id, e1_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave2_test_close_session Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nClose Session between Source (E2) and Destination (E1) Enclaves successful !!!");
            }
            else
            {
                printf("\n\nClose session failure between Source (E2) and Destination (E1): Error code is %x", ret_status);
                break;
            }
        }


        printf("Welcome to the Intel-SGX Password Manager\n");
    printf("Type \"help\"\n");

    //TODO: passwords of length 1 don't work (maybe strncpy is deprecated)
    //TODO: multiple passwords dont work

    while (true)
    {

        // std::string command;
        char strCommand[100];

        char str1[100];
        char str2[100];
        char str3[100];
        char str4[100];
        int i;
        void *encrypt = malloc(100);

        printf("Enter a command: ");

        // std::getline (std::cin, command);

        scanf("%[^\n]%*c", strCommand);
        char* split = strtok(strCommand, " ");
        strcpy(str1, split);

        if (strcmp(str1, "help") == 0) {
            printf("Example Usage\n");
            printf("create MasterPassword\n");
            printf("add Website WebsitePassword\n");
            printf("get Website MasterPassword\n");
            printf("quit\n");

        } 
        else if (strcmp(str1, "quit") == 0) 
        {
            break;
        } 
        else if (strcmp(str1, "create") == 0)
        {

            printf("Creating Password Manager\n");
            split = strtok(NULL, " ");
            strcpy(str2, split);


            //str2 = main keystore password
            int create_keystore_return;
            sgx_status_t status = Enclave1_create_keystore(e1_enclave_id, &create_keystore_return, str2);
        }
        else if (strcmp(str1, "add") == 0)
        {
            printf("Adding Password\n");

            int add_password_return;
            split = strtok(NULL, " ");
            strcpy(str2, split);
            split = strtok(NULL, " ");
            strcpy(str3, split);


            //str2 = website
            //str3 = password
            sgx_status_t status2 = Enclave1_add_password(e1_enclave_id, &add_password_return, str2, str3);
            printf("add_password returned: %u\n", add_password_return);
        }
        else if (strcmp(str1, "get") == 0)
        {
            printf("Getting Password\n");

            char get_password_return_str[16];
            int get_password_return;
            split = strtok(NULL, " ");
            strcpy(str2, split);
            split = strtok(NULL, " ");
            strcpy(str3, split);


            //str2 = website
            //str3 = main keystore password
            sgx_status_t status3 = Enclave1_get_password(e1_enclave_id, &get_password_return, str2, get_password_return_str, str3);
            printf("get_password returned: %u\n", get_password_return);
            printf("get_password buffer: %s\n", get_password_return_str);
        }
        else if (strcmp(str1, "encrypt") == 0)
        {
            //Serializes keystone (all data along with masterpassword) and saves to file
            printf("Serializing Keystore");

            int encrypt_return;
            FILE *fp = fopen("encrypt.txt", "w+");

            //TODO: Look at git history for working encrypt example, does this version write to file correctly?

            sgx_status_t status4 = Enclave1_encrypt_and_serialize_key_store(e1_enclave_id, &encrypt_return, encrypt);
            fprintf(fp, "%s", encrypt);
            fclose(fp);
            printf("serialize_key_store returned: %u\n", encrypt_return);
            printf("serialize_key_store string: %s\n", (char *)encrypt);
        }
        else if (strcmp(str1, "decrypt") == 0)
        {
            printf("Decrypting and Setting Keystore");

            //TODO: read from encrypt.txt to set the keystone to all of the old values

            int encrypt_return;
            // size_t nread;

            // FILE *file = fopen("encrypt.txt", "r");
            // if (file) {
            //     while ((nread = fread(encrypt, 1, sizeof encrypt, file)) > 0)
            //         fwrite(encrypt, 1, nread, stdout);
            //     if (ferror(file)) {
            //         /* deal with error */
            //     }
            //     fclose(file);
            // }
            printf("encrypted string %s", encrypt);
            sgx_status_t status5 = Enclave1_decrypt_and_set_key_store(e1_enclave_id, &encrypt_return, encrypt);
        }
    }


       
}while(0);

    sgx_destroy_enclave(e1_enclave_id);
    sgx_destroy_enclave(e2_enclave_id);

    waitForKeyPress();

    return 0;
}
