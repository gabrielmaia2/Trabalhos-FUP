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

#ifndef __WAVFILE_H
#define __WAVFILE_H

/*============================================================================*/

#include <stdio.h>

/*============================================================================*/

#define TRAB_PI 3.1415926536

/*============================================================================*/

typedef struct
{
	/* "RIFF" chunk */
	char chunk_id [4];				/* ChunkID: 4 bytes, big endian. */
	unsigned long chunk_size;		/* ChunkSize: 4 bytes, little endian. */
	char format [4];				/* Format: 4 bytes, big endian. */

	/* "fmt" sub-chunk */
	char subchunk1_id [4];			/* Subchunk1ID: 4 bytes, big endian. */
	unsigned long subchunk1_size;	/* Subchunk1Size: 4 bytes, little endian. */
	unsigned short audio_format;	/* AudioFormat: 2 bytes, little endian. */
	unsigned short num_channels;	/* NumChannels: 2 bytes, litte endian. */
	unsigned long sample_rate;		/* SampleRate: 4 bytes, little endian. */
	unsigned long byte_rate;		/* ByteRate: 4 bytes, little endian. */
	unsigned short block_align;		/* BlockAlign: 2 bytes, little endian. */
	unsigned short bits_per_sample;	/* BitsPerSample: 2 bytes, little endian. */

	/* "data" sub-chunk */
	char subchunk2_id [4];			/* Subchunk2ID: 4 bytes, big endian. */
	unsigned long subchunk2_size;	/* Subchunk2Size: 4 bytes, little endian. */

} WavHeader;

/*============================================================================*/

int readWavFile (char* filename, WavHeader* header, double** data_l, double** data_r);
int writeWavFile (char* filename, WavHeader* header, double* data_l, double* data_r);
WavHeader generateSignal (unsigned long* n_samples, unsigned short num_channels, unsigned long sample_rate, double** data_l, double** data_r);
void generateRandomData (double* data, unsigned long n_samples);
int writeSamplesAsText (char* filename, double* data, unsigned long n_samples);

/*============================================================================*/
#endif /* __WAVFILE_H */
