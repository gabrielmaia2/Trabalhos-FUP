/******************************************************************************
 *============================================================================*
 * IF61C - 2017-1 - TRABALHO 1                                                *
 *----------------------------------------------------------------------------*
 * Autor: Gabriel Maia Gondim - gondim@alunos.utfpr.edu.br - 1936352          *
 *============================================================================*
 ******************************************************************************/

/*
 * Para compactar, o programa "puxa" os pixels um a um e vai os agrupando.
 * A variável tamanho serve para saber se o somabyte já tem bytes completos para enviar.
 * No descompacta, o programa vai preenchendo os pixels e vai agrupando no somapixel.
 * O tamanho no descompacta serve para saber qual parte do somapixel ele vai enviar.
 * Do mesmo jeito que o compacta, o descompacta pega os pixels completos e já envia.
 * O trabalho tem muitos comentários, mas os que explicam o funcionamento geral
 * tem uma barra de asteriscos vertical à esquerda.
 */

#include "trabalho1.h"

/*
 * Cabeçalhos para gerar valores aleatórios
 */

#include <stdlib.h>
#include <time.h>

void codificaStreamImagem(int n_bits)
{
    /*
     * A variável proxpix serve para pegar sempre o byte do próximo pixel
     * A variável somabyte vai servir para agrupar os bytes na ordem certa
     * A variável envbyte vai servir para enviar o byte ja compactado
     * A variável tamanho serve para indicar o tamanho de somabyte em bits
     * O unsigned serve para ter certeza de que a variável vai ter 4 bytes de alocação
     */
    unsigned long int proxpix=pegaProximoPixel(),envbyte,somabyte=0,tamanho=0;
    if(n_bits>=1 && n_bits<=8)/* Testa se a entrada dos n_bits e válida */
    {
        while(proxpix!=0xFFFFFFFF)/* Testa se não chegou ao final */
        {
            proxpix=proxpix>>(8-n_bits);/* Desloca 8-n_bits para direita deixando apenas os bits desejados */
            somabyte=proxpix+(somabyte<<n_bits);/* Desloca o somabyte n_bits para adicionar o proxpix ao seu lado*/
            tamanho+=n_bits;
            while(tamanho>=8)/* Testa se o somabyte já tem 1 byte completo e pronto para enviar */
            {
                envbyte=somabyte;/* O envbyte recebe o valor de somabyte para auxiliar no envio do byte */
                envbyte=envbyte>>(tamanho-8);/* O envbyte é deslocado tamanho-8 bits a direita para sobrar apenas o byte desejado */
                enviaByteRBD((unsigned char)envbyte);/* O envbyte é convertido para byte (char) e enviado */
                /*
                 * Somabyte é deslocado 32-(tamanho-8) casas para a esquerda e depois para a direita
                 * Isso elimina os 8 bits que foram enviados
                 * O shift dado 2 vezes serve para impedir o bug que impede o shift com mais de 31 casas
                 */
                somabyte=somabyte<<(20-tamanho);
                somabyte=somabyte<<20;
                somabyte=somabyte>>(20-tamanho);
                somabyte=somabyte>>20;
                tamanho-=8;/* O tamanho é diminuido em 8 (1 byte) */
            }
            proxpix=pegaProximoPixel();/* Proxpix armazena o próximo pixel */
            /*
             * Quando chegar no último byte (0xFFFFFFFF), o loop não realizado.
             * Qualquer valor que sobrou em somabyte será descartado.
             * Por isso o proxpix recebe o próximo pixel no final do loop
             */
        }
    }
}

void decodificaStreamRBD (int n_bits, int preenche)
{
    /*
     * A variável proxbyte serve para pegar sempre o próximo byte
     * A variável somapix vai servir para agrupar os pixels na ordem certa
     * A variável envpix vai servir para enviar os pixels já descompactados
     * A variável tamanho serve para indicar o tamanho de somapix em bits
     * O unsigned serve para ter certeza de que a variável vai ter 4 bytes de alocação
     */
    unsigned long int proxbyte=pegaProximoByteRBD(),envpix,somapix=0;
    unsigned char tamanho=0;
    if(n_bits>=1 && n_bits<=8)/* Testa se a entrada dos n_bits e válida */
    {
        srand(time(NULL));/* Semente a ser usada para os numeros 'aleatórios' */
        while(proxbyte!=0xFFFFFFFF)/* Testa se não chegou ao final */
        {
            somapix=proxbyte+(somapix<<8);/* Desloca o somapix 8 bytes para a esquerda para adicionar o proxbyte */
            tamanho+=8;
            while(tamanho>=n_bits)/* Testa quando já se tem pixels completos */
            {
                envpix=somapix;/* O envpix recebe o valor de somapix para auxiliar no envio do pixel */
                /*
                 * O envpix é deslocado tamanho-n_bits bits à direita para sobrar apenas o pixel desejado
                 */
                envpix=envpix>>(tamanho-n_bits);
                envpix=envpix<<(8-n_bits);/* O envpix é deslocado 8-n_bits à esquerda para se tornar um byte completo */
                if(preenche)/* Para preenche verdadeiro, o resto do byte terá bits aleatórios */
                    envpix=envpix+rand()%(1<<(8-n_bits));/* Gera números entre 0 e 2^(8-n_bits)-1 */
                enviaPixel((unsigned char)envpix);/* O envpix é convertido para byte (char) e enviado */
                /*
                 * Somapix é deslocado 32-(tamanho-n_bits) casas à esquerda e depois à direita
                 * Isso elimina os n_bits bits que foram enviados
                 * O shift dado 2 vezes serve para impedir o bug que impede o shift com mais de 31 casas
                 */
                somapix=somapix<<(12+n_bits-tamanho);
                somapix=somapix<<20;
                somapix=somapix>>(12+n_bits-tamanho);
                somapix=somapix>>20;
                tamanho-=n_bits;/* O tamanho é diminuído em n_bits */
            }
            proxbyte=pegaProximoByteRBD();/* Proxbyte armazena o próximo byte */
            /*
             * Quando chegar no último byte (0xFFFFFFFF), o loop nao será realizado.
             * Qualquer valor que sobrou em somapix será descartado.
             * Por isso o proxbyte recebe o próximo byte no final do loop
             */
        }
    }
}
