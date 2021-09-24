/*============================================================================*/
/* MANIPULAÇÃO DE ARQUIVOS WAV                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Arquivo auxiliar com rotinas para leitura e escrita de arquivos wav.
 * Implementei aqui apenas um sub-conjunto mínimo do formato, não estou
 * tratando diversos casos. Codificamos os dados como doubles entre -1 e 1, a
 * conversão é feita quando o arquivo é lido ou escrito.  */
/*============================================================================*/

#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "wavfile.h"

/*============================================================================*/
/* Funções auxiliares (internas). */

int readRIFF (FILE* file, WavHeader* header);
int readFmt (FILE* file, WavHeader* header);
int readData (FILE* file, WavHeader* header, double** data_l, double** data_r);
unsigned long getLittleEndianULong (unsigned char* buffer);
unsigned short getLittleEndianUShort (unsigned char* buffer);
short getLittleEndianShort (unsigned char* buffer);

int writeRIFF (FILE* file, WavHeader* header);
int writeFmt (FILE* file, WavHeader* header);
int writeData (FILE* file, WavHeader* header, double* data_l, double* data_r);
void putLittleEndianULong (unsigned long val, unsigned char* buffer);
void putLittleEndianUShort (unsigned short val, unsigned char* buffer);

/*============================================================================*/
/* Macros. */

#define MIN(a,b)((a>b)?b:a)
#define MAX(a,b)((a>b)?a:b)
#define ROUND(x)((int)(x + (x >= 0 ? 0.5 : -0.5)))
#define N_SAMPLES(header)(header->subchunk2_size / header->block_align)

/*============================================================================*/
/* LEITURA DE ARQUIVO                                                         */
/*============================================================================*/
/** Abre e lê o conteúdo de um arquivo wav.
 *
 * Parâmetros: char* filename: nome do arquivo a ser lido.
 *             WavHeader* header: parâmetro de saída para os dados do
 *               cabeçalho.
 *             double** data_l: parâmetro de saída, é um ponteiro para um
 *               vetor dinâmico, onde manteremos os dados lidos para o canal
 *               esquerdo. O vetor será alocado nesta função, lembre-se de
 *               desalocá-lo quando ele não for mais necessário! Se ocorrerem
 *               erros, o vetor NÃO estará alocado quando a função retornar.
 *             double** data_r: igual ao anterior, mas para o canal direito.
 *               Se o arquivo for mono, o vetor será == NULL.
 *
 * Valor de Retorno: o número de amostras lidas, ou 0 se ocorreu um erro. */

int readWavFile (char* filename, WavHeader* header, double** data_l, double** data_r)
{
	FILE* file;

	/* Abre o arquivo. */
	file = fopen (filename, "rb");
	if (!file)
		return (0);

	/* Lê os blocos. */
	if (!readRIFF (file, header))
	{
		fclose (file);
		return (0);
	}

	if (!readFmt (file, header))
	{
		fclose (file);
		return (0);
	}

	if (!readData (file, header, data_l, data_r))
	{
		fclose (file);
		return (0);
	}

	fclose (file);
	return (N_SAMPLES (header));
}

/*----------------------------------------------------------------------------*/
/** Lê o chunk "RIFF" (o primeiro bloco do arquivo).
 *
 * Parâmetros: FILE* file: arquivo a ser lido. Supomos que já está aberto.
 *             WavHeader* header: parâmetro de saída para os dados do
 *               cabeçalho.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int readRIFF (FILE* file, WavHeader* header)
{
	unsigned char data [12]; /* O bloco tem exatamente 12 bytes. */
	int pos = 0;
	int i;

	if (fread ((void*) data, 1, 12, file) != 12)
	{
		printf ("Error reading the \"RIFF\" chunk.\n");
		return (0);
	}

	/* ChunkID: 4 bytes, big endian. */
	for (i = 0; i < 4; i++)
		header->chunk_id [i] = data [pos++];

	/* Precisa estar escrito "RIFF". */
	if (header->chunk_id [0] != 'R' ||
		header->chunk_id [1] != 'I' ||
		header->chunk_id [2] != 'F' ||
		header->chunk_id [3] != 'F')
	{
		printf ("Error: ChunkID must be \"RIFF\".\n");
		return (0);
	}

	/* ChunkSize: 4 bytes, little endian. */
	header->chunk_size = getLittleEndianULong (&(data [pos]));
	pos += 4;

	/* Format: 4 bytes, big endian. */
	for (i = 0; i < 4; i++)
		header->format [i] = data [pos++];

	/* Precisa estar escrito "WAVE". */
	if (header->format [0] != 'W' ||
		header->format [1] != 'A' ||
		header->format [2] != 'V' ||
		header->format [3] != 'E')
	{
		printf ("Error: format must be \"WAVE\".\n");
		return (0);
	}

	return (1);
}

