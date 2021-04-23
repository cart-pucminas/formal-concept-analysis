#include <iostream>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <CL/cl.hpp>
#include <string>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <ctime>

#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

int *dadosROM;
int *retKernel;

//Necessário definir quantidade de objetos e combinações dos atributos
//int qtdObjetos = 256; //total de objetos do arquivo
//int combAtributos = 4095; //Quantidade total de combinações

//int qtdObjetos = 8192; //total de objetos do arquivo
//int combAtributos = 65535; //Quantidade total de combinações

int qtdObjetos = 1000; //total de objetos do arquivo
int combAtributos = 33554431; //Quantidade total de combinaçõ

void CarregaContexto(){
  dadosROM = (int*)malloc(sizeof(int) * qtdObjetos);
  retKernel = (int*)malloc(sizeof(int) * combAtributos);

  for (int w = 0; w < combAtributos; w++)
  {
    retKernel[w] = 0;
  }
  char dir[200];

  char Linha[25];
  char *result;

  FILE* arq = fopen("baseTeste.txt", "r");

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
    }
    i++;
  }
  fclose(arq);
}

void SalvarResultadoArquivo(double tempoGasto, int conceitos){
  char palavra[200] = "Tempo gasto: "; 

  FILE* pont_arq = fopen("baseTeste.txt", "r");

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

int main()
{

	printf("\nIniciando programa...");
	clock_t time_clock;
	int Conceitos = 0;
	cl_int err = 0;
	double Tempo;

	//Criando o programa
	std::vector<cl::Platform> platform;
	cl::Platform::get(&platform);

    if (platform.empty()) {
	    std::cerr << "OpenCL platforms not found." << std::endl;
	    return 1;
	}
	std::vector<cl::Device> device;
    try {
        platform[0].getDevices(CL_DEVICE_TYPE_GPU, &device);
        std::string ext = device[0].getInfo<CL_DEVICE_EXTENSIONS>();
    } catch(...) {
        device.clear();
    }
    if (device.empty()) {
	    std::cerr << "GPUs with double precision not found." << std::endl;
	    return 1;
	}

	std::ifstream fcaDeviceFile("fca_device.cl");
	std::string src(std::istreambuf_iterator<char>(fcaDeviceFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(device[0]);

	cl::Program program(context, sources);

	program.build("-cl-std=CL1.2");
	
	//Carregando os dados do arquivo
	printf("\nCarregando contexto...");
  	CarregaContexto();

	//inicializando os buffers e parametros
	printf("\nIniciando Kernel...");
	cl::Buffer dadosBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * qtdObjetos, dadosROM, &err);
	cl::Buffer retornoBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * combAtributos, retKernel, &err);
	cl::Kernel kernel(program, "fca_device");

	err = kernel.setArg(0, dadosBuffer);
	err = kernel.setArg(1, retornoBuffer);
	err = kernel.setArg(2, qtdObjetos);
	err = kernel.setArg(3, combAtributos);

	cl::CommandQueue queue(context, device[0]);

	printf("\nExecutando...\n");
	
	//executando o kernel
	time_clock = clock();

	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(combAtributos));

	//lendo o resultado do kernel
	err = queue.enqueueReadBuffer(retornoBuffer, CL_TRUE, 0, sizeof(int)* combAtributos, retKernel);

	//contar a quantidade de conceitos
	for(int z=0; z<combAtributos; z++)
	{
		Conceitos = Conceitos + retKernel[z];
	}
	Conceitos += 2;
	time_clock = clock() - time_clock;

	printf("\n\nTotal de Conceitos: %d\n\n", Conceitos);

	printf("Tempo gasto: %f\n\n", ((float)time_clock) / CLOCKS_PER_SEC );
	//gravar em um arquivo o resultado
  	SalvarResultadoArquivo(((float)time_clock) / CLOCKS_PER_SEC, Conceitos);

	std::cin.get();

}

