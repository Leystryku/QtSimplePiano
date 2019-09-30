#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtMultimedia/QMediaPlayer>
#include <sstream>
#include <ostream>


#include <QKeyEvent>

Ui::MainWindow *win = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    win = ui;//ui extern lagern weil, brauchen es außerhalb einer Ui-Klassenfunktion
    ui->setupUi(this);
    keyEnterReceiver *key = new keyEnterReceiver();//custom klass, keyEnterReceiver erstellen

    ui->centralWidget->installEventFilter(key);//installiere ihren event filter
}

MainWindow::~MainWindow()
{
    delete ui;
}


std::string TranslateToNoteName(uint8_t key)//funktion um von nummern auf notennamen zu kommen
{

    key += 20;

    int octave = (key / 12) - 1;

    std::string notes = "C DbD EbE F GbG AbA BbB ";
    std::string note = "";
    note = notes;
    note = note.substr(((key % 12) * 2), ((key % 12) * 2 + 2) - ((key % 12) * 2));


    std::ostringstream ostr;
    ostr << octave;

    if (note[note.length() - 1] == ' ')
    {
        note.erase(note.length() - 1);
    }

    note.append(ostr.str());


    return note;
}

QMediaPlayer *players[88] = {0}; // 88 mediaplayer für 88 tasten

void PlayButtonSound(int buttonnum) // funktion um knöpfe abzuspieen
{


    char sound[0xFF];
    sprintf(sound, "sounds/Keys/%s.wav", TranslateToNoteName(buttonnum).c_str()); // übersetze die tasten

     qDebug() << "Playing: " << sound;
    if(!players[buttonnum]) // falls es kein player gibt
        {
        players[buttonnum] = new QMediaPlayer; // mediaplayer erstellen
        players[buttonnum]->setMedia(QUrl::fromLocalFile(sound)); // lade die Datei von unserer Festplatte
        players[buttonnum]->setVolume(100);//lautstärke normal
    }else{
        players[buttonnum]->stop();//falls es einen gibt, stop, sonst buggen sie
    }

    players[buttonnum]->play();//abspielen

}


bool keysshifted = false;//falls war dann werden schwarze tasten gespielt
bool keyson = false;

int keys[0xFFF] = {0}; //mapping für normale tasten
int keys_black[0xFFF] = {0}; // mapping für schwarze tasten

int keys_cur = 0;
int keys_blackcur= 0;
void AddKey(int key) // funktion um tasten zur map hinzuzufügen
{
    keys_cur+=1;
    keys[keys_cur] = key;
}

void AddBlackKey(int key) // funktion um tasten zur map für die black keys hinzuzufügen
{
    keys_blackcur += 1;
    keys_black[keys_blackcur] = key;
}

void InitKeys()
{

    //diese tasten werden sequentiell verwendet
    AddKey(Qt::Key_1);
    AddKey(Qt::Key_2);
    AddKey(Qt::Key_3);
    AddKey(Qt::Key_4);
    AddKey(Qt::Key_5);
    AddKey(Qt::Key_6);
    AddKey(Qt::Key_7);
    AddKey(Qt::Key_8);
    AddKey(Qt::Key_9);
    AddKey(Qt::Key_0);
    AddKey(Qt::Key_Q);
    AddKey(Qt::Key_W);
    AddKey(Qt::Key_E);
    AddKey(Qt::Key_R);
    AddKey(Qt::Key_T);
    AddKey(Qt::Key_Z);
    AddKey(Qt::Key_U);
    AddKey(Qt::Key_I);
    AddKey(Qt::Key_O);
    AddKey(Qt::Key_P);
    AddKey(Qt::Key_A);
    AddKey(Qt::Key_S);
    AddKey(Qt::Key_D);
    AddKey(Qt::Key_F);
    AddKey(Qt::Key_G);
    AddKey(Qt::Key_H);
    AddKey(Qt::Key_J);
    AddKey(Qt::Key_K);
    AddKey(Qt::Key_L);
    AddKey(Qt::Key_X);
    AddKey(Qt::Key_Y);
    AddKey(Qt::Key_C);
    AddKey(Qt::Key_V);
    AddKey(Qt::Key_B);
    AddKey(Qt::Key_N);
    AddKey(Qt::Key_M);

    //sequentiell die schwarzen tasten
    AddBlackKey(Qt::Key_1);
    AddBlackKey(Qt::Key_2);

    AddBlackKey(Qt::Key_4);
    AddBlackKey(Qt::Key_5);
    AddBlackKey(Qt::Key_6);

    AddBlackKey(Qt::Key_8);
    AddBlackKey(Qt::Key_9);

    AddBlackKey(Qt::Key_Q);
    AddBlackKey(Qt::Key_W);
    AddBlackKey(Qt::Key_E);

    AddBlackKey(Qt::Key_T);
    AddBlackKey(Qt::Key_Z);

    AddBlackKey(Qt::Key_I);
    AddBlackKey(Qt::Key_O);
    AddBlackKey(Qt::Key_P);

    AddBlackKey(Qt::Key_S);
    AddBlackKey(Qt::Key_D);

    AddBlackKey(Qt::Key_G);
    AddBlackKey(Qt::Key_H);
    AddBlackKey(Qt::Key_J);

    AddBlackKey(Qt::Key_L);
    //AddBlackKey(Qt::);

    AddBlackKey(Qt::Key_C);
    AddBlackKey(Qt::Key_V);
    AddBlackKey(Qt::Key_B);


}


