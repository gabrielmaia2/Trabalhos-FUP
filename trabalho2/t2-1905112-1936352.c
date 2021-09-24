#define MAX(a,b) ((a>b)?a:b)  /* calcula o valor máximo entre 2 parâmetros */
#define MAX3(a,b,c) (MAX(a,MAX(b,c)))  /* calcula o valor máximo entre 3 parâmetros */

void mudaGanho (double* dados, unsigned long n_amostras, double ganho)
{
    unsigned long i;
    for(i=0; i < n_amostras; i++)
        dados[i]*=ganho; /* multiplica cada valor do sinal original pelo ganho */
}

void ruidoPeriodico (double* dados, unsigned long n_amostras,unsigned short intervalo)
{
    unsigned long i;
    double pico=1;
    for(i=0; i<n_amostras; i+=intervalo)
    {
        dados[i]=pico; /* adiciona um "estalo" a cada intervalo */
        pico*=(-1); /* alterna o valor do pico adicionado ao sinal */
    }
}

double mediana(double x,double y,double z)
{
    /* retorna o máximo dos 2 valores menores (seria a mediana dos 3) */
    if(MAX3(x,y,z)==x)
        return MAX(y,z);
    if(MAX3(x,y,z)==y)
        return MAX(x,z);
    if(MAX3(x,y,z)==z)
        return MAX(y,x);
}

void removeRuido (double* dados, unsigned long n_amostras)
{
    unsigned long i;
    double med,ant=dados[0]; /* med = mediana, ant = valor anterior */
    for(i=1; i<n_amostras-1; i++)
    {
        med=mediana(ant,dados[i],dados[i+1]); /* calcula a mediana entre dados[i], o valor anterior e o próximo */
        ant=dados[i]; /* ant armazena o valor anterior */
        dados[i]=med; /* substitiu dados[i] pela mediana */
    }
}

int contaSaturacoes (double* dados, unsigned long n_amostras)
{
    unsigned long i,sum=0; /* sum = contador */
    for(i=0; i<n_amostras; i++)
    {
        if(dados[i] > 1 || dados[i] < -1) /* soma 1 ao contador para cada valor que excede 1 ou -1 */
            sum++;
    }
    return sum;
}

void limitaSinal (double* dados, unsigned long n_amostras, unsigned short n_passos)
{
    unsigned long i,j;
    double ratio,inc;  /* ratio = razão para multiplicar os dados, inc = incremento da razão */
    for(i=0;i<n_amostras;i++)
    {
        if((dados[i]>1) || (dados[i]<-1))
        {
            ratio=1/dados[i];  /* calcula a razão pela qual dados[i] será multiplicado */
            if(ratio<0) /* no caso de dados[i] ser negativo, torna a razão positiva */
                ratio*=-1;
            inc=(1-ratio)/(n_passos+1);  /* Incremento a ser dado para a razão nos vizinhos. O +1 é para contar com o dado atual */
            dados[i]*=ratio; /* reduz o valor que excedeu 1 ou -1 para 1 ou -1 respectivamente */

            for(j=1;j<=n_passos;j++)
            {
                ratio+=inc;  /* aumenta a razão conforme o parâmetro n_passos */
                if((i+j) < n_amostras)
                    dados[i+j]*=ratio;  /* multiplica os próximos valores pela razão */

                if((i-j) >= 0)
                    dados[i-j]*=ratio;  /* multiplica os valores anteriores pela razão */
            }
        }
    }
}