/*----------------------------------------------------------------------------*/
/** Lê o chunk "fmt" (o segundo bloco do arquivo).
 *
 * Parâmetros: FILE* file: arquivo a ser lido. Supomos que já está aberto.
 *             WavHeader* header: parâmetro de saída para os dados do
 *               cabeçalho.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int readFmt (FILE* file, WavHeader* header)
{
	unsigned char pre_data [8]; /* Os primeiros 8 bytes do bloco têm informações sobre o resto. */
	unsigned char* data;
	int i;
	int pos = 0;

	if (fread ((void*) pre_data, 1, 8, file) != 8)
	{
		printf ("Error reading the \"fmt\" subchunk.\n");
		return (0);
	}

	/* Subchunk1ID: 4 bytes, big endian. */
	for (i = 0; i < 4; i++)
		header->subchunk1_id [i] = pre_data [pos++];

	/* Precisa estar escrito "fmt ". */
	if (header->subchunk1_id [0] != 'f' ||
		header->subchunk1_id [1] != 'm' ||
		header->subchunk1_id [2] != 't' ||
		header->subchunk1_id [3] != ' ')
	{
		printf ("Error: Subchunk1ID must be \"fmt \".\n");
		return (0);
	}

	/* Subchunk1Size: 4 bytes, little endian. */
	header->subchunk1_size = getLittleEndianULong (&(pre_data [pos]));

	/* OK, agora podemos ler o resto do bloco. */
	data = (unsigned char*) malloc (header->subchunk1_size);
	pos = 0;

	if (fread ((void*) data, 1, header->subchunk1_size, file) != header->subchunk1_size)
	{
		printf ("Error reading the \"fmt\" subchunk.\n");
		free (data);
		return (0);
	}

	/* AudioFormat: 2 bytes, little endian. */
	header->audio_format = getLittleEndianUShort (&(data [pos]));
	pos += 2;

	if (header->audio_format != 1)
	{
		printf ("Error: accepts only uncompressed PCM format.\n");
		free (data);
		return (0);
	}

	/* NumChannels: 2 bytes, litte endian. */
	header->num_channels = getLittleEndianUShort (&(data [pos]));
	pos += 2;

	if (header->num_channels != 1 && header->num_channels != 2)
	{
		printf ("Error: this program accepts only mono (single-channel) and stereo (2-channel) audio.\n");
		free (data);
		return (0);
	}

	/* SampleRate: 4 bytes, little endian. */
	header->sample_rate = getLittleEndianULong (&(data [pos]));
	pos += 4;

	/* ByteRate: 4 bytes, little endian. */
	header->byte_rate = getLittleEndianULong (&(data [pos]));
	pos += 4;

	/* BlockAlign: 2 bytes, little endian. */
	header->block_align = getLittleEndianUShort (&(data [pos]));
	pos += 2;

	/* BitsPerSample: 2 bytes, little endian. */
	header->bits_per_sample = getLittleEndianUShort (&(data [pos]));
	pos += 2;

	if (header->bits_per_sample != 16)
	{
		printf ("Error: this program accepts only 16-bit audio.\n");
		free (data);
		return (0);
	}

	if (header->byte_rate != header->sample_rate * header->num_channels * header->bits_per_sample / 8 ||
		header->block_align != header->num_channels * header->bits_per_sample / 8)
	{
		printf ("Error: inconsistent data in fmt subchunk.\n");
		free (data);
		return (0);
	}

	free (data);
	return (1);
}

