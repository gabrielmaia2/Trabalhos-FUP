/*============================================================================*/
/* IF61C - 2017-1 - TRABALHO 1                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Fun��es pedidas e auxiliares para o 1o trabalho da disciplina IF61C
 * (Fundamentos de Programa��o 1), 1o semestre de 2017, prof. Bogdan T. Nassu,
 * Universidade Tecnol�gica Federal do Paran�. */
/*============================================================================*/

#ifndef __TRABALHO1_H
#define __TRABALHO1_H

/*============================================================================*/
/* Fun��es do trabalho. */

void codificaStreamImagem (int n_bits);
void decodificaStreamRBD (int n_bits, int preenche);

/*----------------------------------------------------------------------------*/
/* Fun��es auxiliares que DEVEM ser chamadas pelos alunos. */

unsigned int pegaProximoPixel ();
void enviaPixel (unsigned char pixel);
unsigned int pegaProximoByteRBD ();
void enviaByteRBD (unsigned char byte);

/*----------------------------------------------------------------------------*/
/* Fun��es auxiliares que N�O DEVEM ser chamadas pelos alunos. */

void criaStreamsCod (char* arquivo_img, char* arquivo_rle);
void criaStreamsDecod (char* arquivo_rbd);
void salvaStreamImagem (char* arquivo_img);
void destroiStreams ();

/*============================================================================*/
#endif /* __TRABALHO1_H */