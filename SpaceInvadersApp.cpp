#include <wx/dcbuffer.h>
#include <wx/textfile.h>
#include <thread>
#include <wx/sound.h>
#include <wx/wx.h>
#include <wx/timer.h>
#include <direct.h>

#include "alien.h"
#include "explosion.h"
#include "alienschuss.h"
#include "schuss.h"
#include "spieler.h"
#include "spiel.h"
#include "objekt.h"

#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>



class BasicDrawPane;

class RenderTimer : public wxTimer
{
BasicDrawPane* pane;
public:
    RenderTimer(BasicDrawPane* pane);
    void Notify();
    void start(int timerzeit);
    void stop();
};

RenderTimer::RenderTimer(BasicDrawPane* pane) : wxTimer()
{
   RenderTimer::pane = pane;
}
void RenderTimer::start(int timerzeit)
{
    wxTimer::Start(timerzeit);
}

void RenderTimer::stop()
{
    wxTimer::Stop();
}
RenderTimer* timer;


wxBitmap bHintergrund,bRaumschiff,bSchuss,bAlienschuss,bAlien,bLeben,bExplosion;
sf::SoundBuffer bufferexplosion,bufferschuss;
sf::Sound Soundexplosion,Soundschuss;
sf::Music Hintergrundmusik;
bool mute=false;
sf::Ftp ftp;


explosion Explosion[10];
alienschuss Alienschuss[100];
alien Alien[40];
schuss Schuss[10];
spieler Spieler;
spiel Spiel;

void tastatureingaben()
{
    if (Spiel.isGameInForeground() && Spiel.isGameRunning())
    {

    if((wxGetKeyState((wxKeyCode)'a') || wxGetKeyState((wxKeyCode)'A') || wxGetKeyState(WXK_LEFT)) && (Spieler.getX()-5>=0) )
    {
        Spieler.bewegen(-2);

    }

    if ((wxGetKeyState((wxKeyCode)'d') || wxGetKeyState((wxKeyCode)'D') || wxGetKeyState(WXK_RIGHT)) && (Spieler.getX()+48<=Spiel.getBreite()))
    {
        Spieler.bewegen(+2);
    }

    if ((wxGetKeyState((wxKeyCode)' ') || wxGetKeyState((wxKeyCode)'w') || wxGetKeyState((wxKeyCode)'W') || wxGetKeyState(WXK_UP)) && (Spieler.ShootingAllowed()))
    {

        Spieler.schiessen(&Schuss[Spiel.getAnzahl().Schuss]);
        Spiel.setAnzahlSchuss(Spiel.getAnzahl().Schuss+1);


        if (!mute) Soundschuss.play();




    }

    }
}

void schussloeschen()
{
  for (int i=0;i<Spiel.getAnzahl().Schuss;i++)
        {
          if (Schuss[i].getY()<0)       ///Schuss oben raus
          {
               Spiel.objektLoeschen(Schuss,i,&Spiel,'s');
          }
        }

       for (int i=0;i<Spiel.getAnzahl().Alienschuss;i++)
        {
          if (Alienschuss[i].getY()>Spiel.getHoehe())       ///Alienschuss unten raus
          {
                Spiel.objektLoeschen(Alienschuss,i,&Spiel,'l');
          }
        }


}

void alienBewegen()
{
    for (int i=0; i<Spiel.getAnzahl().Alien;i++)
    {
        Alien[i].bewegenY(Spiel.getAlienGeschw().y,500);       ///bewegen
        Alien[i].bewegenX(Spiel.getBreite());
    }
}

void trefferregistrieren()
{
    ///Spieler Sch�sse -> Aliens
    for (int i=0; i<Spiel.getAnzahl().Schuss; i++)
    {
            int c=Schuss[i].trefferpruefen(Alien,&Explosion[Spiel.getAnzahl().Explosion],&Spiel, &Spieler);        ///Eigentliche Trefferanalyse
            if ( c != -1 )
            {
                Spiel.objektLoeschen(Schuss,i,&Spiel,'s');
                Spiel.objektLoeschen(Alien,c,&Spiel,'a');
                Spiel.aliensGeschwindigkeitErhoehen(Alien);
                i--;
                if (!mute) Soundexplosion.play();
            }

    }

    ///Aliensch�sse -> Spieler
    for (int i=0; i<Spiel.getAnzahl().Alienschuss; i++)
    {
        int c = Alienschuss[i].trefferpruefen(&Spieler,&Spiel);     ///Eigentliche Trefferanalyse
        if (c != -1)
        {
            Spiel.objektLoeschen(Alienschuss,i,&Spiel,'l');
            Spiel.spawnReinigen(Alienschuss);
            i--;
        }

    }


}