//der tastenfilter

bool keyEnterReceiver::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() != QEvent::KeyPress)
    {

        if(event->type() == QEvent::KeyRelease) // falls etwas losgelassen wird und es ctrl ist, weiße tasten
        {

            QKeyEvent *key = static_cast<QKeyEvent*>(event);
            if(key->key()==Qt::Key_Control)
            {
                keysshifted=false;
                qDebug() << "OFF";
                return true;
            }
        }

        return QObject::eventFilter(obj, event);
    }
    QKeyEvent *key = static_cast<QKeyEvent*>(event); // es ist ein tasten drück event

    if(!keyson)
    {
        keyson = true;
        InitKeys();

    }

    if(key->key() == Qt::Key_Control)//es ist ctrl, schwarze tasten
    {
        keysshifted = true;
        qDebug() << "ON";
        return true;
    }

    if(keysshifted)
    {
        int toadd =  0;
        for(int32_t i=0;i<keys_blackcur+1;i++) //offset für die taste suchen
        {
            if(keys_black[i] == key->key())
            {
                toadd = i;
                break;
            }
        }


        if(toadd)
        {

            toadd += 51; // +51 weil 51 weiße tasten

            char childname[0xFF] = {0};
            sprintf(childname, "pushButton_%d", toadd); //name=pushButton_tastennummer

            QString qtchildname = QString::fromLatin1(childname);

            QPushButton *button = win->centralWidget->findChild<QPushButton*>(qtchildname);

            qDebug() << button->objectName();

            button->pressed();


            return true;

        }

        return true;
    }

    int toadd =  0;
    for(int32_t i=0;i<keys_cur+1;i++) // das selbe für die weißen tasten
    {
        if(keys[i] == key->key())
        {
            toadd = i;
            break;
        }
    }

    if(toadd)
    {
        char childname[0xFF] = {0};
        if(toadd==1)
        {
            sprintf(childname, "pushButton");
        }else{
            sprintf(childname, "pushButton_%d", toadd);
        }

        QString qtchildname = QString::fromLatin1(childname);

        QPushButton *button = win->centralWidget->findChild<QPushButton*>(qtchildname);

        qDebug() << button->objectName();

        button->pressed();


        return true;

    }


    return  QObject::eventFilter(obj, event);
}


void MainWindow::on_pushButton_pressed()
{
    PlayButtonSound(1); // das hier kommt jetzt gut 88 mal, tasten mapping
}

void MainWindow::on_pushButton_52_pressed()
{
    PlayButtonSound(2);
}

void MainWindow::on_pushButton_2_pressed()
{
    PlayButtonSound(3);
}

void MainWindow::on_pushButton_3_pressed()
{
    PlayButtonSound(4);
}
void MainWindow::on_pushButton_53_pressed()
{
    PlayButtonSound(5);
}

void MainWindow::on_pushButton_4_pressed()
{
    PlayButtonSound(6);
}



void MainWindow::on_pushButton_54_pressed()
{
    PlayButtonSound(7);
}

