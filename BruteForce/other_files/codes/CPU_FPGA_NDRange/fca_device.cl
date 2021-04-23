__kernel void fca_device(__global int * restrict dadosROM, 
                          __global int * restrict retKernel, 
                          const int qtdObjetos, 
                          const int combAtributos) {
    
    //Declarações
    int RX = combAtributos;
    int i,j = 0;
    int filtro_id = get_global_id(0);

    if (filtro_id > 0 && filtro_id < RX)
    {
        for(i=0; i<qtdObjetos; i++) //Percorre todos os objetos da base (memória ROM)
        {        
            //verifica se os resultados do "AND" é igual ao filtro para entrar na RAM de objetos do próximo passo
            if((filtro_id & dadosROM[i]) == filtro_id) 
            {
                RX = RX & dadosROM[i];
            }
            else
            {
                j++;
            }
        }

        //Caso o resultado do AND bit a bit dfor igual ao Filtro de entrada, é um conceito, armazenando no vetor correspondente
        if((RX == filtro_id) && (j != qtdObjetos))
        {
	        retKernel[filtro_id]=1;
        }
    }
} 
