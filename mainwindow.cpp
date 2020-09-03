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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <dtmgr.h>
#include <configfile.h>
#include <QMessageBox>
#include <QFileDialog>
#include <savegen.h>
#include <QDir>
#include <dtmgr_settings.h>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    switch(configfile::settings.value("dt_lastused", 0).toInt())
    {
    default:
    case 0:
        ui->radioButton_dt1->setChecked(true);
        on_radioButton_dt1_clicked();
        break;
    case 1:
        ui->radioButton_dt2->setChecked(true);
        on_radioButton_dt2_clicked();
        break;
    case 2:
        ui->radioButton_dt3->setChecked(true);
        on_radioButton_dt3_clicked();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
}

void MainWindow::on_pushButton_2_clicked()
{
    QFile input("in_dt2.bin");
    QFile output("out_psp2.bin");

    input.open(QIODevice::ReadOnly);
    output.open(QIODevice::WriteOnly);

    output.resize(0);

    QDataStream in(&input);
    QDataStream out(&output);

    in.setByteOrder(QDataStream::ByteOrder::BigEndian);
    out.setByteOrder(QDataStream::ByteOrder::LittleEndian);

    input.seek(16);

    int v;
    while(!in.atEnd())
    {
        in >> v;
        out << v;
    }

    input.close();
    output.close();
}

QString MainWindow::folderCfgDlg(QString *titlebar, QString type, QString hint, QString &defaultdir, QString requiredfile="")
{
    QMessageBox::information(this, QApplication::applicationName(), "Please select the "+type+" folder of Dreamy Theater"+*titlebar+hint+".");
    QString result = QFileDialog::getExistingDirectory(this, "Select the "+type+" folder of Dreamy Theater"+*titlebar, defaultdir);
    if(result.isEmpty()) return "";
    if(!requiredfile.isEmpty()&&!QFile::exists(result+"/"+requiredfile))
    {
        QMessageBox::critical(this, QApplication::applicationName(), requiredfile+" not found. Please try again.");
        return "";
    }

    return result;
}

void MainWindow::on_pushButton_dtMgr_clicked()
{
    if(configfile::keys::romdir->isEmpty()||!QDir().exists(*configfile::keys::romdir))
    {
        QString romdir_dlg = folderCfgDlg(configfile::keys::dtmgr_titlebar, "rom", " (game/"+*configfile::keys::gameid+"/USRDIR/rom)", *configfile::keys::defaultdir_rom);
        if(romdir_dlg.isEmpty()) return;
        *configfile::keys::romdir=romdir_dlg;
        configfile::settings.setValue(*configfile::keys::romdir_key, romdir_dlg);
    }

    if(configfile::keys::savedir->isEmpty()||!QDir().exists(*configfile::keys::savedir))
    {
        QString savedir_dlg = folderCfgDlg(configfile::keys::dtmgr_titlebar, "save", " (home/00000001/savedata/"+*configfile::keys::gameid+"-SYSTEM00/", *configfile::keys::defaultdir_save, "SYSTEM.DAT");
        if(savedir_dlg.isEmpty()) return;
        *configfile::keys::savedir=savedir_dlg;
        configfile::settings.setValue(*configfile::keys::savedir_key, savedir_dlg);

        *configfile::keys::user=1;
        QByteArray usrdir_id;
        usrdir_id.resize(32);
        QFile systemSaveFile(*configfile::keys::savedir+"/SYSTEM.DAT");
        if(systemSaveFile.open(QIODevice::ReadOnly))
        {
            systemSaveFile.seek(*configfile::keys::system_usrid_addr);
            systemSaveFile.read(usrdir_id.data(), usrdir_id.size());
            systemSaveFile.close();
            const QString usrdir_id_str = QString::fromUtf8(usrdir_id);
            const QString usrPath = *configfile::keys::romdir+"/../../../"+configfile::keys::gameid+"-DATA/USRDIR/"+usrdir_id_str+"/USER";
            for(int i=0; i<100; i++)
            {
                const QString editNum=QStringLiteral("%1").arg(i, 8, 10, QLatin1Char('0'));
                if(QDir().exists(usrPath+editNum))
                {
                    *configfile::keys::user=i;
                    break;
                }
            }
        }
        configfile::settings.setValue(*configfile::keys::user_key, *configfile::keys::user);
    }

    dtMgr manager;
    manager.exec();
}