void MainWindow::on_pushButton_5_pressed()
{
    PlayButtonSound(8);
}

void MainWindow::on_pushButton_6_pressed()
{
    PlayButtonSound(9);
}

void MainWindow::on_pushButton_56_pressed()
{
    PlayButtonSound(10);
}

void MainWindow::on_pushButton_8_pressed()
{
    PlayButtonSound(11);
}

void MainWindow::on_pushButton_55_pressed()
{
    PlayButtonSound(12);
}

void MainWindow::on_pushButton_10_pressed()
{
    PlayButtonSound(13);
}

void MainWindow::on_pushButton_57_pressed()
{
    PlayButtonSound(14);
}

void MainWindow::on_pushButton_12_pressed()
{
    PlayButtonSound(15);
}

void MainWindow::on_pushButton_11_pressed()
{
    PlayButtonSound(16);
}

void MainWindow::on_pushButton_61_pressed()
{
    PlayButtonSound(17);
}

void MainWindow::on_pushButton_9_pressed()
{
    PlayButtonSound(18);
}

void MainWindow::on_pushButton_60_pressed()
{
    PlayButtonSound(19);
}

void MainWindow::on_pushButton_7_pressed()
{
    PlayButtonSound(20);
}

void MainWindow::on_pushButton_14_pressed()
{
    PlayButtonSound(21);
}

void MainWindow::on_pushButton_58_pressed()
{
    PlayButtonSound(22);
}

void MainWindow::on_pushButton_17_pressed()
{
    PlayButtonSound(23);
}

void MainWindow::on_pushButton_59_pressed()
{
    PlayButtonSound(24);
}

void MainWindow::on_pushButton_23_pressed()
{
    PlayButtonSound(25);
}

void MainWindow::on_pushButton_62_pressed()
{
    PlayButtonSound(26);
}

void MainWindow::on_pushButton_19_pressed()
{
    PlayButtonSound(27);
}

void MainWindow::on_pushButton_15_pressed()
{
    PlayButtonSound(28);
}

void MainWindow::on_pushButton_69_pressed()
{
    PlayButtonSound(29);
}

void MainWindow::on_pushButton_13_pressed()
{
    PlayButtonSound(30);
}

void MainWindow::on_pushButton_68_pressed()
{
    PlayButtonSound(31);
}

void MainWindow::on_pushButton_18_pressed()
{
    PlayButtonSound(32);
}

void MainWindow::on_pushButton_20_pressed()
{
    PlayButtonSound(33);
}

void MainWindow::on_pushButton_65_pressed()
{
    PlayButtonSound(34);
}

void MainWindow::on_pushButton_22_pressed()
{
    PlayButtonSound(35);
}

void MainWindow::on_pushButton_66_pressed()
{
    PlayButtonSound(36);
}

void MainWindow::on_pushButton_16_pressed()
{
    PlayButtonSound(37);
}

void MainWindow::on_pushButton_72_pressed()
{
    PlayButtonSound(38);
}

void MainWindow::on_pushButton_21_pressed()
{
    PlayButtonSound(39);
}

void MainWindow::on_pushButton_24_pressed()
{
    PlayButtonSound(40);
}

void MainWindow::on_pushButton_71_pressed()
{
    PlayButtonSound(41);
}

void MainWindow::on_pushButton_34_pressed()
{
    PlayButtonSound(42);
}

void MainWindow::on_pushButton_63_pressed()
{
    PlayButtonSound(43);
}

void MainWindow::on_pushButton_48_pressed()
{
    PlayButtonSound(44);
}

void MainWindow::on_pushButton_33_pressed()
{
    PlayButtonSound(45);
}

void MainWindow::on_pushButton_67_pressed()
{
    PlayButtonSound(46);
}

void MainWindow::on_pushButton_36_pressed()
{
    PlayButtonSound(47);
}

void MainWindow::on_pushButton_64_pressed()
{
    PlayButtonSound(48);
}

void MainWindow::on_pushButton_42_pressed()
{
    PlayButtonSound(49);
}

void MainWindow::on_pushButton_70_pressed()
{
    PlayButtonSound(50);
}

void MainWindow::on_pushButton_32_pressed()
{
        PlayButtonSound(51);
}

