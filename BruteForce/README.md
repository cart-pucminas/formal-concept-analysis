# IntelHARP
Projeto intel

jppnovais@ssh-iam.intel-research.net

scp harp_opencl-master jppnovais@ssh-iam.intel-research.net:/homes/jppnovais/projetoIntel

source /export/fpga/bin/setup-fpga-env fpga-bdx-opencl

qsub-fpga

make

//criar binfile
qsub-aoc fca_device.cl



#Tutorial: 

https://wiki.intel-research.net/FPGA.html#config-label

#Docs para openCL: 

https://www.intel.com/content/www/us/en/programmable/documentation/mwh1391807516407.html

#Docs para execução de RTL: 

https://wiki.intel-research.net/FPGA.html?highlight=rtl#aal-section-label


#Compilar código em c++ com openMP: 

g++ nomeCodigo.cpp -fopenmp

#Executar arquivo .sh em segundo plano, colocando a saída em um arquivo txt: 

nohup bash arquivo.sh > saida.txt & #

#Monitorar meus processos: 

ps aux | grep jppnova #

#Comandos executados para GPU no linux

#exportando bibliotecas
export PATH=$PATH:/usr/local/cuda-10.2/bin
#export PATH=$PATH:/usr/local/cuda-10.2/lib64
#export PATH=$PATH:/usr/local/cuda-10.2/include
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda-10.2/lib64

#compilando c�digo
g++ main.cpp -o main -I/usr/local/cuda-10.2/include -L/usr/local/cuda-10.2/lib64 -lOpenCL


#Para converter uma base de dados SCGaz em uma reconhecida pelo projeto de FCA, basta utilizar o exe gerado no projeto "ConverterBanco"