void MainWindow::on_radioButton_dt1_clicked()
{
    configfile::keys::romdir=&configfile::storagekeys::romdir_dt;
    configfile::keys::romdir_key=&configfile::standard::keydef::romdir_dt;
    configfile::keys::savedir=&configfile::storagekeys::savedir_dt;
    configfile::keys::savedir_key=&configfile::standard::keydef::savedir_dt;
    configfile::keys::defaultdir_rom=&configfile::standard::defaultdir::dd_rom_dt;
    configfile::keys::defaultdir_save=&configfile::standard::defaultdir::dd_save_dt;
    configfile::keys::dtmgr_titlebar=&configfile::standard::titlebar::titlebar;
    configfile::keys::dtmgr_limited=true;
    configfile::keys::dtmgr_wide=false;
    configfile::keys::gameid=&configfile::standard::gameid::gameid_dt;
    configfile::keys::editentry_addr=&configfile::standard::address::editentry_addr_dt2;
    //
    configfile::keys::user=&configfile::storagekeys::user_dt;
    configfile::keys::user_key=&configfile::standard::keydef::user_dt;

    configfile::settings.setValue("dt_lastused", 0);
}

void MainWindow::on_radioButton_dt2_clicked()
{
    configfile::keys::romdir=&configfile::storagekeys::romdir_dt2;
    configfile::keys::romdir_key=&configfile::standard::keydef::romdir_dt2;
    configfile::keys::savedir=&configfile::storagekeys::savedir_dt2;
    configfile::keys::savedir_key=&configfile::standard::keydef::savedir_dt2;
    configfile::keys::defaultdir_rom=&configfile::standard::defaultdir::dd_rom_dt2;
    configfile::keys::defaultdir_save=&configfile::standard::defaultdir::dd_save_dt2;
    configfile::keys::dtmgr_titlebar=&configfile::standard::titlebar::titlebar_2nd;
    configfile::keys::dtmgr_limited=false;
    configfile::keys::dtmgr_wide=false;
    configfile::keys::gameid=&configfile::standard::gameid::gameid_dt2;
    configfile::keys::editentry_addr=&configfile::standard::address::editentry_addr_dt2;
    configfile::keys::system_usrid_addr=&configfile::standard::address::system_usrid_addr_dt2;
    configfile::keys::user=&configfile::storagekeys::user_dt2;
    configfile::keys::user_key=&configfile::standard::keydef::user_dt2;

    configfile::settings.setValue("dt_lastused", 1);
}

void MainWindow::on_radioButton_dt3_clicked()
{
    configfile::keys::romdir=&configfile::storagekeys::romdir_dt3;
    configfile::keys::romdir_key=&configfile::standard::keydef::romdir_dt3;
    configfile::keys::savedir=&configfile::storagekeys::savedir_dt3;
    configfile::keys::savedir_key=&configfile::standard::keydef::savedir_dt3;
    configfile::keys::defaultdir_rom=&configfile::standard::defaultdir::dd_rom_dt3;
    configfile::keys::defaultdir_save=&configfile::standard::defaultdir::dd_save_dt3;
    configfile::keys::dtmgr_titlebar=&configfile::standard::titlebar::titlebar_extend;
    configfile::keys::dtmgr_limited=false;
    configfile::keys::dtmgr_wide=true;
    configfile::keys::gameid=&configfile::standard::gameid::gameid_dt3;
    configfile::keys::editentry_addr=&configfile::standard::address::editentry_addr_dt2;
    configfile::keys::system_usrid_addr=&configfile::standard::address::system_usrid_addr_dt3;
    configfile::keys::user=&configfile::storagekeys::user_dt3;
    configfile::keys::user_key=&configfile::standard::keydef::user_dt3;

    configfile::settings.setValue("dt_lastused", 2);
}

void MainWindow::on_pushButton_dtMgrSettings_clicked()
{
    dtmgr_settings settings;
    settings.exec();
}

void MainWindow::on_action_Exit_triggered()
{
    exit(0);
}