/*----------------------------------------------------------------------------*/
/** Lê o chunk "data" (o terceiro bloco do arquivo).
 *
 * Parâmetros: FILE* file: arquivo a ser lido. Supomos que já está aberto.
 *             WavHeader* header: parâmetro de saída para os dados do
 *               cabeçalho.
 *             double** data_l: parâmetro de saída, é um ponteiro para um
 *               vetor dinâmico, onde manteremos os dados lidos para o canal
 *               esquerdo. O vetor será alocado nesta função, lembre-se de
 *               desalocá-lo quando ele não for mais necessário! Se ocorrerem
 *               erros, o vetor NÃO estará alocado quando a função retornar.
 *             double** data_r: igual ao anterior, mas para o canal direito.
 *               Se o arquivo for mono, o vetor será == NULL.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int readData (FILE* file, WavHeader* header, double** data_l, double** data_r)
{
	unsigned char pre_data [8]; /* Os primeiros 8 bytes do bloco têm informações sobre o resto. */
	unsigned char samples [4]; /* 2 bytes por canal. Chamamos fread aos poucos para economizar memória. */
	unsigned long i;
	int n_bytes_to_read;
	int pos = 0;

	if (fread ((void*) pre_data, 1, 8, file) != 8)
	{
		printf ("Error reading the \"data\" subchunk.\n");
		return (0);
	}

	/* Subchunk2ID: 4 bytes, big endian. */
	for (i = 0; i < 4; i++)
		header->subchunk2_id [i] = pre_data [pos++];

	/* Precisa estar escrito "data". */
	if (header->subchunk2_id [0] != 'd' ||
		header->subchunk2_id [1] != 'a' ||
		header->subchunk2_id [2] != 't' ||
		header->subchunk2_id [3] != 'a')
	{
		printf ("Error: Subchunk2ID must be \"data\".\n");
		return (0);
	}

	/* Subchunk2Size: 4 bytes, little endian. */
	header->subchunk2_size = getLittleEndianULong (&(pre_data [pos]));

    /* FIXME: It seems some programs write inconsistent data here, so I removed the test (as I can't fix those programs).
	if (header->chunk_size != 20 + header->subchunk1_size + header->subchunk2_size)
	{
		printf ("Error: inconsistent chunk sizes.\n");
		return (0);
	}*/

	/* OK, agora podemos ler o resto do bloco. */
	n_bytes_to_read = (header->num_channels == 2)? 4 : 2;
	*data_l = (double*) malloc (sizeof (double) * N_SAMPLES (header));
	*data_r = (header->num_channels == 2)? (double*) malloc (sizeof (double) * N_SAMPLES (header)) : NULL;

	for (i = 0; i < N_SAMPLES (header); i++)
	{
		if (fread ((void*) samples, 1, n_bytes_to_read, file) != n_bytes_to_read)
		{
			printf ("Error reading sample %lu.\n", i);
			free (*data_l);
			if (*data_r)
				free (*data_r);
			return (0);
		}

		(*data_l) [i] = (double) getLittleEndianShort (samples);

		if (header->num_channels == 2)
			(*data_r) [i] = (double) getLittleEndianShort (&(samples [2]));
	}

	/* Normalize */
	for (i = 0; i < N_SAMPLES (header); i++)
        if ((*data_l) [i] > 0)
            (*data_l) [i] /= (double) SHRT_MAX;
        else if ((*data_l) [i] < 0)
            (*data_l) [i] /= (double) -SHRT_MIN;

    if (*data_r)
    {
        for (i = 0; i < N_SAMPLES (header); i++)
            if ((*data_r) [i] > 0)
                (*data_r) [i] /= SHRT_MAX;
            else if ((*data_r) [i] < 0)
                (*data_r) [i] /= -SHRT_MIN;
    }

	return (1);
}

/*----------------------------------------------------------------------------*/
/** Pega os 4 primeiros bytes do buffer e coloca em um unsigned long,
 * considerando os bytes em ordem little endian.
 *
 * Parâmetros: unsigned char* buffer: lê 4 bytes daqui.
 *
 * Valor de Retorno: um unsigned long com os dados do buffer reorganizados. */

unsigned long getLittleEndianULong (unsigned char* buffer)
{
	return (buffer [3] << 24) | (buffer [2] << 16) | (buffer [1] << 8) | buffer [0];
}

/*----------------------------------------------------------------------------*/
/** Pega os 2 primeiros bytes do buffer e coloca em um unsigned short,
 * considerando os bytes em ordem little endian.
 *
 * Parâmetros: unsigned char* buffer: lê 2 bytes daqui.
 *
 * Valor de Retorno: um unsigned short com os dados do buffer reorganizados. */

unsigned short getLittleEndianUShort (unsigned char* buffer)
{
	return (buffer [1] << 8) | buffer [0];
}

