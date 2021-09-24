/*============================================================================*/
/* Exemplo: adiciona e remove ruído. */

#include <stdlib.h>
#include "wavfile.h"

#include "trabalho2.h"

/*============================================================================*/

#define INPUT_FILE "..\\data\\sonic1.wav"
#define OUTPUT_FILE1 "..\\data\\teste2_ruido.wav"
#define OUTPUT_FILE2 "..\\data\\teste3_limpo.wav"

/*============================================================================*/

int main ()
{
	WavHeader header; /* Cabeçalho para as informações sobre o arquivo. */
	double* data_l; /* Dados do canal esquerdo. Alocado na readWavFile. */
	double* data_r; /* Dados do canal direito. Alocado na readWavFile, se necessário. */
    int n_amostras; /* Número de amostras nos buffers. */

    /* Lê um arquivo. */
    n_amostras = readWavFile (INPUT_FILE, &header, &data_l, &data_r);
    if (!n_amostras)
    {
        printf ("Erro lendo %s.\n", INPUT_FILE);
        return (1);
    }

    /* Adiciona ruído (deixando o período diferente na esquerda e na direita para ficar bem perceptível). */
    ruidoPeriodico (data_l, n_amostras, 44100);
    ruidoPeriodico (data_r, n_amostras, 8000);

    if (!writeWavFile (OUTPUT_FILE1, &header, data_l, data_r))
    {
        printf ("Erro salvando %s.\n", OUTPUT_FILE1);
        return (1);
    }

    /* Remove ruído. */
    removeRuido (data_l, n_amostras);
    removeRuido (data_r, n_amostras);

    if (!writeWavFile (OUTPUT_FILE2, &header, data_l, data_r))
    {
        printf ("Erro salvando %s.\n", OUTPUT_FILE2);
        return (1);
    }

    /* Desaloca os vetores. O direito só vai ter sido alocado se o arquivo for estéreo. */
	free (data_l);
	if (data_r)
        free (data_r);

	return (0);
}

/*============================================================================*/