void MainWindow::on_pushButton_41_pressed()
{
            PlayButtonSound(52);
}

void MainWindow::on_pushButton_79_pressed()
{
        PlayButtonSound(53);
}

void MainWindow::on_pushButton_39_pressed()
{
        PlayButtonSound(54);
}

void MainWindow::on_pushButton_78_pressed()
{
        PlayButtonSound(55);
}

void MainWindow::on_pushButton_30_pressed()
{
        PlayButtonSound(56);
}

void MainWindow::on_pushButton_27_pressed()
{
        PlayButtonSound(57);
}

void MainWindow::on_pushButton_82_pressed()
{
        PlayButtonSound(58);
}

void MainWindow::on_pushButton_45_pressed()
{
        PlayButtonSound(59);
}

void MainWindow::on_pushButton_80_pressed()
{
        PlayButtonSound(60);
}

void MainWindow::on_pushButton_29_pressed()
{
        PlayButtonSound(61);
}

void MainWindow::on_pushButton_76_pressed()
{
        PlayButtonSound(62);
}

void MainWindow::on_pushButton_46_pressed()
{
        PlayButtonSound(63);
}

void MainWindow::on_pushButton_37_pressed()
{
        PlayButtonSound(64);
}

void MainWindow::on_pushButton_74_pressed()
{
        PlayButtonSound(65);
}

void MainWindow::on_pushButton_25_pressed()
{
        PlayButtonSound(66);
}

void MainWindow::on_pushButton_77_pressed()
{
        PlayButtonSound(67);
}

void MainWindow::on_pushButton_40_pressed()
{
        PlayButtonSound(68);
}

void MainWindow::on_pushButton_26_pressed()
{
        PlayButtonSound(69);
}

void MainWindow::on_pushButton_73_pressed()
{
        PlayButtonSound(70);
}

void MainWindow::on_pushButton_35_pressed()
{
        PlayButtonSound(71);
}

void MainWindow::on_pushButton_75_pressed()
{
            PlayButtonSound(72);
}

void MainWindow::on_pushButton_43_pressed()
{
        PlayButtonSound(73);
}

void MainWindow::on_pushButton_81_pressed()
{
        PlayButtonSound(74);
}

void MainWindow::on_pushButton_38_pressed()
{
        PlayButtonSound(75);
}

void MainWindow::on_pushButton_28_pressed()
{
        PlayButtonSound(76);
}

void MainWindow::on_pushButton_85_pressed()
{
        PlayButtonSound(77);
}

void MainWindow::on_pushButton_47_pressed()
{
        PlayButtonSound(78);
}

void MainWindow::on_pushButton_86_pressed()
{
        PlayButtonSound(79);
}

void MainWindow::on_pushButton_44_pressed()
{
        PlayButtonSound(80);
}

void MainWindow::on_pushButton_31_pressed()
{
        PlayButtonSound(81);
}

void MainWindow::on_pushButton_87_pressed()
{
        PlayButtonSound(82);
}

void MainWindow::on_pushButton_49_pressed()
{
        PlayButtonSound(83);
}

void MainWindow::on_pushButton_83_pressed()
{
        PlayButtonSound(84);
}

void MainWindow::on_pushButton_50_pressed()
{
        PlayButtonSound(85);
}

void MainWindow::on_pushButton_51_pressed()
{
        PlayButtonSound(86);
}

void MainWindow::on_pushButton_88_pressed() // fkt für metronom
{
    static QMediaPlayer *metronom = 0;

    if(!metronom)
    {
        metronom = new QMediaPlayer;
        metronom->setMedia(QUrl::fromLocalFile("sounds/metronom.mp3"));
        metronom->setVolume(100);
    }

    static bool on = false;

    on = !on;

    if(on)
        metronom->play();
    else
        metronom->stop();


}

#include <QDir>

void MainWindow::on_pushButton_89_pressed() // fkt für demo song
{
    static QMediaPlayer *demo = 0;

    if(!demo)
    {
        qDebug() << QDir::current().path();
        demo = new QMediaPlayer;
        demo->setMedia(QUrl::fromLocalFile("sounds/demo.mp3"));
        demo->setVolume(100);
    }

    static bool on = false;

    on = !on;

    if(on)
        demo->play();
    else
        demo->stop();
}