/*----------------------------------------------------------------------------*/
/** Pega os 2 primeiros bytes do buffer e coloca em um short, considerando os
 * bytes em ordem little endian.
 *
 * Parâmetros: unsigned char* buffer: lê 2 bytes daqui.
 *
 * Valor de Retorno: um short com os dados do buffer reorganizados. */

short getLittleEndianShort (unsigned char* buffer)
{
	return (buffer [1] << 8) | buffer [0];
}

/*============================================================================*/
/* ESCRITA DE ARQUIVO                                                         */
/*============================================================================*/
/** Salva as amostras em um arquivo texto, uma amostra por linha. Esta função
 * é útil para visualizar os dados.
 *
 * Parâmetros: char* filename: nome do arquivo a ser escrito.
 *             double* data: vetor de dados contendo as amostras.
 *             unsigned long n_samples: número de amostras no vetor.
 *
 * Valor de Retorno: 1 se ocorreram erros, 0 do contrário. */

int writeSamplesAsText (char* filename, double* data, unsigned long n_samples)
{
    FILE* file;
    int i;

	/* Abre o arquivo. */
	file = fopen (filename, "w");

	if (!file)
        return (0);

    for (i = 0; i < n_samples; i++)
        if (fprintf (file, "%f\n", data [i]) < 0)
		{
            return (0);
			fclose (file);
		}

	fclose (file);
    return (1);
}

/*----------------------------------------------------------------------------*/
/** Escreve os dados de áudio em um arquivo wav. Esta função NÃO testa os dados
 * do cabeçalho, então dados inconsistentes não serão detectados. Cuidado!
 *
 * Parâmetros: char* filename: nome do arquivo a ser escrito.
 *             WavHeader* header: ponteiro para os dados do cabeçalho.
 *             double* data_l: ponteiro para os dados do canal esquerdo.
 *             double* data_r: ponteiro para os dados do canal direito.
 *               Ignorado se o arquivo for mono.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int writeWavFile (char* filename, WavHeader* header, double* data_l, double* data_r)
{
	FILE* file;

	/* Abre o arquivo. */
	file = fopen (filename, "wb");
	if (!file)
		return (0);

	/* Escreve os blocos. */
	if (!writeRIFF (file, header))
	{
		fclose (file);
		return (0);
	}

	if (!writeFmt (file, header))
	{
		fclose (file);
		return (0);
	}

	if (!writeData (file, header, data_l, data_r))
	{
		fclose (file);
		return (0);
	}

	fclose (file);
	return (1);
}

/*----------------------------------------------------------------------------*/
/** Escreve o chunk "RIFF" (o primeiro bloco do arquivo).
 *
 * Parâmetros: FILE* file: arquivo a ser escrito. Supomos que já está aberto.
 *             WavHeader* header: dados do cabeçalho.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int writeRIFF (FILE* file, WavHeader* header)
{
	unsigned char data [12]; /* O bloco tem exatamente 12 bytes. */
	int pos = 0;

	/* ChunkID: 4 bytes, big endian. */
	/* Precisa estar escrito "RIFF". */
	data [pos++] = 'R';
	data [pos++] = 'I';
	data [pos++] = 'F';
	data [pos++] = 'F';

	/* ChunkSize: 4 bytes, little endian. */
	putLittleEndianULong (header->chunk_size, &(data [pos]));
	pos += 4;

	/* Format: 4 bytes, big endian. */
	/* Precisa estar escrito "WAVE". */
	data [pos++] = 'W';
	data [pos++] = 'A';
	data [pos++] = 'V';
	data [pos++] = 'E';

	if (fwrite ((void*) data, 1, 12, file) != 12)
	{
		printf ("Error writing RIFF header.\n");
		return (0);
	}

	return (1);
}

