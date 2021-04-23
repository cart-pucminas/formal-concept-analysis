#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <omp.h>

int *dadosROM;
//int *RamObjetos;
//int qtdObjetos = 8192; //total de objetos do arquivo
int qtdObjetos = 1000;
//int combAtributos = 65535; //2^16
int combAtributos = 33554431;
int indice = 1;

void CarregaContexto(){
  dadosROM = (int*)malloc(sizeof(int) * qtdObjetos);
  
  char dir[200];

  strcpy(dir, get_current_dir_name());

  int aux = strlen(dir);
  char Linha[25];
  char *result;

  strcat(dir, "/baseTeste.txt");

  //printf("Endereco: %s\n", dir);
  FILE *arq = fopen(dir, "rt");

  if (arq == NULL)
  {
      printf("Problemas na CRIACAO do arquivo\n");
      return;
  } 

  int i = 0;
  while (!feof(arq))
  {
    result = fgets(Linha, 25, arq);
    if (result){
      dadosROM[i] = atoi(Linha);
      //printf("Valor %d: %d\n", i,dadosROM[i]);
    }
    i++;
  }
  fclose(arq);
}

void SalvarResultadoArquivo(double tempoGasto, int conceitos){
  FILE *pont_arq;
  char palavra[200] = "Tempo gasto: "; // variável do tipo string
  
  //abrindo o arquivo
  pont_arq = fopen("arquivoResultado.txt", "a");

  if(pont_arq != NULL){
    fprintf(pont_arq, "%s", "Testes de ");
    fprintf(pont_arq, "%d", qtdObjetos);
    fprintf(pont_arq, "%s", " Objetos e ");
    fprintf(pont_arq, "%d", combAtributos);
    fprintf(pont_arq, "%s", " Combinacoes\n");
    fprintf(pont_arq, "%s", "Tempo gasto: ");
    fprintf(pont_arq, "%lf", tempoGasto);
    fprintf(pont_arq, "%s", "  - Conceitos: ");
    fprintf(pont_arq, "%d \n\n", conceitos);
  }
  

  fclose(pont_arq);
}

bool ExecutarKernel(int indice){
    int RX;
    int i,j;
    RX = combAtributos;
    j=0;

    ///BC IDA
    for(i=0; i<qtdObjetos; i++) //Percorre todos os objetos da base (memória ROM)
    {
        if((indice & dadosROM[i]) == indice) 
        {
          RX = RX & dadosROM[i];
        }
        else
        {
          j++;
        }
    }
    if((RX == indice) && (j != qtdObjetos)){
        return true;
    }
    return false;
}

int main() {
  time_t Ticks[2];
    
	
  printf("\nCarregando contexto...");
  CarregaContexto();

  int ConceitosV[100], Conceitos;
  for(int z=0; z<100; z++)
  {
	  ConceitosV[z] = 0;
  }

  double start_time, end_time;

  printf("\nExecutando Kernel...\n");
  
  Ticks[0] = time(NULL);
  int i;  
  #pragma omp parallel for private(i)
  for (i = 1; i < combAtributos - 1; i++)
  {
    if(ExecutarKernel(i) == true){
      ConceitosV[omp_get_thread_num()]++;
    }
  }
  Ticks[1] = time(NULL);
  end_time = difftime(Ticks[1], Ticks[0]);

  for(int w=0; w<100; w++)
  {
	  Conceitos += ConceitosV[w];
  }
  printf("\n\nTempo de execução: %lf", end_time);
  
  Conceitos += 2;
  printf("\n\nTotal de Conceitos: %d\n\n", Conceitos);

  SalvarResultadoArquivo(end_time, Conceitos);


  return 0;
}
