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

#include <QFile>
#include <QDataStream>
#include <qgzdec.h>
#include <module.h>
#include <configfile.h>

namespace module
{
    bool decModuleTbl()
    {
        moduleTable.clear();

        QFile file(*configfile::keys::romdir+"/gm_module_tbl.farc");
        if(!file.open(QIODevice::ReadOnly)) return 1;
        QDataStream filestream(&file);
        filestream.setByteOrder(QDataStream::BigEndian);

        char signature[4];
        file.read(signature, 4);
        if(strcmp(signature, "FArC") != 0) return 1;

        int fileListSize;
        filestream >> fileListSize;

        int alignment;
        filestream >> alignment;

        while(file.pos() < fileListSize + 0x8)
        {
            FileEntry fe;

            char c;
            do
            {
                file.read(&c, 1);
                fe.name.append(c);
            }
            while(c != '\0');

            filestream >> fe.offset;
            filestream >> fe.compressedSize;
            filestream >> fe.uncompressedSize;

            fe.fixedSize = qMin(static_cast<long long>(fe.compressedSize), file.size() - fe.offset);

            file.seek(fe.offset);
            QByteArray decompressed;
            gzDecompress(file.read(fe.compressedSize), decompressed);
            moduleTable=QString::fromUtf8(decompressed).split('\n');
        }

        if(moduleTable.isEmpty()) return 1;
        return fillModules();
    }

    bool fillModules()
    {
        if(!modules.isEmpty()) modules.clear();

        Module firstModule;
        modules.append(firstModule);
        for(int pos=0; pos<moduleTable.size(); pos++)
        {
            #define currModule modules[modules.length()-1]

            QString currLine=moduleTable.at(pos).simplified();
            QStringList splitLine=currLine.split('.');
            if(splitLine.size()<3) continue;

            int currNum=splitLine.at(1).toInt();
            static int lastNum=currNum;
            if(currNum!=lastNum)
            {
                Module nextModule;
                modules.append(nextModule);
                lastNum=currNum;
            }

            QStringList currFieldAndValue=splitLine.at(2).split('=');
            const QString currField=currFieldAndValue.at(0);
            const QString currValue=currFieldAndValue.at(1);

            if(currField=="chara") currModule.character=currValue;
            else if(currField=="name") currModule.name=currValue;
            else if(currField=="id") currModule.id=currValue.toInt();
            else if(currField=="sort_index")
            {
                currModule.sort_index=currValue.toInt();
                if(currModule.sort_index==0) currModule.sort_index=INT_MAX;
            }
        }

        if(modules.isEmpty()) return 1;

        for(int i=1; i<modules.size(); i++) for(int j=i; j>0 && modules.at(j-1).sort_index > modules.at(j).sort_index; j--)
        {
            auto tmp=modules.at(j);
            modules[j]=modules.at(j-1);
            modules[j-1]=tmp;
        }

        return 0;
    }

    void getCharaString(QString &dest, chara character)
    {
        switch(character)
        {
        default:
        case MIKU:
            dest="MIKU";
            break;
        case RIN:
            dest="RIN";
            break;
        case LEN:
            dest="LEN";
            break;
        case LUKA:
            dest="LUKA";
            break;
        case MEIKO:
            dest="MEIKO";
            break;
        case KAITO:
            dest="KAITO";
            break;
        case NERU:
            dest="NERU";
            break;
        case HAKU:
            dest="HAKU";
            break;
        case SAKINE:
            dest="SAKINE";
        }

        return;
    }

    chara getCharaChara(QString string)
    {
             if(string=="MIKU") return MIKU;
        else if(string=="RIN") return RIN;
        else if(string=="LEN") return LEN;
        else if(string=="LUKA") return LUKA;
        else if(string=="MEIKO") return MEIKO;
        else if(string=="KAITO") return KAITO;
        else if(string=="NERU") return NERU;
        else if(string=="HAKU") return HAKU;
        else if(string=="SAKINE") return SAKINE;

        return MIKU;
    }

    int getModuleId(QString &modulename, QString &character)
    {
        for(int i=0; i<modules.size(); i++)
        {
            if(modules.at(i).name==modulename&&modules.at(i).character==character)
            {
                return modules.at(i).id;
            }
        }

        return -1;
    }

    void fillCharacterDropdown(QComboBox *combobox)
    {
        combobox->addItems({"初音ミク", "鏡音リン", "鏡音レン", "巡音ルカ", "メイコ", "カイト", "亞北ネル", "弱音ハク", "咲音メイコ"});

        return;
    }

    void fillModuleDropdown(QComboBox *combobox, chara &character)
    {
        QString charastring;
        getCharaString(charastring, character);

        combobox->clear();

        for(int i=0; i<modules.size(); i++)
            if(modules.at(i).character==charastring)
                combobox->addItem(modules.at(i).name);
    }

    void findModule(int &id, QComboBox *characterCb, QComboBox *moduleCb)
    {
        int modpos=-1;
        for(int i=0; i<modules.size(); i++)
        {
            if(modules.at(i).id==id)
            {
                modpos=i;
                break;
            }
        }
        if(modpos==-1) return;
        characterCb->setCurrentIndex(getCharaChara(modules.at(modpos).character));
        moduleCb->setCurrentText(modules.at(modpos).name);
    }
};
