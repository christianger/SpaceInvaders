#include "spiel.h"
#include "Spieler.h"
#include "alien.h"
#include <wx/textfile.h>
#include "alienschuss.h"
#include "schuss.h"
#include "explosion.h"
#include <direct.h>

spiel::spiel()
{

}

spiel::~spiel()
{
    //dtor
}

bool spiel::isGameInForeground()
{
    return (fensterImVordergrund==GetForegroundWindow());
}

void spiel::addPunkte(spieler* Spieler)
{
    Spieler->addPunkte(Spieler->getPunkte()+(geschwAlien.x+geschwAlien.y+schusswahrscheinlichkeit-lebenPUNKTE-schussgeschwSpieler+schussgeschwAliens));
}


void spiel::werteuebernehmen()
{
            schussgeschwSpieler=schussgeschwSpielerNEU;
            geschwAlien.x=geschwXNEU;
            geschwAlien.y=geschwYNEU;
            schusswahrscheinlichkeit=schusswahrscheinlichkeitNEU;
            anzahlAlienNEU=20;
            Anzahl.Alien=anzahlAlienNEU;
            spiellaeuft=true;
            Anzahl.Schuss=0;
            lebenPUNKTE=lebenNEU;
}

void spiel::normalerunde(spieler* Spieler, alien Alien[])
{


    Anzahl.Alienschuss=0;
    Anzahl.Explosion=0;
    Anzahl.Schuss=0;
    Spieler->setX(250-16);


        int spaltealien=0;
        int reihealien=1;

        for (int i=0;i<Anzahl.Alien;i++)
        {
           spaltealien++;

           if (spaltealien>10)
           {    spaltealien=1;
                reihealien++;   }                               ///Aliens hinzuf�gen

        Alien[i].reseten(spaltealien*40,reihealien*40);


        }


}

void spiel::aliensGeschwindigkeitErhoehen(alien Alien[])
{
    if (Anzahl.Alien %5 ==0)
    {
        for (int i=0; i<Anzahl.Alien; i++)
        {
            Alien[i].speedUp();
        }
    }
}

void spiel::spawnReinigen(alienschuss Alienschuss[])
{

            ///Sch�sse beim Spawnpunkt entfernen
            for (int f=0; f<Anzahl.Alienschuss; f++)
            {
                if ((Alienschuss[f].getX()>220) && (Alienschuss[f].getX()<280) && (Alienschuss[f].getY()>250))
                {

                    for (int e=f; e<Anzahl.Alienschuss-1; e++)
                    {
                        Alienschuss[e]=Alienschuss[e+1];
                    }
                    Anzahl.Alienschuss--;
                    f--;
                }
            }
}

