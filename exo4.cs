using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace $safeprojectname$
{
    public class Client
    {
        public int Identifiant { get; set; }
        public string Nom { get; set; }
        public int Age { get; set; }
    }

    class ProgramCLient
    {
        public static void MainClient(string[] args)
        {
            List<Client> listeClients = new List<Client>
            {
                new Client {Identifiant=1, Nom="Nicolas", Age=30},
                new Client {Identifiant=2, Nom="Jeremie", Age=20},
                new Client {Identifiant=3, Nom="Delphine", Age=30},
                new Client {Identifiant=4, Nom="Bob", Age=10},
            };

            var clientQuery =
                from client in listeClients
                where (client.Age > 18)
                select client;

            foreach (Client client in clientQuery)
            {
                Console.WriteLine("nom. {0}  age. {1}", client.Nom, client.Age);
            }
        }
    }
}
