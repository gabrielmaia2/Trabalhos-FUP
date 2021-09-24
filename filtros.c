
unsigned char mediana9(unsigned char *vetor)/* Mediana de 9 valores */
{
    unsigned char subs;
    int j,aux=1;
    while(aux!=0)
    {
        aux=0;
        for(j=0;j<8;j++)
        {
            if(vetor[j]>vetor[j+1])/* Coloca os valores maiores uma posição na frente do vetor e os menores uma posição atrás */
            {
                aux++;
                subs=vetor[j];
                vetor[j]=vetor[j+1];
                vetor[j+1]=subs;
            }
        }
    }
    return vetor[4];/* Retorna a posição do meio do vetor */
}

void filtroMediana9(Imagem1C *img)
{
    /* Remove parte do ruído pegando a mediana entre os pontos de um quadrado 3x3 e colocando o valor no ponto do meio */
    unsigned long i,j;
    int k,l;
    unsigned char vet[9];
    unsigned char **copia = (unsigned char **) malloc(sizeof(unsigned char *)*img->altura);/* Copia é a matriz cópia */
    for(i=0;i<img->altura;i++)
    {
        copia[i]= (unsigned char *) malloc(sizeof(unsigned char)*img->largura);
    }
    for(i=0;i<img->altura;i++)
    {
        for(j=0;j<img->largura;j++)
        {
            copia[i][j]=img->dados[i][j];/* Copia a matriz a ser modificada para a matriz cópia */
        }
    }
    for(i=1;i<img->altura-1;i++)
    {
        for(j=1;j<img->largura-1;j++)
        {
            /* Pega uma matriz 3x3 com os 9 pontos na vizinhança do atual (incluindo este) e tira a mediana dos valores */
            for(k=0;k<3;k++)
            {
                for(l=0;l<3;l++)
                {
                    vet[k*3+l]=copia[i+k-1][j+l-1];
                }
            }

            /* Coloca a mediana dos valores na posição atual (remove uma parte do ruído) */
            img->dados[i][j]=mediana9(vet);
        }
    }

    /*
     * O motivo de não modificar diretamente a matriz original é para usar os valores
     * originais para encontrar a mediana no lugar dos já modificados
     */
    for(i=0;i<img->altura;i++)
    {
        free(copia[i]);
    }
    free(copia);
}

void filtroMedia9(Imagem1C *img)
{
    /* Remove parte do ruído pegando a média entre os pontos de um quadrado 3x3 e colocando o valor no ponto do meio */
    unsigned long i,j;
    int k,l,soma;
    unsigned char vet[9];
    unsigned char **copia = (unsigned char **) malloc(sizeof(unsigned char *)*img->altura);/* Copia é a matriz cópia */
    for(i=0;i<img->altura;i++)
    {
        copia[i]= (unsigned char *) malloc(sizeof(unsigned char)*img->largura);
    }
    for(i=0;i<img->altura;i++)
    {
        for(j=0;j<img->largura;j++)
        {
            copia[i][j]=img->dados[i][j];/* Copia a matriz a ser modificada para a matriz cópia */
        }
    }
    for(i=1;i<img->altura-1;i++)
    {
        for(j=1;j<img->largura-1;j++)
        {
            soma=0;
            /* Pega uma matriz 3x3 com os 9 pontos na vizinhança do atual (incluindo este) */
            for(k=0;k<3;k++)
            {
                for(l=0;l<3;l++)
                {
                    soma+=(int) copia[i+k-1][j+l-1];
                }
            }

            /* Coloca a média dos valores na posição atual (remove uma parte do ruído) */
            img->dados[i][j]=(char) (soma/9);
        }
    }

    /*
     * O motivo de não modificar diretamente a matriz original é para usar os valores
     * originais para encontrar a média no lugar dos já modificados
     */
    for(i=0;i<img->altura;i++)
    {
        free(copia[i]);
    }
    free(copia);
}






