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

#include "dtmgr_settings.h"
#include "ui_dtmgr_settings.h"
#include <configfile.h>

dtmgr_settings::dtmgr_settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dtmgr_settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Dreamy Theater"+*configfile::keys::dtmgr_titlebar+" Edit Manager settings");
    ui->lineEdit_rom->setText(*configfile::keys::romdir);
    ui->lineEdit_Save->setText(*configfile::keys::savedir);
    ui->spinBox_User->setValue(*configfile::keys::user);
}

dtmgr_settings::~dtmgr_settings()
{
    delete ui;
}

void dtmgr_settings::on_pushButton_browse_rom_clicked()
{
    ui->lineEdit_rom->clear();
}

void dtmgr_settings::on_pushButton_browse_Save_clicked()
{
    ui->lineEdit_Save->clear();
    ui->spinBox_User->setValue(1);
}

void dtmgr_settings::on_buttonBox_accepted()
{
    *configfile::keys::romdir=ui->lineEdit_rom->text();
    configfile::settings.setValue(*configfile::keys::romdir_key, *configfile::keys::romdir);

    *configfile::keys::savedir=ui->lineEdit_Save->text();
    configfile::settings.setValue(*configfile::keys::savedir_key, *configfile::keys::savedir);

    *configfile::keys::user=ui->spinBox_User->value();
    configfile::settings.setValue(*configfile::keys::user_key, *configfile::keys::user);
}
