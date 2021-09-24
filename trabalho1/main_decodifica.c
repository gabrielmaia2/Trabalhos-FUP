/*============================================================================*/
/* IF61C - 2017-1 - TRABALHO 1                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Reduzindo a profundidade de bits de imagens (decodificando). */
/*============================================================================*/

#include "trabalho1.h"

/*============================================================================*/

#define ARQUIVO_RBD "img/teste0.rbd" /* Entrada codificada. */
#define ARQUIVO_IMG "img/teste1.bmp" /* Imagem de saída. */
#define BPP 4 /* Precisa estar entre 1 e 8. */

/*============================================================================*/

int main ()
{
    criaStreamsDecod (ARQUIVO_RBD);
    decodificaStreamRBD (BPP);
    salvaStreamImagem (ARQUIVO_IMG);
    destroiStreams ();
    return (0);
}

/*============================================================================*/

