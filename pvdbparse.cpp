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

#include <pvdbparse.h>
#include <configfile.h>

namespace pvdbparse
{
    bool parsePvDbSongList()
    {
        parsedSongList.clear();

        QFile file(*configfile::keys::romdir+"/pv_db.txt");
        if(!file.open(QIODevice::ReadOnly)) return 1;

        QString lastId="";
        while(!file.atEnd())
        {
            QString line = file.readLine();
            if(line.startsWith('#')) continue;
            QStringList splitLine = line.split('.');
            if(splitLine.length()<2) continue;
            const QString currId=splitLine.at(0);
            if(currId!=lastId)
            {
                ParsedSong ps;
                parsedSongList.append(ps);
                lastId=currId;
            }
            QStringList eqSplitLine = line.split('=');
            if(eqSplitLine.length()!=2) continue;

            if(splitLine.at(1).startsWith("song_name=")) parsedSongList.last().title=eqSplitLine.at(1).simplified();
            else if(splitLine.at(1).startsWith("song_file_name=")) parsedSongList.last().filename=eqSplitLine.at(1).simplified();
        }

        file.close();
        return 0;
    }
};
