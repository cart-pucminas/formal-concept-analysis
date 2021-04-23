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
int qtdObjetos = 8192; //total de objetos do arquivo
int combAtributos = 65535; //2^16

//size_t global_work[3] = { combAtributos , 1 , 1 };

cl_mem dadosBuffer, retornoBuffer, RamObjetosBuffer;

//size_t global_work[3] = { qtdObjetos , 1 , 1 };
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
  retKernel = (int*)malloc(sizeof(int));
  RamObjetos = (int*)malloc(sizeof(int) * qtdObjetos);

  retKernel[0] = 0;
  int w;
  for(w=0; w < qtdObjetos; w++){
    RamObjetos[w]=0;
  }

  
  char dir[200];

  strcpy(dir, get_current_dir_name());

  int aux = strlen(dir);
  char Linha[25];
  char *result;

  //dir[aux-1] = '\0';
  //dir[aux-2] = '\0';
  //dir[aux-3] = '\0';

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

void StartKernel(){
  //criando os buffers
  dadosBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                sizeof(int) * qtdObjetos, dadosROM, &status);
  retornoBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                  sizeof(int), retKernel, &status); //buffer de 1 (apenas para retorno)
  RamObjetosBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                sizeof(int) * qtdObjetos, RamObjetos, &status);
  
  //escrevendo o contexto no buffer
  status  = clEnqueueWriteBuffer(queue, dadosBuffer, CL_FALSE, 0, 
            sizeof(int) * qtdObjetos, dadosROM, 0, NULL, NULL);
  status  |= clEnqueueWriteBuffer(queue, RamObjetosBuffer, CL_FALSE, 0, 
            sizeof(int) * qtdObjetos, RamObjetos, 0, NULL, NULL);

  //set nas variáveis do kernel
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dadosBuffer);
  status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &retornoBuffer);
  status |= clSetKernelArg(kernel, 2, sizeof(int), &qtdObjetos);
  status |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &RamObjetosBuffer);
  status |= clSetKernelArg(kernel, 4, sizeof(int), &combAtributos);
  
  status |= clFinish(queue);
	checkError(status, "Failed to set arguments");
}

int ExecutarKernel(){
  //conjunto de atributos que precisa ser verificado se é conceito
  //filtro = valFiltro;
  //status |= clSetKernelArg(kernel, 3, sizeof(int), &filtro);
 
  size_t size[3] = { 1 , 1 , 1 };     
  status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &size[0], &size[1], 0, NULL, NULL);
  checkError(status, "Falha ao executar o kernel");

  status = clEnqueueReadBuffer(queue, dadosBuffer, CL_TRUE, 0, sizeof(int) * qtdObjetos, dadosROM, 0, NULL, NULL);
  status |= clEnqueueReadBuffer(queue, retornoBuffer, CL_TRUE, 0, sizeof(int), retKernel, 0, NULL, NULL);
  checkError(status, "Falha ao ler os Buffers");
  
  return retKernel[0];
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

  Conceitos = ExecutarKernel();
//   for (int i = 1; i < combAtributos - 1; i++)
//   {
//     if(ExecutarKernel(i) == true){
//       Conceitos++;
//     }
//   }

  end_time += getCurrentTimestamp() - start_time;
  printf("\n\nTempo de execução: %lf", end_time);
  
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