void alienschiessen()
{
    for (int i=0;i<Spiel.getAnzahl().Alien;i++)
    {
        if (Spiel.getAnzahl().Alienschuss<100)   ///Schiessen
        {
            if (Alien[i].schiessen(&Alienschuss[Spiel.getAnzahl().Alienschuss],Spiel.getschusswahrscheinlichkeit())==true)
            {Spiel.setAnzahlAlienschuss(Spiel.getAnzahl().Alienschuss+1);}
        }

    }
}

void endeerkennug()
{
    if (Spieler.getLeben()<=0 && Spiel.isGameRunning())
    {

        Spiel.stopGame();
        Spiel.highscore(&Spieler,&ftp);
        Spieler.setPunkte(0);
        Spiel.normalerunde(&Spieler,Alien);
        Spiel.writeLog("Game lost, because the Player has no life left");
    }

    if (Alien[Spiel.getAnzahl().Alien-1].getY()>=360 && Spiel.isGameRunning())
    {

        Spiel.stopGame();
        Spieler.setLeben(0);
        Spiel.highscore(&Spieler,&ftp);
        Spieler.setPunkte(0);
        timer->start(Spiel.getSpielgeschwindigkeit());
        Spiel.normalerunde(&Spieler,Alien);
        Spiel.writeLog("Game lost, because the Aliens reached the Earth");
    }

    if (Spiel.getAnzahl().Alien<=0)
    {
        Spiel.setAnzahlAlien(Spiel.getAliensProRunde());
        Spiel.normalerunde(&Spieler,Alien);
        Spiel.writeLog("one Wave finished");
    }


}

void explosionenentfernen()
{
    for (int i=0;i<Spiel.getAnzahl().Explosion;i++)
    {

        if (Explosion[i].ueberpruefen()==true)
        {
            Spiel.objektLoeschen(Explosion,i,&Spiel,'e');
            i--;
        }


    }
}

void schiessenerlauben()
{
    Spieler.schiessenerlauben(Spiel.getAnzahl().Schuss);

}

void schussbewegen()
{
    for (int i=0; i<Spiel.getAnzahl().Schuss;i++)
    {
        Schuss[i].bewegen(Spiel.getschussgeschwSpieler());
    }

    for (int i=0; i<Spiel.getAnzahl().Alienschuss;i++)
    {
        Alienschuss[i].bewegen(Spiel.getschussgeschwAliens());
    }
}




class BasicDrawPane : public wxPanel
{

public:
    BasicDrawPane(wxFrame* parent);

    void paintEvent(wxPaintEvent& evt);
    void paintNow();
    void render( wxDC& dc );

    DECLARE_EVENT_TABLE()
};

class MyFrame;

class MyApp: public wxApp
{

    bool OnInit();


    MyFrame* frame;
public:

};


IMPLEMENT_APP(MyApp)

class MyFrame : public wxFrame
{

