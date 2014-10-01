#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QShortcut>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    recursive = false;
    filesToBeDeleted = new QStringList();
    strmod = new QStringListModel();
    shortcut_del = new QShortcut(QKeySequence(Qt::Key_Delete), this); //see del_pressed(), enables the user to remove files with the delete key.
    shortcut_del->setContext(Qt::ApplicationShortcut);

    connect(shortcut_del, SIGNAL(activated()), this, SLOT(del_pressed()));

    viewModelStringList = new QStringList();
    strmod->setStringList(*viewModelStringList);

    algoText << "md5" << "sha1" << "sha224" << "sha256" << "sha384" << "sha512" << "sha3_224" << "sha3_256" << "sha3_384" << "sha3_512";

    ui->setupUi(this);
    ui->comboBox->addItems(algoText);
    alg = (Algo)ui->comboBox->currentIndex(); //make sure if you add or remove algorithms, to update the Text and makes sure the positions stay the same. If you put (lets say) murmurhash at first position in the header but not in the algoText you will confuse you users.
}

MainWindow::~MainWindow()
{
    delete ui;
}


QHash<QByteArray, QString> *MainWindow::HashSourceFiles(const QStringList &files)
{
    QHash<QByteArray, QString> * result = new QHash<QByteArray, QString>();
    QByteArray *hash;

    foreach(QString const &filepath, files)
    {

        hash = HashFile(filepath);
        result->insert(*hash, filepath);
    }

    return result;
}

QByteArray *MainWindow::HashFile(const QString &fileName) //TODO: move to a separate thread to prevent UI-lockup
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite);
    QByteArray *result;



    if(file.isReadable())
    {
        result = new QByteArray(QCryptographicHash::hash(file.readAll(), (QCryptographicHash::Algorithm)alg));
        file.close();        
        return result;
    }
    result = new QByteArray();
    return result;
}

QHash<QByteArray, QString> *MainWindow::HashTargetDir(const QString &dir, bool recursive)
{
    QDir dirpath(dir);
    dirpath.setFilter(QDir::Writable | QDir::AllEntries | QDir::NoDot); //these are the filters: all files writable without Dot
    QHash<QByteArray, QString> *result;
    result = new QHash<QByteArray, QString>();

    QByteArray *hash;
    hash = new QByteArray();
    QDirIterator dI(dirpath, recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(dirpath.entryList().count()); //count without recursion first. inaccuracies really shouldnt matter here


    int t_progress = 0;
    while(dI.hasNext())
    {

        t_progress++; //see: if t_progress == 10

        if(QFileInfo(dI.next()).isFile())
        {
            hash = HashFile (dI.filePath());
        }

        if(result->contains(*hash))  //in case of internal doubles. It cannot be assumed that even in the target directory there are none.
                                    //especially not if the user runs the program for the first time
        {

            QFile file(result->value(*hash));   //removed the todo. probablility of collision is at 1:10^60 with only md5, shouldnt be an issue

            if(file.open(QFile::ReadWrite) && file.exists())
            {                
                QDir doublesdir(dI.path() + "/InternalDoubles/");
                if(!doublesdir.exists()) //turns out that moving a file to a directory that does not yet exists does not work.
                {
                    doublesdir.mkpath(dI.path() + "/InternalDoubles/");
                }

                file.rename(dI.path() + "/InternalDoubles/" + dI.fileName()); //instead of deleting the file, move it to the /doubles/ dir
            }

            else
            {
              qDebug() << "You do not have the permission to change " << dI.filePath() << " or the file does not exist anymore\n";

            }
        }
        result->insert(*hash, dI.filePath());
        if (t_progress == 10) //update on every 10 steps, should be enough precision for everyone
        {
            int val = ui->progressBar->value();
            ui->progressBar->setValue(val+10);
            t_progress = 0;
        }

    }
    t_progress = 0;
    ui->progressBar->reset();

    return result;



}

void MainWindow::MergeHashLists(const QHash<QByteArray, QString> &sourceHash, const QHash<QByteArray, QString> &targetHash)
{
    QHash<QByteArray, QString>::const_iterator i = sourceHash.constBegin();

    while(i != sourceHash.constEnd())
    {
        if(targetHash.contains(i.key()))
        {
            QFile file(i.value());
            if(file.exists())
            {
                Rename(file);
            }

        }
        ++i;
    }
}

void MainWindow::Rename(QFile &file)
{    
    QFileInfo fI(file);
    QString newPath(fI.absolutePath() + "/doubles/" + fI.fileName());
    bool openingResult = true;

    if(QFile::exists(newPath))
    {
        QString newAltPath(fI.absolutePath() + "/doubles/nameCollisions/" + fI.fileName()); //there is probably a more elegant and generic
                                                                                            //way to auto-rename a file with the same name.
                                                                                            //none comes to mind currently
        file.open(QFile::ReadWrite);
        file.rename(newAltPath);
        file.close();
    }
    else
    {
        file.open(QFile::ReadWrite);
        file.rename(newPath);
        file.close();
    }

    if (!openingResult)
    {
        qDebug() << "Unable to open file: " << fI.absoluteFilePath() << ". Maybe you dont have the premissions to change it";
    }
}


void MainWindow::on_pb_Browse_clicked()
{
    QFileDialog dialog;
    QDir uiDir = (ui->le_targetDir->text());
    QDir directory;

    if (uiDir.exists() && uiDir.isReadable())
        {
        directory = uiDir;
        }

    else {
        directory = QDir::home();
        ui->le_targetDir->setText(directory.path());

    }
    dialog.setViewMode(QFileDialog::List);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setDirectory(directory);


    if (dialog.exec())
    {        
        QString dir(dialog.directory().path());
        ui->le_targetDir->setText(dir);
        HashTargetDir(dir); //the directory is immediately hashed after the Dialog is closed. TODO: change to only hash when clicking on merge
    }



}

void MainWindow::on_pb_Browse_Source_clicked()
{
    QFileDialog dialog;

    dialog.setViewMode(QFileDialog::List);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setDirectory(QDir::home());


    if(dialog.exec()) {
        if (!strmod->stringList().isEmpty()) { //either empty or currently displaying the hashes
            *viewModelStringList << dialog.selectedFiles(); //put all the files in a stringlist, then pass it to the model
            viewModelStringList->removeDuplicates();

        }
        else {
            *viewModelStringList << dialog.selectedFiles();
            strmod->setStringList(*viewModelStringList);
        }
        ui->listView->setModel(strmod);
        ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui->listView->setEditTriggers(QAbstractItemView::AnyKeyPressed |QAbstractItemView::DoubleClicked);


    }
    strmod->setStringList(*viewModelStringList);





}

void MainWindow::on_pb_Merge_clicked()
{

    QList<QByteArray> keys;
    QString t_str;

    if (!strmod->stringList().isEmpty())
    {
       keys = HashSourceFiles(*viewModelStringList)->keys(); //HashSourceFiles returns a QHash, its keys are the QCryptographicHash


       strmod->setStringList(*viewModelStringList);
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("You should select a few files first.");
        msgBox.exec();
    }


}


void MainWindow::del_pressed()
{


    if(!strmod->stringList().isEmpty())
    {
        QModelIndexList const mil = ui->listView->selectionModel()->selectedIndexes();
        foreach(QModelIndex dex, mil)
        {
            strmod->removeRows(dex.row(),1);
        }



    }
}


void MainWindow::on_cb_subdirs_toggled(bool checked)
{
    recursive = checked;
}


void MainWindow::on_comboBox_activated(int index)
{
    alg = (Algo)index;
}
