#include "display.h"
#include "uart_rasp.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char* msgg = "";
char* solicitacao = "";
pthread_t id_thread;

/**
 * Recebe a mensagem e escreve no display continuamente
 * @param uart_filestream - arquivo UART
*/
void * receiveMsg(void * uart){
    int uart_filestream = *((int *) uart);
    char texto[] = "";
    while(strcmp(msgg, "") != 0 || strcmp(solicitacao, "") != 0){ // enquanto a mensagem de finalizacao n$
        if(strcmp(msgg, "") != 0){
            uart_send(solicitacao, uart_filestream);
            char* result = uart_receive(uart_filestream);
            if(strcmp(result, "") != 0){
                sprintf(texto, "%s%s", msgg, result); // concatena o conte$
                write_textLCD(texto);
            }
        }
    }
}


/**
 * Para a thread de recebimento de mensagens
*/
void encerrarThread(){
    msgg = "";
    solicitacao = "";
    pthread_join(id_thread, NULL);
}

int main() {
    initDisplay();  // inicializa o display lcd
    write_textLCD("Problema 2 - SD");

    int uart_filestream = uart_config();
    if(uart_filestream == -1){
        printf("\nFalha na abertura do arquivo UART!\n");
        return 0;
    }    

    char sensor[] = "0";
    char opcao = '/';
    do{
        printf("========================================\n");
        printf("            Escolha uma opcao           \n");
        printf("----------------------------------------\n");
        printf("| 1 | Situação atual do NodeMCU\n"); // 0x03
        printf("| 2 | Valor da entrada analógica\n"); // 0x04
        printf("| 3 | Valor das entradas digitais\n"); // 0x05
        printf("| 4 | Acender/Apagar LED\n"); // 0x06
        printf("| 0 | Sair\n");
        printf("========================================\n");
        printf("=>  ");
        scanf("%s", &opcao);
        system("cls || clear");

        switch(opcao){
            case '1':
                encerrarThread();

                uart_send("30", uart_filestream);
                if(strcmp(uart_receive(uart_filestream), "00") == 0){
                    write_textLCD("NodeMCU OK!");
                }
                break;
            case '2': // sensor analogico
                encerrarThread();

                msgg = "Sensor A: ";
                solicitacao = "40";
                //cria uma thread que recebe e envia as mensagens
                pthread_create(&id_thread, NULL, receiveMsg, &uart_filestream);
                break;
            case '3': // sensor digital
                encerrarThread();

                printf("\nQual sensor digital deseja selecionar? [1-8] \n =>  ");
                scanf("%s", &sensor);

                if(sensor[0] >= '1' && sensor[0] <= '8' && strlen(sensor) == 1){
                    char texto[] = "";
                    char solicicatacao_sensor[] = "";
                    sprintf(texto, "Sensor D%s: ", sensor);
                    sprintf(solicicatacao_sensor, "5%s: ", sensor);

                    msgg = texto;
                    solicitacao = solicicatacao_sensor;
                    //cria uma thread que recebe e envia as mensagens
                    pthread_create(&id_thread, NULL, receiveMsg, &uart_filestream);
                }else{
                    printf("\nOpção inválida!\n\n");
                }
                break;
            case '4':
                encerrarThread();

                uart_send("60", uart_filestream);
                if(uart_receive(uart_filestream)[0] == '1'){
                    write_textLCD("LED ligado!");
                }else{
                    write_textLCD("LED desligado!");
                }
                break;
            case '0':
                encerrarThread();

                printf("\n\n\tFinalizando...\n");
                break;
            default:
                printf("\n\n\tOpcao invalida!\n\n");
        }
    } while(opcao != '0');

    return 0;
}