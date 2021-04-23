using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConverterBanco
{
    class Program
    {
        static string caminho = @"D:\GitHub\IntelHARP\ConverterBanco\";

        private static int ConverterBinario(string value)
        {
            int retSoma = 0, z=0;

            for (int i = value.Length - 1; i >= 0; i--)
            {
                if(value[i] == '1')
                {
                    retSoma = retSoma + Convert.ToInt32(Math.Pow(2, z));
                }
                z++;
            }

            return retSoma;
        }

        static void Main(string[] args)
        {
            try
            {
                Console.Write("Digite o nome do arquivo cxt: ");
                var nome = Console.ReadLine();
                Console.WriteLine("Carregando...");
                caminho = System.AppDomain.CurrentDomain.BaseDirectory.ToString();

                string[] lines = System.IO.File.ReadAllLines(caminho + nome + ".cxt");

                int objetos = Convert.ToInt32(lines[2]);
                int atributos = Convert.ToInt32(lines[3]);

                int inicio = objetos + atributos + 5;
                string aux;

                using (StreamWriter writer = new StreamWriter(caminho + nome + "_Convertida.txt", true))
                {
                    for (int i = inicio; i < lines.Length; i++)
                    {
                        aux = lines[i].Replace(' ', '0');
                        aux = aux.Replace('X', '1');
                    
                        writer.WriteLine(ConverterBinario(aux));
                    }
                }

                Console.WriteLine("Concluido");
            }
            catch (Exception e)
            {
                Console.WriteLine("Erro: " + e.Message);
            }

            Console.WriteLine("Finalizado");
            Console.ReadKey();
        }
    }
}