void spiel::highscore(spieler* Spieler)
{
    bool hso=false,hs=false,chanceled=false;
    mkdir("Highscore");                                ///erstellt nur, falls nicht vorhanden
    wxTextFile highscoreTXT( wxT("Highscore/Highscore.txt") );

    highscoreTXT.Create("Highscore/Highscore.txt");             ///erstellt nur, falls nicht vorhanden
    highscoreTXT.Open("Highscore/Highscore.txt");




    ///Highscore
    int platz=0;
    wxString highscore,tmp;

    tmp = highscoreTXT.GetFirstLine();

    for (int i=0; i<10; i++)
    {
        if (tmp[i] == ' ')
        {
          for (int c=0;c<i;c++)
          {
             highscore=highscore+tmp[c];
          }
          break;
        }
    }


    do {


        if ((Spieler->getPunkte()>wxAtoi(highscore) || highscore=="")    && !hs && Spieler->getPunkte()>0)
            {


            wxTextEntryDialog *dlg = new wxTextEntryDialog((wxFrame *)NULL,wxT("Gib bitte deinen Namen ein"),wxT("Highscore"));
                if ( dlg->ShowModal() == wxID_OK )
                {
                    dlg->Destroy();
                    wxString punktstand;
                    punktstand << Spieler->getPunkte();



                        if (Spieler->getPunkte()<100)
                        {
                        punktstand=punktstand+" ";
                        }
                        if (Spieler->getPunkte()<1000)
                        {
                        punktstand=punktstand+" ";
                        }
                        Spieler->setName(dlg->GetValue());
                    punktstand=punktstand+"   "+Spieler->getName();
                    highscoreTXT.InsertLine( punktstand, platz);
                    highscoreTXT.Write();
                    hs=true;


                }
                else {hs=true;chanceled=true;}

            dlg->Destroy();


            break;
            }
        highscore="";
        tmp = highscoreTXT.GetNextLine();
        for (int i=0; i<10; i++)
    {

        if (tmp[i] == ' ')
        {
          for (int c=0;c<i;c++)
          {
             highscore=highscore+tmp[c];
          }
          break;
        }
    }
        platz++;

    } while(!highscoreTXT.Eof() && platz<10);

    if (platz<10 && !hs && Spieler->getPunkte()>0)
    {

            wxTextEntryDialog *dlg = new wxTextEntryDialog((wxFrame *)NULL,wxT("Gib bitte deinen Namen ein"),wxT("Highscore"));
                if ( dlg->ShowModal() == wxID_OK )
                {
                    dlg->Destroy();
            wxString punktstand;
            punktstand << Spieler->getPunkte();                   ///Punktzahl ganz am ende
            if (Spieler->getPunkte()<100)
            {
            punktstand=punktstand+" ";
            }
            if (Spieler->getPunkte()<1000)
            {
            punktstand=punktstand+" ";
            }
            Spieler->setName(dlg->GetValue());
             punktstand=punktstand+"   "+Spieler->getName();
            highscoreTXT.InsertLine( punktstand, platz);
            highscoreTXT.Write();
                } else {chanceled=true;}
                dlg->Destroy();



    }



for(int i=10;i<highscoreTXT.GetLineCount();i++)
{
    highscoreTXT.RemoveLine(i);
}


    highscoreTXT.Write();
    highscoreTXT.Close();







    ///Highscore Online

if (!chanceled){
    wxTextFile highscoreTXTOnline( wxT("Highscore/HighscoreOnline.txt") );
    highscoreTXTOnline.Create("Highscore/HighscoreOnline.txt");
    highscoreTXTOnline.Open("Highscore/HighscoreOnline.txt");



    ///Zeilen wieder formatieren





    platz=0;
    highscore="",tmp="";

    tmp = highscoreTXTOnline.GetFirstLine();

    for (int i=0; i<10; i++)
    {
        if (tmp[i] == ' ')
        {
          for (int c=0;c<i;c++)
          {
             highscore=highscore+tmp[c];
          }
          break;
        }
    }


    do {


        if ((Spieler->getPunkte()>wxAtoi(highscore) || highscore=="")    && !hso && Spieler->getPunkte()>0)
            {



                    wxString punktstand;
                    punktstand << Spieler->getPunkte();



                        if (Spieler->getPunkte()<100)
                        {
                        punktstand=punktstand+" ";
                        }
                        if (Spieler->getPunkte()<1000)
                        {
                        punktstand=punktstand+" ";
                        }
                    punktstand=punktstand+"   "+Spieler->getName();
                    highscoreTXTOnline.InsertLine( punktstand, platz);
                    highscoreTXTOnline.Write();
                    hso=true;

                    break;
                }






        highscore="";
        tmp = highscoreTXTOnline.GetNextLine();
        for (int i=0; i<10; i++)
    {

        if (tmp[i] == ' ')
        {
          for (int c=0;c<i;c++)
          {
             highscore=highscore+tmp[c];
          }
          break;
        }
    }
        platz++;

    } while(!highscoreTXTOnline.Eof() && platz<10);


    if (platz<10 && !hso && Spieler->getPunkte()>0)
    {
            wxString punktstand;
            punktstand << Spieler->getPunkte();                   ///Punktzahl ganz am ende
            if (Spieler->getPunkte()<100)
            {
            punktstand=punktstand+" ";
            }
            if (Spieler->getPunkte()<1000)
            {
            punktstand=punktstand+" ";
            }
             punktstand=punktstand+"   "+Spieler->getName();
            highscoreTXTOnline.InsertLine( punktstand, platz);
            highscoreTXTOnline.Write();
                }








for(int i=10;i<highscoreTXTOnline.GetLineCount();i++)
{
    highscoreTXTOnline.RemoveLine(i);
}


    highscoreTXTOnline.Write();
    highscoreTXTOnline.Close();

}

}