/*----------------------------------------------------------------------------*/
/** Escreve o chunk "fmt" (o segundo bloco do arquivo).
 *
 * Parâmetros: FILE* file: arquivo a ser escrito. Supomos que já está aberto.
 *             WavHeader* header: dados do cabeçalho.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int writeFmt (FILE* file, WavHeader* header)
{
	unsigned char* data;
	int pos = 0;

	/* O tamanho do bloco é dado por Subchunk1Size. */
	data = (unsigned char*) malloc (header->subchunk1_size + 8);

	/* Subchunk1ID: 4 bytes, big endian. */
	/* Precisa estar escrito "fmt ". */
	data [pos++] = 'f';
	data [pos++] = 'm';
	data [pos++] = 't';
	data [pos++] = ' ';

	/* Subchunk1Size: 4 bytes, little endian. */
	putLittleEndianULong (header->subchunk1_size, &(data [pos]));
	pos += 4;

	/* AudioFormat: 2 bytes, little endian. */
	putLittleEndianUShort (header->audio_format, &(data [pos]));
	pos += 2;

	/* NumChannels: 2 bytes, litte endian. */
	putLittleEndianUShort (header->num_channels, &(data [pos]));
	pos += 2;

	/* SampleRate: 4 bytes, little endian. */
	putLittleEndianULong (header->sample_rate, &(data [pos]));
	pos += 4;

	/* ByteRate: 4 bytes, little endian. */
	putLittleEndianULong (header->byte_rate, &(data [pos]));
	pos += 4;

	/* BlockAlign: 2 bytes, little endian. */
	putLittleEndianUShort (header->block_align, &(data [pos]));
	pos += 2;

	/* BitsPerSample: 2 bytes, little endian. */
	putLittleEndianUShort (header->bits_per_sample, &(data [pos]));
	pos += 2;

	/* Fill any remaining positions with 0s. */
	for (; (unsigned int) pos < header->subchunk1_size + 8; pos++)
		data [pos] = 0;

	if (fwrite ((void*) data, 1, header->subchunk1_size + 8, file) != header->subchunk1_size + 8)
	{
		printf ("Error writing fmt header.\n");
		return (0);
	}

	free (data);
	return (1);
}

/*----------------------------------------------------------------------------*/
/** Escreve o chunk "data" (o terceiro bloco do arquivo).
 *
 * Parâmetros: FILE* file: arquivo a ser escrito. Supomos que já está aberto.
 *             WavHeader* header: dados do cabeçalho.
 *             double* data_l: ponteiro para os dados do canal esquerdo.
 *             double* data_r: ponteiro para os dados do canal direito.
 *               Ignorado se o arquivo for mono.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int writeData (FILE* file, WavHeader* header, double* data_l, double* data_r)
{
	unsigned char pre_data [8]; /* Os primeiros 8 bytes do bloco têm informações sobre o resto. */
	unsigned char samples [4]; /* 2 bytes por canal. Chamamos fwrite aos poucos para economizar memória. */
	int n_bytes_to_write;
	int pos = 0;
	unsigned long i;

	/* Subchunk2ID: 4 bytes, big endian. */
	/* Precisa estar escrito "data". */
	pre_data [pos++] = 'd';
	pre_data [pos++] = 'a';
	pre_data [pos++] = 't';
	pre_data [pos++] = 'a';

	/* Subchunk2Size: 4 bytes, little endian. */
	putLittleEndianULong (header->subchunk2_size, &(pre_data [pos]));

	/* Escreve... */
	if (fwrite ((void*) pre_data, 1, 8, file) != 8)
	{
		printf ("Error writing data header.\n");
		return (0);
	}

	/* OK, agora podemos escrever o resto do bloco. */
	n_bytes_to_write = (header->num_channels == 2)? 4 : 2;

	for (i = 0; i < N_SAMPLES (header); i++)
	{
	    double factor = (data_l [i] > 0)? SHRT_MAX : -SHRT_MIN;
		putLittleEndianUShort ((short) MIN (MAX (SHRT_MIN, data_l [i] * factor), SHRT_MAX), samples);

		if (header->num_channels == 2)
		{
            factor = (data_r [i] > 0)? SHRT_MAX : -SHRT_MIN;
			putLittleEndianUShort ((short) MIN (MAX (SHRT_MIN, data_r [i] * factor), SHRT_MAX), &(samples [2]));
		}

		if (fwrite ((void*) samples, 1, n_bytes_to_write, file) != n_bytes_to_write)
		{
			printf ("Error writing data.\n");
			return (0);
		}
	}

	return (1);
}

/*----------------------------------------------------------------------------*/
/** Coloca um unsigned long nos 4 primeiros bytes do buffer, em ordem little
 * endian.
 *
 * Parâmetros: unsigned long val: valor a escrever.
 *             unsigned char* buffer: coloca o valor aqui.
 *
 * Valor de Retorno: NENHUM */

void putLittleEndianULong (unsigned long val, unsigned char* buffer)
{
	buffer [0] = (unsigned char) val;
	buffer [1] = (unsigned char) (val >> 8);
	buffer [2] = (unsigned char) (val >> 16);
	buffer [3] = (unsigned char) (val >> 24);
}

