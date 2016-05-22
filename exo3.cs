using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace $safeprojectname$
{
    public class Dossier : IComparable<Dossier>
    {
        public int ID { get; set; }
        public double Encours { get; set; }

        public int CompareTo(Dossier that)
        {
            if (this.Encours == that.Encours) return 0;
            if (this.Encours < that.Encours) return -1;
            return 1;
        }
    }

    class ProgramDossier
    {
        public static void MainDossier(string[] args)
        {

            Dossier[] listDocs=new Dossier[]{
				new Dossier{ID=1,Encours=400000,},
				new Dossier{ID=2,Encours=1000,},
				new Dossier{ID=3,Encours=2000,},
				new Dossier{ID=4,Encours=20,},
            };

			// tri croissant
			Array.Sort(listDocs);
			                
			foreach(Dossier d in listDocs){
				Console.WriteLine(d.Encours);
			}
			
		}
    }
}
