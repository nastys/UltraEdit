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

#ifndef SAVEGEN_H
#define SAVEGEN_H

#include <QString>
#include <QStringList>
#include <QVector>

namespace paramSfo
{
    enum DataType
    {
        utf8s=0x04,
        utf8=0x0204,
        int32=0x0404
    };

    struct Header
    {
        unsigned int magic=1179865088;
        unsigned int version=257;
        unsigned int key_table_offset=0;
        unsigned int data_table_offset=0;
        unsigned int entry_count=0;
    };

    struct IndexTable
    {
        unsigned short key_offset=0;
        unsigned short data_type=0;
        unsigned int data_size_used=0;
        unsigned int data_size_total=0;
        unsigned int data_offset=0;
    };

    class ParamSfo
    {
    public:
        Header header;
        QStringList keys;
        QByteArrayList data;
        QVector<IndexTable> indexTables;

        void addKey(QString key, QByteArray &data, DataType type);
        void exportSfo(QString filepath);
        void clear();

        ~ParamSfo(){clear();}
    };

    void generateEditSave(QString editNumber, QString &editName);
    void generateEditSave(int editNumber, QString &editName);

    void deleteEditSave(QString editNumber);
    void deleteEditSave(int editNumber);
};

#endif // SAVEGEN_H
