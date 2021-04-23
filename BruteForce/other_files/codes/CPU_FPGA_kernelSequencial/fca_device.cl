__kernel void fca_device(__global int* restrict dadosROM, 
                          __global int* restrict retKernel, 
                          const int qtdObjetos, 
                          __global int* restrict RamObjetos, 
                          const int combAtributos) {
    
    int RX;
    //Declarações
    int i,j, indice;

    for(indice=1; indice<combAtributos; indice++)
    {
        RX = combAtributos;
        j=0;

        ///BC IDA
        for(i=0; i<qtdObjetos; i++) //Percorre todos os objetos da base (memória ROM)
        {        
            RamObjetos[i] = 0; 
            //verifica se os resultados do "AND" é igual ao filtro para entrar na 
            //RAM de objetos do próximo passo
            if((indice & dadosROM[i]) == indice) 
            {
                RamObjetos[i] = dadosROM[i];
            }
            else
            {
                //coloca um dado vazio caso nao encontre
                RamObjetos[i] = 0;
                j++;
            }
        }
        ///BC Volta
        for(i=0; i<qtdObjetos; i++) //Percorre todos os objetos da base (Memória ROM)
        {
            //verifica se o objeto da ROM está vazio
            if(RamObjetos[i] != 0)
            {
                RX = RX & RamObjetos[i];
            }
        }
        //barrier(CLK_GLOBAL_MEM_FENCE);
        //Caso o resultado do AND bit a bit do BC_Volta for igual ao Filtro de entrada, 
        //é um conceito, e deve-se retornar o conceito, ou uma flag
        if((RX == indice) && (j != qtdObjetos)){
            retKernel[0]++;
            //Saída = RX, Filtro ou um flag
        }
    }
} 