    BasicDrawPane* drawPane;

public:
    MyFrame() : wxFrame((wxFrame *)NULL, -1,  wxT("Space Invaders"), wxPoint(50,50), wxSize(500,500),wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
    {


        drawPane = new BasicDrawPane( this );
        drawPane->Bind(wxEVT_CHAR_HOOK, &MyFrame::KeyDown, this);

        SetMinSize(GetSize());
        SetMaxSize(GetSize());

        SetIcon(wxICON(spaceinvadersicon));


        timer = new RenderTimer(drawPane);
        Show();
        timer->start(Spiel.getSpielgeschwindigkeit());

    }
    ~MyFrame()
    {
        delete timer;
    }


    void onClose(wxCloseEvent& evt)
    {
        timer->Stop();
        evt.Skip();
    }
    void KeyDown(wxKeyEvent& event)
    {
    if (Spiel.isGameInForeground())
    {


        if ((event.GetKeyCode()==80))           ///P        Pause
        {
            Spiel.writeLog(wxT("Player pressed P"));
           if (Spiel.isGameRunning()) {timer->Stop(); Spiel.stopGame();}
           else {timer->start(Spiel.getSpielgeschwindigkeit()); Spiel.resumeGame();}
        }

        if ((event.GetKeyCode()==82))           ///R        Neustart
        {
            Spiel.writeLog(wxT("Player pressed R"));
            Spiel.werteuebernehmen();
            Spieler.setPunkte(0);
            Spieler.allowShooting();
            Spieler.setLeben(Spiel.getlebenNEU());
            if (Spiel.isGameRunning()) timer->start(Spiel.getSpielgeschwindigkeit());
            Spiel.normalerunde(&Spieler,Alien);


        }

        if (event.GetKeyCode()==WXK_F1)         /// F1      Hilfe
        {
            Spiel.writeLog(wxT("Player pressed F1"));
        timer->stop();
        wxMessageBox("A / Pfeiltaste Links                          Raumschiff nach links bewegen \nD / Pfeiltaste Rechts                       Raumschiff nach rechts bewegen \nW / Leertaste / Pfeiltaste oben     schiessen\nR                                                        Neustart \nF1                                                       Hilfe \nH                                                        Highscore","Hilfe" ,wxICON_QUESTION);
        if (Spiel.isGameRunning()) timer->start(Spiel.getSpielgeschwindigkeit());
        }

        if (event.GetKeyCode()==72)            ///H        Highscore
        {
            Spiel.writeLog(wxT("Player pressed H"));
            timer->stop();
            Spiel.highscoreZeigen(&ftp);
            if (Spiel.isGameRunning()) timer->start(Spiel.getSpielgeschwindigkeit());
        }

        if ((event.GetKeyCode()==27))           ///ESC      Beenden
        {
           Close();
        }

        if (event.GetKeyCode()==69)         ///E        Einstellungen
        {
            Spiel.writeLog(wxT("Player pressed E"));
            timer->stop();

            Spiel.einstellungen();

            if (Spiel.isGameRunning()) timer->start(Spiel.getSpielgeschwindigkeit());
        }

        if (event.GetKeyCode()==77)
        {
            Spiel.writeLog(wxT("Player pressed M"));
           if (!mute) {mute=true;} else {mute=false;}
           if (mute) {Hintergrundmusik.stop();} else {Hintergrundmusik.play();}
        }

    }

        event.Skip();
    }

    DECLARE_EVENT_TABLE()
};



BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_CLOSE(MyFrame::onClose)
EVT_KEY_DOWN(MyFrame::KeyDown)
END_EVENT_TABLE()


void loadAllFiles()
{
    wxInitAllImageHandlers();
    bHintergrund.LoadFile("Images\\Hintergrund.png",wxBITMAP_TYPE_PNG);
    bRaumschiff.LoadFile("Images\\Raumschiff.png",wxBITMAP_TYPE_PNG);
    bSchuss.LoadFile("Images\\Munition.png",wxBITMAP_TYPE_PNG);
    bAlienschuss.LoadFile("Images\\Alienmunition.png",wxBITMAP_TYPE_PNG);
    bAlien.LoadFile("Images\\Alien.png",wxBITMAP_TYPE_PNG);                     ///Alle Bilder werden geladen
    bLeben.LoadFile("Images\\Leben.png",wxBITMAP_TYPE_PNG);
    bExplosion.LoadFile("Images\\Explosion.png",wxBITMAP_TYPE_PNG);

    Hintergrundmusik.openFromFile("music\\SpaceInvadersSoundtrack.wav");
    bufferexplosion.loadFromFile("music\\explosion.wav");
    bufferschuss.loadFromFile("music\\laser.wav" );

    Soundexplosion.setBuffer(bufferexplosion);
    Soundschuss.setBuffer(bufferschuss);

    Soundexplosion.setVolume(10);
    Soundschuss.setVolume(10);

    Hintergrundmusik.setLoop(true);
    Hintergrundmusik.play();

    _mkdir("Highscore");
    ftp.connect("staacraft.square7.ch", 21, sf::seconds(2));
    ftp.login("staacraft_SpaceInvaders", "1324");
    //ftp.login();
    ftp.download("HighscoreOnline.txt", "Highscore", sf::Ftp::Ascii);

    Spiel.writeLog("all Files succesfully loaded");


}

bool MyApp::OnInit()
{
    Spiel.writeLog("Game has started");

    std::srand(std::time(0));   ///Zufallszahlen generieren


        ///Bilder+Musik+HighscoreOnline Laden
     std::thread tLoadAllFiles(loadAllFiles);


    frame = new MyFrame();             ///Fenster wird erstellt
    frame->Show();                      ///Fenster wird angezeigt


    Spiel.saveForegroundWindow();                            ///Speichert (hoffentlich) SpaceInvaders, damit Tastatureingaben nur
                                                            ///funktionieren, wenn SpaceInvaders im Vordergrund ist

    tLoadAllFiles.join();

    return true;
}




BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
EVT_PAINT(BasicDrawPane::paintEvent)
END_EVENT_TABLE()




void RenderTimer::Notify()
{


   if (Spiel.isGameRunning())
   {
    std::thread t0(schussbewegen);
    std::thread t1(schiessenerlauben);
    std::thread t2(tastatureingaben);               ///Startet alle Funktionen als Threads
    std::thread t3(schussloeschen);                 ///Threads k�nnen gleichzeitig ablaufen
    std::thread t4(trefferregistrieren);
    std::thread t5(explosionenentfernen);
    std::thread t6(alienBewegen);

                ///Bis hier laufen alle Funktionen parallel
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();                                  ///Hier wird auf alle Funktionen gewartet
    t5.join();
    t6.join();

    endeerkennug();     ///bei den zwei gab es Probleme als Threads
    alienschiessen();

    pane->Refresh();       ///Das Bild wird neu gemalt
   }
}



BasicDrawPane::BasicDrawPane(wxFrame* parent) :
wxPanel(parent)
{
}


void BasicDrawPane::paintEvent(wxPaintEvent& evt)
{

    wxBufferedPaintDC dc(this);
    render(dc);
}


void BasicDrawPane::render( wxDC& dc )
{


    ///Rendering w�hrend das Spiel l�uft

    if (Spiel.isGameRunning())
    {

        SetBackgroundStyle(wxBG_STYLE_PAINT);

        if (bHintergrund.IsOk())
        {
            dc.DrawBitmap(bHintergrund,0,0);

        }

        if (bSchuss.IsOk())
        {
            for (int i=0; i<Spiel.getAnzahl().Schuss; i++)
            {
                dc.DrawBitmap(bSchuss,Schuss[i].getX(),Schuss[i].getY());
            }
        }

        if (bSchuss.IsOk())
        {
            for (int i=0; i<Spiel.getAnzahl().Alienschuss; i++)
            {
                dc.DrawBitmap(bAlienschuss,Alienschuss[i].getX(),Alienschuss[i].getY());
            }
        }


        if (bRaumschiff.IsOk())
        {
            dc.DrawBitmap(bRaumschiff,Spieler.getX(),Spieler.getY());
        }


        if (bAlien.IsOk())
        {
            for (int i=0; i<Spiel.getAnzahl().Alien; i++)
            {
                dc.DrawBitmap(bAlien,Alien[i].getX(),Alien[i].getY());
            }

            for (int i=1; i<=Spieler.getLeben(); i++)
            {
                dc.DrawBitmap(bLeben,i*50-40,430);
            }



        }

        ///Punktestand
        dc.SetTextForeground( *wxRED );
        dc.SetFont(wxFontInfo(12).FaceName("Distant Galaxy").Light());

        wxString punktstand="Punkte: ";
        punktstand << Spieler.getPunkte();

        dc.DrawText(punktstand, 380, 10);



        if (bExplosion.IsOk())
        {
            for (int i=0; i<Spiel.getAnzahl().Explosion; i++)
            {
                dc.DrawBitmap(bExplosion,Explosion[i].getX(),Explosion[i].getY());
            }
        }

    }

    ///Game Over
 if (!Spiel.isGameRunning() && Spieler.getLeben()!=-100)
 {
    dc.SetTextForeground( *wxRED );
    dc.SetFont(wxFontInfo(28).FaceName("Distant Galaxy").Light());
    dc.DrawText(wxT("Du hast Verloren"), 50, 200);
    dc.SetFont(wxFontInfo(15).FaceName("Distant Galaxy").Light());
    dc.DrawText(wxT("Neustart mit R"), 150, 250);

 }

 ///Spielstart Anzeige
 if (Spieler.getLeben()==-100)
 {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    dc.SetTextForeground( *wxRED );
    dc.SetFont(wxFontInfo(40).FaceName("Distant Galaxy").Light());
    dc.DrawText(wxT("Space Invaders"), 20, 200);
    dc.SetFont(wxFontInfo(20).FaceName("Distant Galaxy").Light());
    dc.DrawText(wxT("Start mit R"), 160, 280);
 }


}