void spiel::einstellungen()
{
    wxTextEntryDialog *dlg = new wxTextEntryDialog((wxFrame *)NULL,wxT("Spielgeschwindigkeit (1-100)"),wxT("Einstellungen"),wxT("85"));
             if ( dlg->ShowModal() == wxID_OK )
                {

                 timerzeit = 100- wxAtoi(dlg->GetValue());
                }
                dlg->Destroy();


        wxTextEntryDialog *dlg1 = new wxTextEntryDialog((wxFrame *)NULL,wxT("Schwierigkeit (1-10)"),wxT("Einstellungen"),wxT("2"));
             if ( dlg1->ShowModal() == wxID_OK )
                {

                 int tmp =  wxAtoi(dlg1->GetValue());
                 if (tmp>0 && tmp<=10)
                 {
                     geschwXNEU=tmp/2;
                     geschwYNEU=tmp*5;
                 }

                }
                dlg1->Destroy();


                wxTextEntryDialog *dlg2 = new wxTextEntryDialog((wxFrame *)NULL,wxT("Schusswahrscheinlichkeit der Aliens (1-10)"),wxT("Einstellungen"),wxT("5"));
             if ( dlg2->ShowModal() == wxID_OK )
                {

                 int tmp =  wxAtoi(dlg2->GetValue());
                 if (tmp>0 && tmp<=10)
                 {
                    schusswahrscheinlichkeitNEU=tmp;
                 }

                }
                dlg2->Destroy();


                 wxTextEntryDialog *dlg3 = new wxTextEntryDialog((wxFrame *)NULL,wxT("Anzahl deiner Leben (1-10)"),wxT("Einstellungen"),wxT("3"));
             if ( dlg3->ShowModal() == wxID_OK )
                {

                 int tmp =  wxAtoi(dlg3->GetValue());
                 if (tmp>0 && tmp<=10)
                 {
                    lebenNEU=tmp;
                 }

                }
                dlg3->Destroy();

                 wxTextEntryDialog *dlg4 = new wxTextEntryDialog((wxFrame *)NULL,wxT("Geschwindigkeit der AlienSchuesse (1-10)"),wxT("Einstellungen"),wxT("2"));
             if ( dlg4->ShowModal() == wxID_OK )
                {

                 int tmp =  wxAtoi(dlg4->GetValue());
                 if (tmp>0 && tmp<=10)
                 {
                    schussgeschwAliens=tmp;
                 }

                }
                dlg4->Destroy();

                  wxTextEntryDialog *dlg5 = new wxTextEntryDialog((wxFrame *)NULL,wxT("Geschwindigkeit der Spielerschuesse (1-10)"),wxT("Einstellungen"),wxT("3"));
             if ( dlg5->ShowModal() == wxID_OK )
                {

                 int tmp =  wxAtoi(dlg5->GetValue());
                 if (tmp>0 && tmp<=10)
                 {
                    schussgeschwSpielerNEU=tmp;
                 }

                }
                dlg4->Destroy();



}

void spiel::highscoreZeigen()
{

        wxString tmp;
        _mkdir("Highscore");

        wxTextFile highscoreTXT( wxT("Highscore/Highscore.txt") );
        highscoreTXT.Create("Highscore/Highscore.txt");
        highscoreTXT.Open();

        wxTextFile highscoreTXTOnline( wxT("Highscore/HighscoreOnline.txt") );
        highscoreTXTOnline.Create("Highscore/HighscoreOnline.txt");
        highscoreTXTOnline.Open();

        tmp="Lokaler Highscore: \n";
        tmp=tmp+highscoreTXT.GetFirstLine();
        while (!highscoreTXT.Eof())
        {
          tmp=tmp+" \n "+highscoreTXT.GetNextLine();
        }
        tmp=tmp+"\n";

        tmp=tmp+"Globaler Highscore: \n";
        tmp=tmp+highscoreTXTOnline.GetFirstLine();
        while (!highscoreTXTOnline.Eof())
        {
          tmp=tmp+" \n "+highscoreTXTOnline.GetNextLine();
        }


        wxMessageBox( tmp,"Highscore" ,wxICON_INFORMATION);


        highscoreTXT.Close();
}

