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
#include <QApplication>
#include <module.h>
#include <configfile.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString defaultval="";

    configfile::storagekeys::romdir_dt=configfile::settings.value(configfile::standard::keydef::romdir_dt, defaultval).toString();
    configfile::storagekeys::romdir_dt2=configfile::settings.value(configfile::standard::keydef::romdir_dt2, defaultval).toString();
    configfile::storagekeys::romdir_dt3=configfile::settings.value(configfile::standard::keydef::romdir_dt3, defaultval).toString();

    configfile::storagekeys::savedir_dt=configfile::settings.value(configfile::standard::keydef::savedir_dt, defaultval).toString();
    configfile::storagekeys::savedir_dt2=configfile::settings.value(configfile::standard::keydef::savedir_dt2, defaultval).toString();
    configfile::storagekeys::savedir_dt3=configfile::settings.value(configfile::standard::keydef::savedir_dt3, defaultval).toString();

    configfile::storagekeys::user_dt=configfile::settings.value(configfile::standard::keydef::user_dt, defaultval).toInt();
    configfile::storagekeys::user_dt2=configfile::settings.value(configfile::standard::keydef::user_dt2, defaultval).toInt();
    configfile::storagekeys::user_dt3=configfile::settings.value(configfile::standard::keydef::user_dt3, defaultval).toInt();

    MainWindow w;
    w.show();

    return a.exec();
}
