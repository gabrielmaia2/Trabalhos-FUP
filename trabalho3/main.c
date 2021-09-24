/*============================================================================*/
/* TRABALHO: WII SENSOR BAR                                                   */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "gerador_de_testes.h"
#include "trabalho3.h"

/*============================================================================*/
/* Um modificador para a semente usada pelo gerador de números aleatórios na
 * hora de criar imagens de teste. Todos os trabalhos usarão as mesmas
 * imagens, então o offset precisa ser igual para todos. Entretanto, usaremos
 * um valor diferente na hora de testar os trabalhos de fato. */

//#define RANDOM_SEED_OFFSET 1

//#define N_TESTES 10000
//#define SALVA_ERRO 1 /* Flag que diz se devemos salvar o arquivo de erros */
//#define ERRO_MIN 1.001 /* Erro mínimo para salvar erro */

//#define SALVA_ANTES 0 /* Flag que diz se devemos salvar as imagens de teste. Desative se for rodar muitos testes! */
//#define SALVA_DEPOIS 0 /* Flag que diz se devemos salvar as imagens de teste. Desative se for rodar muitos testes! */

#define CAM_SALVA_ANTES "./img/antes_%d.bmp"
#define CAM_SALVA_DEPOIS "./img/depois_%d.bmp"


/*============================================================================*/

#define MIN(a,b) ((a<b)? a : b)
#define MAX(a,b) ((a>b)? a : b)

/*============================================================================*/

