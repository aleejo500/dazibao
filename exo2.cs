using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace $safeprojectname$
{
    public struct Perso
    {
        public int Age { get; set; }
        public string Nom { get; set; }

        public void Person(int agePersonne, string nomPersonne)
        {
            //Your code goes here
            Age = agePersonne;
            Nom = nomPersonne;
        }
    }


    class ProgramPerso
    {
        public static void MainPerso(string[] args)
        {

            Perso p = new Perso();

            p.Person(10, "Thomas");

            Console.WriteLine("nom. {0}  age. {1}", p.Nom, p.Age);

        }
    }
}
