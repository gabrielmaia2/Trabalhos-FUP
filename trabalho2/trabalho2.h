/*============================================================================*/
/* IF61C - 2017-1 - TRABALHO 2                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Funções pedidas para o 2o trabalho da turma da disciplina IF61C
 * (Fundamentos de Programação 1), 1o semestre de 2017, prof. Bogdan T. Nassu,
 * Universidade Tecnológica Federal do Paraná. */
 /*============================================================================*/

#ifndef __TRABALHO2_H
#define __TRABALHO2_H

/*============================================================================*/

void mudaGanho (double* dados, unsigned long n_amostras, double ganho);
void ruidoPeriodico (double* dados, unsigned long n_amostras, unsigned short intervalo);
void removeRuido (double* dados, unsigned long n_amostras);
int contaSaturacoes (double* dados, unsigned long n_amostras);
void limitaSinal (double* dados, unsigned long n_amostras, unsigned short n_passos);

/*============================================================================*/
#endif /* __TRABALHO2_H */