int main ()
{
    int erros1=0,erros2=0,erros3=0,erros4=0,erros10=0,erros100=0,erros1000=0,errosneg=0;
	int i;
	Imagem1C* img; /* Imagem de teste. */
	clock_t tempo_inicio, tempo_total = 0;
	double erro_l, erro_r;
	double erro_medio=0, desvpad, erro_max=0;
	int pior_teste = -1,angulos_errados = 0;
	Coordenada l_real, r_real, l_medido, r_medido;
	double diagonal, angulo_real, angulo_medido;

	char erro_string[100];
	int RANDOM_SEED_OFFSET,N_TESTES,SALVA_ERRO,ERRO_MIN,SALVA_ANTES,SALVA_DEPOIS;
	printf("Seed offset:\n");
	scanf("%d",&RANDOM_SEED_OFFSET);
	printf("Num testes:\n");
	scanf("%d",&N_TESTES);
	double erros [N_TESTES];
	printf("Salva erro:\n");
	scanf("%d",&SALVA_ERRO);
	printf("Erro min:\n");
	scanf("%d",&ERRO_MIN);
	printf("Salva antes:\n");
	scanf("%d",&SALVA_ANTES);
	printf("Salva depois:\n");
	scanf("%d",&SALVA_DEPOIS);
	system("clear");

    /* Realiza N_TESTES testes. */
	for (i = 0; i < N_TESTES; i++)
	{
        if(SALVA_ERRO)
			sprintf (erro_string, "./erros/%d - %s - %s.txt",N_TESTES,__DATE__,__TIME__);

		/* Gera um caso de teste. */
		img = geraCasoDeTeste (i+RANDOM_SEED_OFFSET, &l_real, &r_real, &angulo_real);
		if (!img)
		{
			printf ("Erro gerando caso de teste (avisar o professor!!!).\n");
			return (1);
		}
		diagonal = sqrt (img->altura*img->altura + img->largura*img->largura);

		/* Salva (se necessário). */
		if (SALVA_ANTES)
		{
			char foostring [64];
			sprintf (foostring, CAM_SALVA_ANTES, i);
			salvaImagem1C (img, foostring);
		}

		/* Invoca o testador. */
		tempo_inicio = clock ();
		angulo_medido = detectaSensorBar (img, &l_medido, &r_medido);
		tempo_total += clock () - tempo_inicio;

		/* Compara os resultados com o que foi gerado. */
		erro_l = sqrt ((l_real.x-l_medido.x)*(l_real.x-l_medido.x) + (l_real.y-l_medido.y)*(l_real.y-l_medido.y));
        erro_r = sqrt ((r_real.x-r_medido.x)*(r_real.x-r_medido.x) + (r_real.y-r_medido.y)*(r_real.y-r_medido.y));
        erros [i] = MIN (diagonal, MAX (erro_l, erro_r));

        {
            /* Salva (se necessário). */
            if((erros[i]>=1)&&(erros[i]<2))
                erros1++;
            if((erros[i]>=2)&&(erros[i]<3))
                erros2++;
            if((erros[i]>=3)&&(erros[i]<4))
                erros3++;
            if((erros[i]>=4)&&(erros[i]<10))
                erros4++;
            if((erros[i]>=10)&&(erros[i]<100))
                erros10++;
            if((erros[i]>=100)&&(erros[i]<1000))
                erros100++;
            if(erros[i]>=1000)
                erros1000++;
            if(erros[i]<0)
                errosneg++;
        }

        /* Calcula média, máximo e desvio padrão. */
        erro_medio += erros [i];
        if (erros [i] > erro_max)
        {
            erro_max = erros [i];
            pior_teste = i;
        }

        system("clear");/* Limpa a tela */

        if((erros[i]>=ERRO_MIN)&&(SALVA_ERRO))
		{
            FILE *fileprint;
            fileprint=fopen (erro_string, "a+");
            fprintf(fileprint, "Teste: %d\nReal     : (%d,%d) e (%d,%d) (%.4f graus)\nDetectado: (%d,%d) e (%d,%d) (%.4f graus)\nErro: %.2f\n\n", i, l_real.x, l_real.y, r_real.x, r_real.y, angulo_real, l_medido.x, l_medido.y, r_medido.x, r_medido.y, angulo_medido, erros [i]);
            fclose(fileprint);
		}

	    printf ("----------------------------------------\n");
		printf ("Teste %d\n", i);

        if (fabs (angulo_real - angulo_medido) > 0.0002)
            angulos_errados++;
        printf ("Real     : (%d,%d) e (%d,%d) (%.4f graus)\nDetectado: (%d,%d) e (%d,%d) (%.4f graus)\nErro: %.2f\n", l_real.x, l_real.y, r_real.x, r_real.y, angulo_real, l_medido.x, l_medido.y, r_medido.x, r_medido.y, angulo_medido, erros [i]);
	    printf ("----------------------------------------\n");

        /* Mostra os resultados. */
        printf ("Numero de testes: %d\n", N_TESTES);
        printf ("Pior teste: %d (erro: %.6f)\n", pior_teste, erro_max);
        printf ("Angulos errados: %d\n", angulos_errados);
        printf ("Tempo total: %d\n", (int) tempo_total);
        /* Erros */
        printf ("\nErros:\n");
        printf ("Entre 1 e 2    : %d\tEntre 2 e 3      : %d\n",erros1,erros2);
        printf ("Entre 3 e 4    : %d\tEntre 4 e 10     : %d\n",erros3,erros4);
        printf ("Entre 10 e 100 : %d\tEntre 100 e 1000 : %d\n",erros10,erros100);
        printf ("Acima de 1000  : %d\tOverflow         : %d\n",erros1000,errosneg);

		/* Salva (se necessário). */
		if (SALVA_DEPOIS)
		{
			char foostring [64];
			sprintf (foostring, CAM_SALVA_DEPOIS, i);
			salvaImagem1C (img, foostring);
		}

		/* Joga fora as imagens geradas. */
		destroiImagem1C (img);
	}

    system("clear");/* Limpa a tela */

    /* Calcula média, máximo e desvio padrão.
    erro_medio = erros [0];
    erro_max = erros [0];
    pior_teste = 0;
    for (i = 1; i < N_TESTES; i++)
    {
        erro_medio += erros [i];
        if (erros [i] > erro_max)
        {
            erro_max = erros [i];
            pior_teste = i;
        }
    }*/
    erro_medio /= N_TESTES;
    desvpad = 0;
    for (i = 0; i < N_TESTES; i++)
        desvpad += (erros [i] - erro_medio)*(erros [i] - erro_medio);
    desvpad /= N_TESTES;
    desvpad = sqrt (desvpad);

    /* Salva (se necessário). */
    if(SALVA_ERRO)
    {
        FILE *fileprint;
        fileprint=fopen (erro_string, "a+");
        /* Mostra os resultados. */
        fprintf(fileprint, "Numero de testes: %d\n", N_TESTES);
        fprintf(fileprint, "Erro medio: %.6f\n", erro_medio);
        fprintf(fileprint, "Pior teste: %d (erro: %.6f)\n", pior_teste, erro_max);
        fprintf(fileprint, "Desvio padrao: %.6f\n", desvpad);
        fprintf(fileprint, "Score: %.6f\n", erro_medio + 1.5*desvpad);
        fprintf(fileprint, "Angulos errados: %d\n", angulos_errados);
        fprintf(fileprint, "Tempo total: %d\n", (int) tempo_total);
        /* Erros */
        fprintf(fileprint, "\nErros:\n");
        fprintf(fileprint, "Entre 1 e 2    : %d\tEntre 2 e 3      : %d\n",erros1,erros2);
        fprintf(fileprint, "Entre 3 e 4    : %d\tEntre 4 e 10     : %d\n",erros3,erros4);
        fprintf(fileprint, "Entre 10 e 100 : %d\tEntre 100 e 1000 : %d\n",erros10,erros100);
        fprintf(fileprint, "Acima de 1000  : %d\tOverflow         : %d\n",erros1000,errosneg);

        fclose(fileprint);
    }

    {
        /* Mostra os resultados. */
        printf ("----------------------------------------\n");
        printf ("Numero de testes: %d\n", N_TESTES);
        printf ("Erro medio: %.6f\n", erro_medio);
        printf ("Pior teste: %d (erro: %.6f)\n", pior_teste, erro_max);
        printf ("Desvio padrao: %.6f\n", desvpad);
        printf ("Score: %.6f\n", erro_medio + 1.5*desvpad);
        printf ("Angulos errados: %d\n", angulos_errados);
        printf ("Tempo total: %d\n", (int) tempo_total);
        /* Erros */
        printf ("\nErros:\n");
        printf ("Entre 1 e 2    : %d\tEntre 2 e 3      : %d\n",erros1,erros2);
        printf ("Entre 3 e 4    : %d\tEntre 4 e 10     : %d\n",erros3,erros4);
        printf ("Entre 10 e 100 : %d\tEntre 100 e 1000 : %d\n",erros10,erros100);
        printf ("Acima de 1000  : %d\tOverflow         : %d\n",erros1000,errosneg);
    }

    system("pause");

	return (0);
}

/*============================================================================*/