void MainWindow::on_action_Discord_server_triggered()
{
    QDesktopServices::openUrl(QString("https://discord.gg/cvBVGDZ"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About UltraEdit", "UltraEdit 1.0\n\n"

                                                "Copyright (c) 2020 nastys\n\n"

                       "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
                       "of this software and associated documentation files (the \"Software\"), to deal\n"
                       "in the Software without restriction, including without limitation the rights\n"
                       "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
                       "copies of the Software, and to permit persons to whom the Software is\n"
                       "furnished to do so, subject to the following conditions:\n\n"

                       "The above copyright notice and this permission notice shall be included in all\n"
                       "copies or substantial portions of the Software.\n\n"

                       "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
                       "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
                       "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
                       "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
                       "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
                       "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
                       "SOFTWARE."
                       );

    QMessageBox::about(this, "About PSP SDK",
                       "PSP Software Development Kit - http://www.pspdev.org\n"
                       "-----------------------------------------------------------------------\n"
                       "Licensed under the BSD license, see LICENSE in PSPSDK root for details.\n\n"

                       "Copyright (c) 2005 Jim Paris <jim@jtan.com>\n"
                       "Coypright (c) 2005 psp123"
                       );

    QMessageBox::about(this, "About PSP SDK",
                       "PSPSDK BSD-compatible copyright notice (Some parts of the PSPSDK were used)\n\n"

                       "Copyright (c) 2005  adresd\n"
                       "Copyright (c) 2005  Marcus R. Brown\n"
                       "Copyright (c) 2005  James Forshaw\n"
                       "Copyright (c) 2005  John Kelley\n"
                       "Copyright (c) 2005  Jesper Svennevid\n"
                       "All rights reserved.\n\n"

                       "Redistribution and use in source and binary forms, with or without\n"
                       "modification, are permitted provided that the following conditions\n"
                       "are met:\n"
                       "1. Redistributions of source code must retain the above copyright\n"
                          "notice, this list of conditions and the following disclaimer.\n"
                       "2. Redistributions in binary form must reproduce the above copyright\n"
                          "notice, this list of conditions and the following disclaimer in the\n"
                          "documentation and/or other materials provided with the distribution.\n"
                       "3. The names of the authors may not be used to endorse or promote products\n"
                          "derived from this software without specific prior written permission.\n\n"

                       "THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR\n"
                       "IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES\n"
                       "OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.\n"
                       "IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
                       "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT\n"
                       "NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
                       "DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
                       "THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
                       "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n"
                       "THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
                       );

    QMessageBox::about(this, "About KIRK-ENGINE", "Draan proudly presents:\n\n"

                       "With huge help from community:\n"
                       "coyotebean, Davee, hitchhikr, kgsws, liquidzigong, Mathieulh, Proxima, SilverSpring\n\n"

                       "******************** KIRK-ENGINE ********************\n"
                       "An Open-Source implementation of KIRK (PSP crypto engine) algorithms and keys.\n"
                       "Includes also additional routines for hash forging.\n\n"

                       "********************\n\n"

                       "This program is free software: you can redistribute it and/or modify\n"
                       "it under the terms of the GNU General Public License as published by\n"
                       "the Free Software Foundation, either version 3 of the License, or\n"
                       "(at your option) any later version.\n"
                       "This program is distributed in the hope that it will be useful,\n"
                       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                       "GNU General Public License for more details.\n"
                       "You should have received a copy of the GNU General Public License\n"
                       "along with this program.  If not, see <http://www.gnu.org/licenses/>."
                       );

    QMessageBox::about(this, "About PPSSPP", "Copyright (c) 2012- PPSSPP Project.\n\n"

                       "This program is free software: you can redistribute it and/or modify\n"
                       "it under the terms of the GNU General Public License as published by\n"
                       "the Free Software Foundation, version 2.0 or later versions.\n\n"

                       "This program is distributed in the hope that it will be useful,\n"
                       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                       "GNU General Public License 2.0 for more details.\n\n"

                       "A copy of the GPL 2.0 should have been included with the program.\n"
                       "If not, see http://www.gnu.org/licenses/\n\n"

                       "Official git repository and contact information can be found at\n"
                       "https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/."
                       );

    QMessageBox::about(this, "About qt-crc32", "Copyright (c) Alexander Nusov 2015\n\n"

                       "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
                       "of this software and associated documentation files (the \"Software\"), to deal\n"
                       "in the Software without restriction, including without limitation the rights\n"
                       "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
                       "copies of the Software, and to permit persons to whom the Software is\n"
                       "furnished to do so, subject to the following conditions:\n\n"

                       "The above copyright notice and this permission notice shall be included in all\n"
                       "copies or substantial portions of the Software.\n\n"

                       "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
                       "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
                       "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
                       "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
                       "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
                       "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
                       "SOFTWARE."
                       );
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}
