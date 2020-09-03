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

#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QComboBox>

namespace module
{

    class FileEntry
    {
    public:
        QString name;
        unsigned int offset;
        unsigned int compressedSize;
        unsigned int uncompressedSize;
        long fixedSize;

        //bool isCompressed() {return compressedSize!=uncompressedSize;};
    };

    enum chara
    {
        MIKU,
        RIN,
        LEN,
        LUKA,
        MEIKO,
        KAITO,
        NERU,
        HAKU,
        SAKINE,
        TETO
    };

    class Module
    {
    public:
        QString name;
        QString character;
        int id;
        int sort_index;
    };

    static QStringList moduleTable;
    static QVector<Module> modules;

    bool decModuleTbl();
    bool fillModules();

    void getCharaString(QString &dest, chara character);
    chara getCharaChara(QString string);
    int getModuleId(QString &modulename, QString &character);

    void fillCharacterDropdown(QComboBox *combobox);
    void fillModuleDropdown(QComboBox *combobox, chara &character);
    void findModule(int &id, QComboBox *characterCb, QComboBox *moduleCb);
};

#endif // MODULE_H
