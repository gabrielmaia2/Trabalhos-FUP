/*============================================================================*/
/* Exemplo: Aumenta (bastante) o volume do sinal e conta saturações. Limita o
 * sinal. */

#include <stdlib.h>
#include "wavfile.h"

#include "trabalho2.h"

/*============================================================================*/

#define INPUT_FILE "..\\data\\sonic1.wav"
#define OUTPUT_FILE "..\\data\\teste5_limita.wav"

/*============================================================================*/

int main ()
{
	WavHeader header; /* Cabeçalho para as informações sobre o arquivo. */
	double* dados_l; /* Dados do canal esquerdo. Alocado na readWavFile. */
	double* dados_r; /* Dados do canal direito. Alocado na readWavFile, se necessário. */
	int n_amostras; /* Número de amostras nos buffers. */
	int n_saturacoes; /* Número de amostras com saturação. */

    /* Lê um arquivo. */
    n_amostras = readWavFile (INPUT_FILE, &header, &dados_l, &dados_r);
    if (!n_amostras)
    {
        printf ("Erro lendo %s.\n", INPUT_FILE);
        return (1);
    }

	/* Confirma que o arquivo é stereo. */
    if (!dados_r)
    {
        printf ("%s nao eh um arquivo stereo.\n", INPUT_FILE);
        return (1);
    }

    /* Conta as saturações. */
    printf ("Antes de aumentar o volume:\n");
    n_saturacoes = contaSaturacoes (dados_l, n_amostras);
    printf ("Saturacoes esquerda: %d\n", n_saturacoes);
    n_saturacoes = contaSaturacoes (dados_r, n_amostras);
    printf ("Saturacoes direita: %d\n\n", n_saturacoes);

    /* Aumenta o volume. */
    mudaGanho (dados_l, n_amostras, 5);
    mudaGanho (dados_r, n_amostras, 5);

    /* Conta as saturações de novo. */
    printf ("Depois de aumentar o volume:\n");
    n_saturacoes = contaSaturacoes (dados_l, n_amostras);
    printf ("Saturacoes esquerda: %d\n", n_saturacoes);
    n_saturacoes = contaSaturacoes (dados_r, n_amostras);
    printf ("Saturacoes direita: %d\n\n", n_saturacoes);

    /* Limita. */
    printf ("Limitando em [-1,1]:\n");
    limitaSinal (dados_l, n_amostras, 50);
    limitaSinal (dados_r, n_amostras, 50);
    n_saturacoes = contaSaturacoes (dados_l, n_amostras);
    printf ("Saturacoes esquerda: %d\n", n_saturacoes);
    n_saturacoes = contaSaturacoes (dados_r, n_amostras);
    printf ("Saturacoes direita: %d\n\n", n_saturacoes);

    /* Salva. */
    if (!writeWavFile (OUTPUT_FILE, &header, dados_l, dados_r))
    {
        printf ("Erro salvando %s.\n", OUTPUT_FILE);
        return (1);
    }

    /* Desaloca os vetores. */
	free (dados_l);
    free (dados_r);

	return (0);
}

/*============================================================================*/
