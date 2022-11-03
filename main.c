#include "display.h"
#include "uart_rasp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    initDisplay();  // inicializa o display lcd
    char text[] = "Testando!!!";
    write_textLCD(text);

    int uart_filestream = uart_config();
    if(uart_filestream == -1){
        printf("\nFalha na abertura do arquivo!\n");
        return 0;
    }

    char opcao = '1';
    char* msg = "";
    do{
        printf("========================================\n");
        printf("            Escolha uma opcao\n");
        printf("----------------------------------------\n");
        printf("| 1 | Situação atual do NodeMCU\n"); // 0x03
        printf("| 2 | Valor da entrada analógica\n"); // 0x04
        printf("| 3 | Valor das entradas digitais\n"); // 0x05
        printf("| 4 | Acender/Apagar LED\n"); // 0x06
        printf("| 0 | Sair\n");
        printf("========================================\n");
        printf("=>  ");
        scanf("%s", &opcao);
        //system("cls || clear"); 
        
        switch(opcao){
            case '1':
                uart_send("0x03", uart_filestream);
                break;
            case '2':
                uart_send("0x04", uart_filestream);
        	sleep(5);
		//msg = uart_receive(uart_filestream);
		//printf(msg);
                //write_textLCD(msg);
                break;
            case '3':
                uart_send("0x05", uart_filestream);

                //msg = uart_receive(uart_filestream);
                //write_textLCD(msg);
                break;
            case '4':
                uart_send("0x06", uart_filestream);
                break;
            case '0':
                printf("\n\n\tFinalizando...\n");
                break;
            default:
                printf("\n\n\tOpcao invalida!\n\n");
        }
    } while(opcao != '0');

    return 0;
}
