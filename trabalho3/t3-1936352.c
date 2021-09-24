/**================================================================**/
/*------------------------------------------------------------------*/
/**                           Trabalho 3                           **/
/*------------------------------------------------------------------*/
/**    Autor : Gabriel Maia Gondim - gondim@alunos.utfpr.edu.br    **/
/*------------------------------------------------------------------*/
/**================================================================**/

#include <stdlib.h>
#include <math.h>
#include "trabalho3.h"
#define FRAC 0.6


double detectaSensorBar (Imagem1C *img, Coordenada *l, Coordenada *r)
{
    unsigned char maior=0;
    unsigned long i,j,k,m;/* Contadores */
    double medx,medy;/* Variáveis para encontrar as coordenadas do centro */

    /* Mat é o tamanho da "matriz" usada para achar o centro, cord é para pegar os dois maiores centros */
    Coordenada mat,cord[4];

    /* Tamanhos (em números de pixels claros). Borda é uma bool para ver se o loop já chegou na borda do círculo */
    int tam[4]={0,0,0,0},borda;

    /*
     * Pega o maior valor (mais claro) dentre todos os pixels
     * Todas as contas vão ser feitas tendo como referência esse valor
     */
    for(i=0;i<img->altura;i++)
        for(j=0;j<img->largura;j++)
            if(maior<img->dados[i][j])
                maior=img->dados[i][j];

    /*
     * Encontra os centros
     * Aqui ele percorre toda a matriz de dados exceto nas bordas buscando por pixels não escuros
     */
    for(i=1;i<img->altura-1;i++)
    {
        for(j=1;j<img->largura-1;j++)
        {
            if((img->dados[i][j])>maior*FRAC)/* Se o pixel não for escuro */
            {
                mat.x=mat.y=1;
                borda=0;/* Para entrar no loop */

                /* Encontra o centro de um círculo */
                while(!borda)
                {
                    borda=1;/* Caso não entre em nenhum dos if, borda será 1 indicando que chegou na borda do círculo */
                    for(k=0;(j>1)&&(k<mat.y);k++)/* Olha o lado esquerdo do círculo */
                    {
                        if(img->dados[i+k][j-1]>maior*FRAC)
                        {
                            j--;
                            mat.x++;
                            borda=0;
                            break;
                        }
                    }
                    for(k=0;(j+mat.x+1<img->largura)&&(k<mat.y);k++)/* Olha o lado direito do círculo */
                    {
                        if(img->dados[i+k][j+mat.x]>maior*FRAC)
                        {
                            mat.x++;
                            borda=0;
                            break;
                        }
                    }
                    for(k=0;(i+mat.y+1<img->altura)&&(k<mat.x);k++)/* Olha a parte de baixo do círculo */
                    {
                        if(img->dados[i+mat.y][j+k]>maior*FRAC)
                        {
                            mat.y++;
                            borda=0;
                            break;
                        }
                    }
                    /* Ele não verifica a parte de cima pois o loop já passou pela linha de cima */
                }

                /*
                 * A partir daqui serão usados os vetores tam e cord
                 * No vetor cord: cord[0] serve para substituição de valores, cord[1] serve para guardar o centro atual
                 * cord[2] e cord[3] servem para guardar as coordenadas dos dois maiores centros
                 * A organização das variáveis no vetor tam é a mesma que a do vetor cord
                 */
                tam[1]=medx=medy=0;
                for(k=0;k<mat.x;k++)
                {
                    for(m=0;m<mat.y;m++)
                    {
                        if(((i+k)<img->altura)&&((j+m)<img->largura)&&(img->dados[i+k][j+m]>maior*FRAC))
                        {
                            /* Verifica o tamanho do círculo atual */
                            tam[1]++;
                            medy+=k;
                            medx+=m;
                            /* Apaga o círculo atual para que não o encontre de novo */
                            img->dados[i+k][j+m]=0;
                        }
                    }
                }
                medy/=tam[1];
                medx/=tam[1];

                /*
                 * Guarda as coordenadas do centro atual
                 * Para arredondar corretamente, deve-se acrescentar 0.5
                 */
                cord[1].x=j+medx+0.5;
                cord[1].y=i+medy+0.5;

                /*
                 * Ordena as coordenadas de forma que as duas maiores fiquem nos dois últimos espaços (os centros)
                 * As outras duas posições servem apenas para armazenar temporariamente valores e para substituição
                 */
                for(k=0;k<3;k++)
                {
                    for(m=1;m<3;m++)
                    {
                        if(tam[m]>tam[m+1])
                        {
                            tam[0]=tam[m];
                            tam[m]=tam[m+1];
                            tam[m+1]=tam[0];

                            cord[0]=cord[m];
                            cord[m]=cord[m+1];
                            cord[m+1]=cord[0];
                        }
                    }
                }
                /* Zera as duas últimas posições para não dar erro na próxima soma */
                cord[1].x=cord[0].x=0;
                cord[1].y=cord[0].y=0;
            }
        }
    }

    /* Substituindo os dois maiores centros (duas posições da ponta) */
    *l=((cord[3].x)<(cord[2].x))?cord[3]:cord[2];
    *r=((cord[3].x)<(cord[2].x))?cord[2]:cord[3];
    /* Cálculo do ângulo */
    return (atan(((double) (r->y)-(l->y))/((r->x)-(l->x))));
}
