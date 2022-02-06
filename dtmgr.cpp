/*
Copyright (c) 2020 nastys

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "dtmgr.h"
#include "ui_dtmgr.h"
#include <types.h>
#include <QFile>
#include <QMessageBox>
#include <module.h>
#include <QtEndian>
#include <configfile.h>
#include <convert.h>
#include <savegen.h>
#include <pvdbparse.h>
#include <QFileDialog>
#include <QTextCodec>
#include <QDesktopServices>
#include <QInputDialog>

types::DT1::editSaveEntry entry1[100];
types::DT2::editSaveEntry entry2[100];
QFile systemSaveFile;
QString usrdir_id;

dtMgr::dtMgr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dtMgr)
{
    ui->setupUi(this);
    module::decModuleTbl();
    pvdbparse::parsePvDbSongList();
    this->setWindowTitle("Dreamy Theater"+*configfile::keys::dtmgr_titlebar+" Edit Manager");
    if(configfile::keys::dtmgr_limited)
    {
        ui->groupBox_moduleB->setVisible(false);
        ui->radioButton_Vorbis->setVisible(false);
    }
    else
    {
        ui->radioButton_adx->setVisible(false);
        ui->radioButton_aix->setVisible(false);
    }
    if(configfile::keys::dtmgr_wide)
    {
        ui->openGLWidget->setMinimumSize(250, 304);
        ui->openGLWidget_2->setMinimumSize(250, 304);
    }
    systemSaveFile.setFileName(*configfile::keys::savedir+"/SYSTEM.DAT");
    if(systemSaveFile.open(QIODevice::ReadWrite))
    {
        systemSaveFile.seek(*configfile::keys::editentry_addr);
        if(configfile::keys::dtmgr_limited)
        {
            systemSaveFile.read((char*)entry1, sizeof(entry1));
            for(int i=0; i<100; i++)
            {
                entry1[i].internalSongId=qFromBigEndian(entry1[i].internalSongId);
                ui->listWidget->addItem(QString::fromUtf8(entry1[i].title));
            }

            usrdir_id=".";
        }
        else
        {
            systemSaveFile.read((char*)entry2, sizeof(entry2));
            for(int i=0; i<100; i++)
            {
                entry2[i].moduleA=qFromBigEndian(entry2[i].moduleA);
                entry2[i].moduleB=qFromBigEndian(entry2[i].moduleB);
                entry2[i].internalSongId=qFromBigEndian(entry2[i].internalSongId);
                ui->listWidget->addItem(QString::fromUtf8(entry2[i].title));
            }

            QByteArray usrdir_id_ba;
            usrdir_id_ba.resize(32);
            systemSaveFile.seek(*configfile::keys::system_usrid_addr);
            systemSaveFile.read(usrdir_id_ba.data(), usrdir_id_ba.size());
            usrdir_id = QString::fromUtf8(usrdir_id_ba);
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", "Could not open DT 2nd save file.");
        close();
    }

    module::fillCharacterDropdown(ui->comboBox_characterA);
    if(!configfile::keys::dtmgr_limited) module::fillCharacterDropdown(ui->comboBox_characterB);

    for(int i=0; i<pvdbparse::parsedSongList.length(); i++)
        ui->comboBox_builtinSong->addItem(pvdbparse::parsedSongList.at(i).title);
}

dtMgr::~dtMgr()
{
    systemSaveFile.close();
    delete ui;
}

void dtMgr::on_listWidget_currentRowChanged(int currentRow)
{
    QString fileString;

    QString editNum=QStringLiteral("%1").arg(currentRow, 2, 10, QLatin1Char('0'));
    const QString editSaveFolderBase = *configfile::keys::savedir+"/../"+*configfile::keys::gameid+"-EDIT";
    const QString editFile = editSaveFolderBase+editNum+"/EDIT"+editNum+".DAT";

    if(configfile::keys::dtmgr_limited)
    {
        ui->lineEdit_title->setText(entry1[currentRow].title);
        ui->lineEdit_author->setText(entry1[currentRow].author);
        ui->checkBox_enable->setChecked(entry1[currentRow].enable);

        QFile edit(editFile);
        int editMdl=0;
        if(edit.open(QIODevice::ReadOnly))
        {
            edit.seek(0x89CC+0x10);
            edit.read((char*)&editMdl, sizeof(editMdl));
            edit.close();
        }

        ui->spinBox_moduleA->setValue(editMdl);

        fileString = entry1[currentRow].audioFile;
    }
    else
    {
        ui->lineEdit_title->setText(entry2[currentRow].title);
        ui->lineEdit_author->setText(entry2[currentRow].author);
        ui->spinBox_moduleA->setValue(entry2[currentRow].moduleA);
        ui->spinBox_moduleB->setValue(entry2[currentRow].moduleB);
        ui->checkBox_moduleB->setChecked(entry2[currentRow].duet);
        ui->checkBox_enable->setChecked(entry2[currentRow].enable);

        fileString = entry2[currentRow].audioFile;
    }
    if(fileString.isEmpty())
        ui->radioButton_none->setChecked(true);
    else
    {
        for(int i=0; i<pvdbparse::parsedSongList.length(); i++)
        {
            if(fileString==pvdbparse::parsedSongList.at(i).filename)
            {
                ui->radioButton_builtin->setChecked(true);
                ui->comboBox_builtinSong->setEnabled(true);
                ui->comboBox_builtinSong->setCurrentIndex(i);
                break;
            }
            else if(i==pvdbparse::parsedSongList.length()-1)
            {
                if(fileString.endsWith(".mp3"))
                {
                    if(QFile::exists(*configfile::keys::romdir+"/../" + fileString))
                        ui->radioButton_MP3_2->setChecked(true);
                    else
                        ui->radioButton_MP3->setChecked(true);
                }
                else if(fileString.endsWith(".ogg"))
                    ui->radioButton_Vorbis->setChecked(true);
                else if(fileString.endsWith(".adx"))
                    ui->radioButton_adx->setChecked(true);
                else if(fileString.endsWith(".aix"))
                    ui->radioButton_aix->setChecked(true);
            }
        }
    }

    if(QFile::exists(editFile))
    {
        ui->pushButton_import->setEnabled(0);
        ui->pushButton_export->setEnabled(1);
        ui->pushButton_delete->setEnabled(1);
    }
    else
    {
        ui->pushButton_import->setEnabled(1);
        ui->pushButton_export->setEnabled(0);
        ui->pushButton_delete->setEnabled(0);
    }

    ui->widget_audiofile->setEnabled(!ui->radioButton_none->isChecked());
    ui->widget_entry->setEnabled(1);
}

void dtMgr::on_comboBox_characterA_currentIndexChanged(int index)
{
    ui->comboBox_moduleA->blockSignals(true);
    module::chara chr=static_cast<module::chara>(index);
    module::fillModuleDropdown(ui->comboBox_moduleA, chr);
    on_comboBox_moduleA_currentTextChanged(ui->comboBox_moduleA->currentText());
    ui->comboBox_moduleA->blockSignals(false);
}

void dtMgr::on_comboBox_characterB_currentIndexChanged(int index)
{
    ui->comboBox_moduleB->blockSignals(true);
    module::chara chr=static_cast<module::chara>(index);
    module::fillModuleDropdown(ui->comboBox_moduleB, chr);
    on_comboBox_moduleB_currentTextChanged(ui->comboBox_moduleB->currentText());
    ui->comboBox_moduleB->blockSignals(false);
}

void dtMgr::on_spinBox_moduleA_valueChanged(int arg1)
{
    ui->openGLWidget->loadModule(arg1);
    ui->comboBox_moduleA->blockSignals(true);
    module::findModule(arg1, ui->comboBox_characterA, ui->comboBox_moduleA);
    ui->comboBox_moduleA->blockSignals(false);
}

void dtMgr::on_spinBox_moduleB_valueChanged(int arg1)
{
    ui->openGLWidget_2->loadModule(arg1);
    ui->comboBox_moduleB->blockSignals(true);
    module::findModule(arg1, ui->comboBox_characterB, ui->comboBox_moduleB);
    ui->comboBox_moduleB->blockSignals(false);
}

void dtMgr::on_comboBox_moduleA_currentTextChanged(const QString &arg1)
{
    QString name = arg1;
    QString charaStr;
    getCharaString(charaStr, (module::chara)(ui->comboBox_characterA->currentIndex()));
    int id = module::getModuleId(name, charaStr);
    if(id!=-1)
        ui->spinBox_moduleA->setValue(id);
}

void dtMgr::on_comboBox_moduleB_currentTextChanged(const QString &arg1)
{
    QString name = arg1;
    QString charaStr;
    getCharaString(charaStr, (module::chara)(ui->comboBox_characterB->currentIndex()));
    int id = module::getModuleId(name, charaStr);
    if(id!=-1)
        ui->spinBox_moduleB->setValue(id);
}

void dtMgr::on_checkBox_moduleB_stateChanged(int arg1)
{
    ui->spinBox_moduleB->setEnabled(arg1);
    ui->comboBox_characterB->setEnabled(arg1);
    ui->comboBox_moduleB->setEnabled(arg1);
    if(!arg1) ui->spinBox_moduleB->setValue(0);
}

void dtMgr::on_pushButton_close_clicked()
{
    this->close();
}

void dtMgr::on_pushButton_apply_clicked()
{
    const int i = ui->listWidget->currentRow();

    ui->listWidget->item(i)->setText(ui->lineEdit_title->text());

    const QByteArray title=ui->lineEdit_title->text().toUtf8();
    const char* title_c=title.constData();

    if(configfile::keys::dtmgr_limited)
    {
        strcpy(entry1[i].title, title_c);
        for(int c=strlen(entry1[i].title); c<256; c++) entry1[i].title[c]='\0';

        const QByteArray author=ui->lineEdit_author->text().toUtf8();
        const char* author_c=author.constData();

        strcpy(entry1[i].author, author_c);
        for(int c=strlen(entry1[i].author); c<128; c++) entry1[i].author[c]='\0';

        types::DT1::editSaveEntry tmpentry;
        memcpy(&tmpentry.unk1, &entry1[i].unk1, sizeof(tmpentry.unk1));
        memcpy(&tmpentry.unk2, &entry1[i].unk2, sizeof(tmpentry.unk2));
        memcpy(&tmpentry.unk3, &entry1[i].unk3, sizeof(tmpentry.unk3));
        memset(tmpentry.title, 0, sizeof(tmpentry.title));
        memcpy(tmpentry.title, entry1[i].title, sizeof(tmpentry.title));
        memset(tmpentry.author, 0, sizeof(tmpentry.author));
        memcpy(tmpentry.author, entry1[i].author, sizeof(tmpentry.author));
        tmpentry.editN=entry1[i].editN=qToBigEndian(i);
        tmpentry.enable=entry1[i].enable=qToBigEndian(static_cast<int>(ui->checkBox_enable->isChecked()));

        memset(entry1[i].audioFile, 0, sizeof(entry1[i].audioFile));
        memset(tmpentry.audioFile, 0, sizeof(tmpentry.audioFile));
        if(ui->radioButton_MP3->isChecked() || ui->radioButton_MP3_2->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editmp3str="DATA/SONG_EDIT"+editNum.toLatin1()+".mp3";
            qstrcpy(entry1[i].audioFile, editmp3str.constData());
            qstrcpy(tmpentry.audioFile, editmp3str.constData());
        }
        else if(ui->radioButton_Vorbis->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editoggstr="DATA/SONG_EDIT"+editNum.toLatin1()+".ogg";
            qstrcpy(entry1[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else if(ui->radioButton_adx->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editoggstr="DATA/SONG_EDIT"+editNum.toLatin1()+".adx";
            qstrcpy(entry1[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else if(ui->radioButton_aix->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editoggstr="DATA/SONG_EDIT"+editNum.toLatin1()+".aix";
            qstrcpy(entry1[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else if(ui->radioButton_builtin->isChecked())
        {
            const QByteArray editoggstr=pvdbparse::parsedSongList.at(ui->comboBox_builtinSong->currentIndex()).filename.toUtf8();
            qstrcpy(entry1[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else songFileCleanup();
        tmpentry.internalSongId=qToBigEndian(-1);

        systemSaveFile.seek(*configfile::keys::editentry_addr+sizeof(tmpentry)*i);
        systemSaveFile.write((char*)&tmpentry, sizeof(tmpentry));
    }
    else
    {
        strcpy(entry2[i].title, title_c);
        for(int c=strlen(entry2[i].title); c<256; c++) entry2[i].title[c]='\0';

        const QByteArray author=ui->lineEdit_author->text().toUtf8();
        const char* author_c=author.constData();

        strcpy(entry2[i].author, author_c);
        for(int c=strlen(entry2[i].author); c<128; c++) entry2[i].author[c]='\0';

        entry2[i].moduleA=ui->spinBox_moduleA->value();

        if(ui->checkBox_moduleB->isChecked()) entry2[i].moduleB=ui->spinBox_moduleB->value();
        else entry2[i].moduleB=0xFFFF;

        types::DT2::editSaveEntry tmpentry;
        tmpentry.duet=entry2[i].duet=ui->checkBox_moduleB->isChecked();
        memcpy(tmpentry.unk1, entry2[i].unk1, sizeof(tmpentry.unk1));
        memcpy(tmpentry.unk2, entry2[i].unk2, sizeof(tmpentry.unk2));
        memset(tmpentry.title, 0, sizeof(tmpentry.title));
        memcpy(tmpentry.title, entry2[i].title, sizeof(tmpentry.title));
        memset(tmpentry.author, 0, sizeof(tmpentry.author));
        memcpy(tmpentry.author, entry2[i].author, sizeof(tmpentry.author));
        tmpentry.moduleA=qToBigEndian(entry2[i].moduleA);
        tmpentry.moduleB=qToBigEndian(entry2[i].moduleB);
        tmpentry.unk1[0]=entry2[i].unk1[0]=1;
        tmpentry.editN=entry2[i].editN=i;
        tmpentry.enable=entry2[i].enable=ui->checkBox_enable->isChecked();
        tmpentry.unkmdl=entry2[i].unkmdl=-1;

        memset(entry2[i].audioFile, 0, sizeof(entry2[i].audioFile));
        memset(tmpentry.audioFile, 0, sizeof(tmpentry.audioFile));
        if(ui->radioButton_MP3->isChecked() || ui->radioButton_MP3_2->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editmp3str="DATA/SONG_EDIT"+editNum.toLatin1()+".mp3";
            qstrcpy(entry2[i].audioFile, editmp3str.constData());
            qstrcpy(tmpentry.audioFile, editmp3str.constData());
        }
        else if(ui->radioButton_Vorbis->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editoggstr="DATA/SONG_EDIT"+editNum.toLatin1()+".ogg";
            qstrcpy(entry2[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else if(ui->radioButton_adx->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editoggstr="DATA/SONG_EDIT"+editNum.toLatin1()+".adx";
            qstrcpy(entry2[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else if(ui->radioButton_aix->isChecked())
        {
            const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
            QByteArray editoggstr="DATA/SONG_EDIT"+editNum.toLatin1()+".aix";
            qstrcpy(entry2[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else if(ui->radioButton_builtin->isChecked())
        {
            const QByteArray editoggstr=pvdbparse::parsedSongList.at(ui->comboBox_builtinSong->currentIndex()).filename.toUtf8();
            qstrcpy(entry2[i].audioFile, editoggstr.constData());
            qstrcpy(tmpentry.audioFile, editoggstr.constData());
        }
        else songFileCleanup();
        tmpentry.internalSongId=qToBigEndian(-1);

        systemSaveFile.seek(*configfile::keys::editentry_addr+sizeof(tmpentry)*i);
        systemSaveFile.write((char*)&tmpentry, sizeof(tmpentry));
    }
    convert::fixCrc32(systemSaveFile);
    systemSaveFile.flush();
}

void dtMgr::on_radioButton_builtin_clicked()
{
    ui->comboBox_builtinSong->setEnabled(true);
    ui->widget_audiofile->setEnabled(1);
}

void dtMgr::on_pushButton_import_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Select PSP edit", "", "DIVA PSP edit (SECURE.BIN)");
    if(path.isEmpty()) return;
    QFile input(path);
    const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
    const QString editDir=*configfile::keys::savedir+"/../"+*configfile::keys::gameid+"-EDIT"+editNum;
    const QString editFilePath=editDir+"/EDIT"+editNum+".DAT";
    QDir().mkdir(editDir);
    QFile output(editFilePath);
    int current=0;
    QStringList typeList;
    if(configfile::keys::dtmgr_limited)
    {
        typeList.append("PSP 1st encrypted");
        typeList.append("PSP 1st decrypted");
    }
    else
    {
        typeList.append("PSP 2nd encrypted");
        typeList.append("PSP 2nd decrypted");
        typeList.append("PSP Extend encrypted");
        typeList.append("PSP Extend decrypted");
        if(configfile::keys::dtmgr_wide)
        {
            current=2;
        }
    }
    bool ok;
    QString type = QInputDialog::getItem(this, "Edit type", "Select edit type", typeList, current, false, &ok);
    if(ok)
    {
             if(type=="PSP 1st encrypted")    convert::psp_to_dt(input, output, true);
        else if(type=="PSP 1st decrypted")    convert::psp_to_dt(input, output, false);
        else if(type=="PSP 2nd encrypted")    convert::psp2nd_to_dt2nd(input, output, false, true);
        else if(type=="PSP 2nd decrypted")    convert::psp2nd_to_dt2nd(input, output, false, false);
        else if(type=="PSP Extend encrypted") convert::psp2nd_to_dt2nd(input, output, true, true);
        else if(type=="PSP Extend decrypted") convert::psp2nd_to_dt2nd(input, output, true, false);

        QString name = "UltraEdit";
        paramSfo::generateEditSave(editNum, name);
        ui->checkBox_enable->setChecked(true);

        on_pushButton_reset_metadata_clicked();
    }
}

void dtMgr::on_pushButton_export_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Save PSP edit", "SECURE.BIN", "DIVA PSP edit (SECURE.BIN)");
    if(path.isEmpty()) return;
    QFile output(path);
    const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
    const QString editDir=*configfile::keys::savedir+"/../"+*configfile::keys::gameid+"-EDIT"+editNum;
    const QString editFilePath=editDir+"/EDIT"+editNum+".DAT";
    QFile input(editFilePath);
    convert::dt2nd_to_psp2nd(input, output, configfile::keys::dtmgr_wide, true);
}

void dtMgr::on_pushButton_delete_clicked()
{
    if(QMessageBox::warning(this, "Delete edit data", "Are you sure?", QMessageBox::StandardButton::Yes|QMessageBox::StandardButton::No)==QMessageBox::Yes)
    {
        paramSfo::deleteEditSave(ui->listWidget->currentRow());
        on_listWidget_currentRowChanged(ui->listWidget->currentRow());
    }
}

void dtMgr::on_pushButton_clear_clicked()
{
    if(QMessageBox::warning(this, "Delete entry", "Are you sure?", QMessageBox::StandardButton::Yes|QMessageBox::StandardButton::No)==QMessageBox::Yes)
    {
        paramSfo::deleteEditSave(ui->listWidget->currentRow());

        ui->checkBox_enable->setChecked(false);
        ui->spinBox_moduleA->setValue(0);
        ui->spinBox_moduleB->setValue(0);
        ui->checkBox_moduleB->setChecked(false);
        ui->lineEdit_title->clear();
        ui->lineEdit_author->clear();
        ui->radioButton_none->setChecked(true);

        on_pushButton_apply_clicked();
        on_listWidget_currentRowChanged(ui->listWidget->currentRow());
    }
}

void dtMgr::on_pushButton_reset_metadata_clicked()
{
    const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
    const QString editDir=*configfile::keys::savedir+"/../"+*configfile::keys::gameid+"-EDIT"+editNum;
    const QString editFilePath=editDir+"/EDIT"+editNum+".DAT";
    QTextCodec *shift_jis_codec = QTextCodec::codecForName("Shift-JIS");

    QFile edit(editFilePath);
    if(!edit.open(QIODevice::ReadOnly)) return;
    QDataStream editQds(&edit);

    if(configfile::keys::dtmgr_limited)
    {
        editQds.setByteOrder(QDataStream::ByteOrder::LittleEndian);

        edit.seek(0x89CC+0x10);
        int editMdl;
        editQds >> editMdl;

        edit.seek(0x8AA0+0x10);
        ui->lineEdit_author->setText(shift_jis_codec->toUnicode(edit.read(0x44)));

        edit.seek(0x8ACC+0x10);
        ui->lineEdit_title->setText(shift_jis_codec->toUnicode(edit.read(0x44)));
        ui->listWidget->item(ui->listWidget->currentRow())->setText(ui->lineEdit_title->text());

        ui->spinBox_moduleA->setValue(editMdl);
    }
    else
    {
        editQds.setByteOrder(QDataStream::ByteOrder::BigEndian);

        edit.seek(0x35400+0x10);
        int editMdlA;
        int editMdlB;
        editQds >> editMdlA;
        editQds >> editMdlB;

        edit.seek(0x35524+0x10);
        const QByteArray author_ba = edit.read(0x44);
        const QString author = shift_jis_codec->toUnicode(author_ba);
        ui->lineEdit_author->setText(author);

        edit.seek(0x35550+0x10);
        const QByteArray title_ba = edit.read(0x44);
        const QString title = shift_jis_codec->toUnicode(title_ba);
        ui->lineEdit_title->setText(title);

        ui->spinBox_moduleA->setValue(editMdlA);
        if(editMdlB==-1)
        {
            ui->spinBox_moduleB->setValue(0);
            ui->checkBox_moduleB->setChecked(false);
        }
        else
        {
            ui->checkBox_moduleB->setChecked(true);
            ui->spinBox_moduleB->setValue(editMdlB);
        }
    }

    edit.close();

    on_pushButton_apply_clicked();
    on_listWidget_currentRowChanged(ui->listWidget->currentRow());
}

void dtMgr::on_radioButton_MP3_clicked()
{
    QString result = QFileDialog::getOpenFileName(this, "Select MP3", "", "MPEG-1/2/2.5 Audio Layer III (*.mp3 *.mpga)");
    if(!result.isEmpty())
    {
        const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
        const QString dstFolder=*configfile::keys::romdir+"/../../../"+*configfile::keys::gameid+"-DATA/USRDIR/"+usrdir_id+"/USER"+QStringLiteral("%1").arg(*configfile::keys::user, 8, 10, QLatin1Char('0'))+"/DATA/";
        const QString filePath=dstFolder + "SONG_EDIT"+editNum+".mp3";
        songFileCleanup();
        if(!QDir().mkpath(dstFolder) || !QFile::copy(result, filePath))
        {
            QMessageBox::warning(this, "Warning", "Couldn't copy \"" + result + "\" to \"" + filePath + "\".");
        }
        on_pushButton_apply_clicked();
    }
    ui->widget_audiofile->setEnabled(1);
    ui->comboBox_builtinSong->setEnabled(0);
}

void dtMgr::on_pushButton_audio_preview_clicked()
{
    const QString path = getAudioPath();
    if(!QDesktopServices::openUrl(path))
        QMessageBox::warning(this, "Warning", "Couldn't open \"" + path + "\".");
}

void dtMgr::on_radioButton_none_clicked()
{
    ui->widget_audiofile->setEnabled(0);
    ui->comboBox_builtinSong->setEnabled(0);
}

void dtMgr::on_radioButton_Vorbis_clicked()
{
    installSongHack("Select Ogg Vorbis", "Ogg (*.ogg)");
}

void dtMgr::songFileCleanup()
{
    const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
    const QString filePath=*configfile::keys::romdir+"/../../../"+*configfile::keys::gameid+"-DATA/USRDIR/"+usrdir_id+"/USER"+QStringLiteral("%1").arg(*configfile::keys::user, 8, 10, QLatin1Char('0'))+"/DATA/SONG_EDIT"+editNum+'.';
    QFile::remove(filePath+"mp3");
    QFile::remove(filePath+"ogg");
    QFile::remove(filePath+"adx");
    QFile::remove(filePath+"aix");
    QFile::remove(*configfile::keys::romdir+"/../DATA/SONG_EDIT"+editNum+".mp3");
    QFile::remove(*configfile::keys::romdir+"/../DATA/SONG_EDIT"+editNum+".ogg");
    QFile::remove(*configfile::keys::romdir+"/../DATA/SONG_EDIT"+editNum+".adx");
    QFile::remove(*configfile::keys::romdir+"/../DATA/SONG_EDIT"+editNum+".aix");
}

void dtMgr::on_radioButton_adx_clicked()
{
    installSongHack("Select ADX", "ADX (*.adx)");
}

void dtMgr::installSongHack(QString dlgTitle, QString dlgFormat)
{
    QString result = QFileDialog::getOpenFileName(this, dlgTitle, "", dlgFormat);
    if(!result.isEmpty())
    {
        const QString extension = "." + result.split('.').last().toLower();
        const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
        const QString stubFilePath=*configfile::keys::romdir+"/../../../"+*configfile::keys::gameid+"-DATA/USRDIR/"+usrdir_id+"/USER"+QStringLiteral("%1").arg(*configfile::keys::user, 8, 10, QLatin1Char('0'))+"/DATA/SONG_EDIT"+editNum+extension;
        const QString dataFileFolder=*configfile::keys::romdir+"/../DATA/";
        const QString dataFilePath=dataFileFolder + "SONG_EDIT"+editNum+extension;
        songFileCleanup();
        QFile stub(stubFilePath);
        stub.open(QIODevice::ReadWrite);
        stub.resize(0);
        stub.close();
        QFile::copy(result, dataFilePath);
        if(!QDir().mkpath(dataFileFolder) || !QFile::copy(result, dataFilePath))
        {
            //QMessageBox::warning(this, "Warning", "Couldn't copy \"" + result + "\" to \"" + dataFilePath + "\".");
        }
        on_pushButton_apply_clicked();
    }
    ui->widget_audiofile->setEnabled(1);
    ui->comboBox_builtinSong->setEnabled(0);
}

void dtMgr::on_radioButton_aix_clicked()
{
    installSongHack("Select AIX", "AIX (*.aix)");
}

void dtMgr::on_pushButton_audio_export_clicked()
{
    QString audioPath = getAudioPath();
    if(!audioPath.isEmpty())
    {
        const QString extension = audioPath.right(4);
        QString saveAs = QFileDialog::getSaveFileName(this, "Save audio", "audio"+extension, "Audio (*"+extension+')');
        if(!saveAs.isEmpty())
        {
            QFile::remove(saveAs);
            QFile::copy(audioPath, saveAs);
        }
    }
}

QString dtMgr::getAudioPath()
{
    if(ui->radioButton_none->isChecked())
        return "";
    else if(ui->radioButton_MP3->isChecked())
    {
        QString audiofile;
        if(configfile::keys::dtmgr_limited) audiofile = entry1[ui->listWidget->currentRow()].audioFile;
        else audiofile = entry2[ui->listWidget->currentRow()].audioFile;

        return *configfile::keys::romdir+"/../../../"+*configfile::keys::gameid+"-DATA/USRDIR/"+usrdir_id+"/USER"+QStringLiteral("%1").arg(*configfile::keys::user, 8, 10, QLatin1Char('0'))+"/" + audiofile;
    }
    else if(ui->radioButton_builtin->isChecked())
        return *configfile::keys::romdir + "/../" + pvdbparse::parsedSongList.at(ui->comboBox_builtinSong->currentIndex()).filename;
    else
    {
        QString extension;
        if(ui->radioButton_adx->isChecked()) extension=".adx";
        else if(ui->radioButton_aix->isChecked()) extension=".aix";
        else if(ui->radioButton_MP3_2->isChecked()) extension=".mp3";
        else extension=".ogg";
        const QString editNum=QStringLiteral("%1").arg(ui->listWidget->currentRow(), 2, 10, QLatin1Char('0'));
        const QString stubFilePath=*configfile::keys::romdir+"/../../../"+*configfile::keys::gameid+"-DATA/USRDIR/"+usrdir_id+"/USER"+QStringLiteral("%1").arg(*configfile::keys::user, 8, 10, QLatin1Char('0'))+"/DATA/SONG_EDIT"+editNum+extension;
        return *configfile::keys::romdir+"/../DATA/SONG_EDIT"+editNum+extension;
    }
}

void dtMgr::on_radioButton_MP3_2_clicked()
{
    installSongHack("Select MP3", "MPEG-1/2/2.5 Audio Layer III (*.mp3)");
}

