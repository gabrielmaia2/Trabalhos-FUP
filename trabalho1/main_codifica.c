/*============================================================================*/
/* IF61C - 2017-1 - TRABALHO 1                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Reduzindo a profundidade de bits de imagens (codificando). */
/*============================================================================*/

#include "trabalho1.h"

/*============================================================================*/

#define ARQUIVO_IMG "img/teste1.bmp" /* Imagem para teste. */
#define ARQUIVO_RBD "img/teste1.rbd" /* Saída codificada. */
#define BPP 8 /* Precisa estar entre 1 e 8. */

/*============================================================================*/

int main ()
{
    criaStreamsCod (ARQUIVO_IMG, ARQUIVO_RBD);
    codificaStreamImagem (BPP);
    destroiStreams ();

    return (0);
}

/*============================================================================*/

