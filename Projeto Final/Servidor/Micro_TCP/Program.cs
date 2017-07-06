using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Micro_TCP
{
    class Program
    {
        static byte[] ID1 = { 0x45, 0x76, 0xC3, 0x76, 0x86 };
        static List<byte[]> IDs = new List<byte[]>()
        {
            new byte [] { 0x45, 0x76, 0xC3, 0x76, 0x86 },
            new byte [] { 0x4D, 0xFC, 0x8B, 0xAB, 0x91 },
        };
        static void Main(string[] args)
        {
            Console.WriteLine("Iniciando Servidor");

            new Thread(Server).Start();

            Console.ReadKey();
        }

        static void Server()
        {
            TcpListener server = new TcpListener(IPAddress.Any, 65410);
            server.Start();
            Console.WriteLine("Servidor iniciado");
            while (true)
                ClientConnet(server.AcceptSocket());
        }

        static void ClientConnet(Socket client)
        {
            NetworkStream stream = new NetworkStream(client);
            Console.WriteLine("Client conectado");
            byte[] buffer = new byte[5];
            char result = '0';
            while (true)
            {
                if (stream.Read(buffer, 0, 5) == 0) break;
                string RFID = BitConverter.ToString(buffer);
                Console.WriteLine("RFID Recebido:" + RFID);
                bool IdCadastrado = IDs.Any(p =>
                   p[0] == buffer[0] &&
                   p[1] == buffer[1] &&
                   p[2] == buffer[2] &&
                   p[3] == buffer[3] &&
                   p[4] == buffer[4]);
                if (IdCadastrado)
                {
                    result = '1';
                    Console.WriteLine("ID Autorizado");
                }
                else
                {
                    result = '0';
                    Console.WriteLine("ID Não autorizado");
                }
                stream.WriteByte(Convert.ToByte(result));
            }
        }
    }
}