/*----------------------------------------------------------------------------*/
/** Coloca um unsigned short nos 2 primeiros bytes do buffer, em ordem little
 * endian.
 *
 * Parâmetros: unsigned short val: valor a escrever.
 *             unsigned char* buffer: coloca o valor aqui.
 *
 * Valor de Retorno: NENHUM */

void putLittleEndianUShort (unsigned short val, unsigned char* buffer)
{
	buffer [0] = (unsigned char) val;
	buffer [1] = (unsigned char) (val >> 8);
}

/*============================================================================*/
/* GERAÇÃO DE DADOS                                                           */
/*============================================================================*/
/** Gera um sinal de conteúdo indeterminado.
 *
 * Parâmetros: unsigned long* n_samples: número de amostras a gerar para cada
 *               canal. Se for maior que o permitido, a função gera um sinal
 *               menor que o pedido e modifica o valor desta variável.
 *             unsigned short num_channels: número de canais a gerar. Precisa
 *               ser 1 ou 2, qualquer outro valor é automaticamente tratado
 *               como 1.
 *             unsigned long sample_rate: taxa de amostragem. Se for == 0,
 *               é automaticamente ajustada para 44.1kHz.
 *             double** data_l: parâmetro de saída, é um ponteiro para um
 *               vetor dinâmico, onde manteremos os dados para o canal
 *               esquerdo. O vetor será alocado nesta função, lembre-se de
 *               desalocá-lo quando ele não for mais necessário!
 *             double** data_r: igual ao anterior, mas para o canal direito.
 *               Se num_channels == 1, o vetor será == NULL.
 *
 * Valor de Retorno: o cabeçalho para o sinal gerado (os parâmetros data_l e data_r contêm o sinal propriamente dito) */

WavHeader generateSignal (unsigned long* n_samples, unsigned short num_channels, unsigned long sample_rate, double** data_l, double** data_r)
{
	WavHeader header;

	/* Verifica os parâmetros. */
	if (num_channels != 1 && num_channels != 2)
		num_channels = 1;

	if (sample_rate == 0)
		sample_rate = 44100;

	*n_samples = MIN (*n_samples, 0xffffffffUL / (2 * num_channels));

	/* Preenche o cabeçalho. Faz os chunks ao contrário, para ter tudo consistente. */
	header.subchunk2_id [0] = 'd';
	header.subchunk2_id [1] = 'a';
	header.subchunk2_id [2] = 't';
	header.subchunk2_id [3] = 'a';

	header.subchunk2_size = *n_samples * num_channels * 2;

	header.subchunk1_id [0] = 'f';
	header.subchunk1_id [1] = 'm';
	header.subchunk1_id [2] = 't';
	header.subchunk1_id [3] = ' ';

	header.subchunk1_size = 16; /* Sempre 16 para áudio PCM. */
	header.audio_format = 1; /* PCM */
	header.num_channels = num_channels;
	header.sample_rate = sample_rate;
	header.byte_rate = sample_rate * num_channels * 2;
	header.block_align = num_channels * 2;
	header.bits_per_sample = 16; /* Só trabalha com 16 bits. */

	header.chunk_id [0] = 'R';
	header.chunk_id [1] = 'I';
	header.chunk_id [2] = 'F';
	header.chunk_id [3] = 'F';

	header.chunk_size = 36 + header.subchunk1_size + header.subchunk2_size;

	header.format [0] = 'W';
	header.format [1] = 'A';
	header.format [2] = 'V';
	header.format [3] = 'E';

	/* Aloca e preenche os vetores. */
	*data_l = (double*) malloc (sizeof (double) * *n_samples);
	if (num_channels == 2)
		*data_r = (double*) malloc (sizeof (double) * *n_samples);

	return (header);
}

/*----------------------------------------------------------------------------*/
/** Preenche um vetor dado com dados aleatórios. Cada amostra deve variar
 * entre -32767 e 32767. A função NÃO inicia a semente aleatória.
 *
 * Parâmetros: double* data: vetor a preencher.
 *             unsigned long n_samples: número de amostras no vetor.
 *
 * Valor de Retorno: NENHUM (os dados são escritos no vetor) */

void generateRandomData (double* data, unsigned long n_samples)
{
    int i;
    for (i = 0; i < n_samples; i++)
		data [i] = ((rand ()%2)? 1 : -1) * ((double) rand () / (double) RAND_MAX);
}

/*============================================================================*/
