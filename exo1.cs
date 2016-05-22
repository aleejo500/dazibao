using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace $safeprojectname$
{
    public class Personne
    {
        public string Nom { get; set; }
        public string Prenom { get; set; }

        public virtual void SePresenter()
        {
            Console.WriteLine("Je m'appelle M/Mme. {0}  {1}", Nom, Prenom);
        }
    }

    public class Avocat : Personne
    {
        public override void SePresenter()
        {
            Console.WriteLine("Je m'appelle Maître. {0}  {1}", Nom, Prenom);
        }
    }

    class ProgramHeritage
    {
        static void MainHeritage(string[] args)
        {
            Avocat a = new Avocat { Nom = "Jean", Prenom = "Martin" };
            Personne p = new Personne { Nom = "Romain", Prenom = "Robert" };
            Personne ap = new Avocat { Nom = "Delphine", Prenom = "Lu" };
            a.SePresenter();
            p.SePresenter();
            ap.SePresenter();
        }
    }
}
