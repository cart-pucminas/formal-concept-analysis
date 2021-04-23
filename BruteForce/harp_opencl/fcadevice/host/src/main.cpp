/*
 * Meant to be used as template for new applications...
 * Recommended to refer to OpenCL at Khronos:
 *    https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/
 * Recommended to refer to Intel Altera OpenCL SDK for FPGA Programming Guide
 * Recommended to refer to Intel Altera OpenCL SDK for FPGA Best Practices Guide 
 * 
 * Template HOST to call simple kernel.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>
#include "CL/opencl.h"
#include "AOCLUtils/aocl_utils.h"
#include <unistd.h>
#include <string.h>

using namespace aocl_utils;

// OpenCL runtime configuration
static cl_platform_id platform = NULL;
static cl_device_id device = NULL;
static cl_context context = NULL;
static cl_command_queue queue = NULL;
static cl_kernel kernel = NULL;
static cl_program program = NULL;

cl_int status;

// Function prototypes
bool init();
void cleanup();

//------------------------------------------------------------------
int *dadosROM;
int *retKernel;
int *RamObjetos;
int filtro;

//Necessário definir quantidade de objetos e combinações dos atributos
//int qtdObjetos = 256; //total de objetos do arquivo
//int combAtributos = 4095; //Quantidade total de combinações

//int qtdObjetos = 8192; //total de objetos do arquivo
//int combAtributos = 65535; //Quantidade total de combinações

//int qtdObjetos = 1000; //total de objetos do arquivo
//int combAtributos = 33554431; //Quantidade total de combinações

int qtdObjetos = 8192; //total de objetos do arquivo
int combAtributos = 2048; //65536 / 32
int kernelRX = 65535; //65536 / 32

int indiceInicial = 0;
int quantIndiceInicial = 32;


cl_mem dadosBuffer, retornoBuffer, RamObjetosBuffer;

//------------------------------------------------------------------


void CreateKernel(void) {
	kernel = clCreateKernel(program, "fca_device" , &status);
	checkError(status, "Failed to create kernel");
}

void FinalizarKernel(){
  status = clFinish(queue);  // Wait the queue
  checkError(status, "Failed to finish");
  printf("\nKernel execution is complete.\n");
}

void CarregaContexto(){
  dadosROM = (int*)malloc(sizeof(int) * qtdObjetos);
  retKernel = (int*)malloc(sizeof(int) * combAtributos);

  for (int w = 0; w < combAtributos; w++)
  {
    retKernel[w] = 0;
  }
  char dir[200];

  strcpy(dir, get_current_dir_name());

  int aux = strlen(dir);
  char Linha[25];
  char *result;

  strcat(dir, "/baseTeste.txt");

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
    }
    i++;
  }
  fclose(arq);
}

void StartKernel(){
  //criando os buffers
  dadosBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                sizeof(int) * qtdObjetos, dadosROM, &status);
  retornoBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                  sizeof(int) * combAtributos, retKernel, &status); //buffer de 1 (apenas para retorno)
  
  //escrevendo o contexto no buffer
  status  = clEnqueueWriteBuffer(queue, dadosBuffer, CL_FALSE, 0, 
            sizeof(int) * qtdObjetos, dadosROM, 0, NULL, NULL);
  checkError(status, "Falhou ao enfileirar buffer");

  //set nas variáveis do kernel
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dadosBuffer);
  status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &retornoBuffer);
  status |= clSetKernelArg(kernel, 2, sizeof(int), &qtdObjetos);
  status |= clSetKernelArg(kernel, 3, sizeof(int), &kernelRX);
  status |= clSetKernelArg(kernel, 4, sizeof(int), &indiceInicial);

  checkError(status, "Falhou ao setar argumentos");

  status |= clFinish(queue);
	checkError(status, "Falhou ao finalizar");
}

void ExecutarKernel(int value){
  size_t size[3] = { combAtributos , 1 , 1 };

  indiceInicial = value;
  status |= clSetKernelArg(kernel, 4, sizeof(int), &indiceInicial);

  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, size, &size[0], 0, NULL, NULL);
  checkError(status, "Falha ao executar o kernel");
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

// Entry point.
int main() {
  
  if(!init()) {
    return -1;
  }

  printf("\n\nCriando Kernel...");
  CreateKernel();

  printf("\nCarregando contexto...");
  CarregaContexto();

  printf("\nIniciando Kernel...");
  StartKernel();

  int Conceitos = 0;
  double start_time, end_time;

  printf("\nExecutando Kernel...\n");
  start_time = getCurrentTimestamp();

  for(int w=0; w<quantIndiceInicial; w++)
  {
    ExecutarKernel(w*combAtributos);
  }

  end_time += getCurrentTimestamp() - start_time;
  printf("\n\nTempo de execução: %lf", end_time);
  
  status |= clEnqueueReadBuffer(queue, retornoBuffer, CL_TRUE, 0, sizeof(int) * combAtributos, retKernel, 0, NULL, NULL);
  checkError(status, "Falha ao ler os Buffers");

  for(int z=0; z<combAtributos; z++)
  {
    Conceitos = Conceitos + retKernel[z];
  }

  Conceitos += 2;
  printf("\n\nTotal de Conceitos: %d\n\n", Conceitos);

  SalvarResultadoArquivo(end_time, Conceitos);

  FinalizarKernel();

  // Free
  cleanup();

  return 0;
}

bool init() {
  cl_int status;

  if(!setCwdToExeDir()) {
    return false;
  }

  // Get the OpenCL platform. TODO: More than one platform can be found
  cl_uint num_platforms;
  status = clGetPlatformIDs(1, &platform, &num_platforms);
  checkError(status, "Failed clGetPlatformIDs.");

  // Query the available OpenCL devices.
  scoped_array<cl_device_id> devices;
  cl_uint num_devices;
  devices.reset(getDevices(platform, CL_DEVICE_TYPE_ALL, &num_devices));
  device = devices[0]; // We'll just use the first device.

  // Create the context.
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
  checkError(status, "Failed to create context");

  // Create the command queue.
  queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
  checkError(status, "Failed to create command queue");

  // Create the program, using the name of aocx file
  std::string binary_file = getBoardBinaryFile("fca_device", device);
  program = createProgramFromBinary(context, binary_file.c_str(), &device, 1);

  // Build the program that was just created.
  status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
  checkError(status, "Failed to build program");

  return true;
}

// Free the resources allocated during initialization
void cleanup() {
  if(kernel) {
    clReleaseKernel(kernel);  
  }
  if(program) {
    clReleaseProgram(program);
  }
  if(queue) {
    clReleaseCommandQueue(queue);
  }
  if(context) {
    clReleaseContext(context);
  }
}
