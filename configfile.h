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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QString>
#include <QSettings>
#include <QDir>

namespace configfile
{
    inline QSettings settings("nastys", "UltraEdit");

    namespace standard
    {
        namespace gameid
        {
            inline QString gameid_dt="NPJB00047";
            inline QString gameid_dt2="NPJB00134";
            inline QString gameid_dt3="NPJB00201";
        };

        namespace titlebar
        {
            inline QString titlebar="";
            inline QString titlebar_2nd=" 2nd";
            inline QString titlebar_extend=" Extend";
        };

        namespace address
        {
            inline long editentry_addr_dt2=0x20010;
            inline long system_usrid_addr_dt2=0x50050;
            inline long system_usrid_addr_dt3=0x54050;
        };

        namespace defaultdir
        {
            inline QString dd_rom_dt=QDir::homePath()+"/.config/rpcs3/dev_hdd0/game/NPJB00047/USRDIR/rom/";
            inline QString dd_rom_dt2=QDir::homePath()+"/.config/rpcs3/dev_hdd0/game/NPJB00134/USRDIR/rom/";
            inline QString dd_rom_dt3=QDir::homePath()+"/.config/rpcs3/dev_hdd0/game/NPJB00201/USRDIR/rom/";
            inline QString dd_save_dt=QDir::homePath()+"/.config/rpcs3/dev_hdd0/home/00000001/savedata/NPJB00047-SYSTEM00/";
            inline QString dd_save_dt2=QDir::homePath()+"/.config/rpcs3/dev_hdd0/home/00000001/savedata/NPJB00134-SYSTEM00/";
            inline QString dd_save_dt3=QDir::homePath()+"/.config/rpcs3/dev_hdd0/home/00000001/savedata/NPJB00201-SYSTEM00/";
        };

        namespace keydef
        {
            inline QString romdir_dt="romdir_dt";
            inline QString romdir_dt2="romdir_dt2";
            inline QString romdir_dt3="romdir_dt3";
            inline QString savedir_dt="savedir_dt";
            inline QString savedir_dt2="savedir_dt2";
            inline QString savedir_dt3="savedir_dt3";
            inline QString user_dt="user_dt";
            inline QString user_dt2="user_dt2";
            inline QString user_dt3="user_dt3";
        };

        namespace savekeys
        {
            inline static unsigned char key_pd1[16]={0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
            inline static unsigned char key_pd2[16]={0x51, 0x52, 0x0F, 0x89, 0x00, 0xA0, 0xC1, 0xF1, 0xFD, 0xB2, 0xA7, 0xDF, 0x5C, 0xBE, 0x4B, 0xBD};
            inline static unsigned char key_pde[16]={0xB2, 0xD2, 0xF4, 0x5C, 0x15, 0x73, 0x59, 0x85, 0x55, 0x2E, 0x8C, 0xBD, 0x2A, 0x11, 0x88, 0x67};
        }
    }

    namespace keys
    {
        inline QString *romdir;
        inline QString *romdir_key;
        inline QString *defaultdir_rom;

        inline QString *savedir;
        inline QString *savedir_key;
        inline QString *defaultdir_save;

        inline QString *gameid;
        inline QString *dtmgr_titlebar;

        inline QStringList *btsong;

        inline bool dtmgr_limited=true;
        inline bool dtmgr_wide=false;

        inline long *editentry_addr;
        inline long *system_usrid_addr;

        inline int *user;
        inline QString *user_key;

        inline bool gl_use_spirv=true;
    };

    namespace storagekeys
    {
        inline QString romdir_dt;
        inline QString romdir_dt2;
        inline QString romdir_dt3;
        inline QString savedir_dt;
        inline QString savedir_dt2;
        inline QString savedir_dt3;
        inline int user_dt;
        inline int user_dt2;
        inline int user_dt3;
    };
};

#endif // CONFIGFILE_H
