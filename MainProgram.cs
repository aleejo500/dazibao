using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace $safeprojectname$
{
    class Program
    {
        public static void Main(string[] args)
        {
            /*** Exo Heritage ***/
            Avocat a = new Avocat { Nom = "Jean", Prenom = "Martin" };
            Personne p = new Personne { Nom = "Romain", Prenom = "Robert" };
            Personne ap = new Avocat { Nom = "Delphine", Prenom = "Lu" };
            a.SePresenter();
            p.SePresenter();
            ap.SePresenter();

            /*** Exo Personne ***/
            Perso pe = new Perso();
            pe.Person(10, "Thomas");
            Console.WriteLine("nom. {0}  age. {1}", pe.Nom, pe.Age);


            /*** Exo Dossier ***/
            Dossier[] listDocs = new Dossier[]{
				new Dossier{ID=1,Encours=400000,},
				new Dossier{ID=2,Encours=1000,},
				new Dossier{ID=3,Encours=2000,},
				new Dossier{ID=4,Encours=20,},
            };

            // tri croissant
            Array.Sort(listDocs);
            foreach (Dossier d in listDocs)
            {
                Console.WriteLine(d.Encours);
            }


            /*** Exo Client ***/
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
