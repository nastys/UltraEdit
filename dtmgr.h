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

#ifndef DTMGR_H
#define DTMGR_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDir>

namespace Ui {
class dtMgr;
}

class dtMgr : public QDialog
{
    Q_OBJECT

public:
    explicit dtMgr(QWidget *parent = nullptr);
    ~dtMgr();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_comboBox_characterA_currentIndexChanged(int index);

    void on_comboBox_characterB_currentIndexChanged(int index);

    void on_spinBox_moduleA_valueChanged(int arg1);

    void on_spinBox_moduleB_valueChanged(int arg1);

    void on_comboBox_moduleA_currentTextChanged(const QString &arg1);

    void on_comboBox_moduleB_currentTextChanged(const QString &arg1);

    void on_checkBox_moduleB_stateChanged(int arg1);

    void on_pushButton_close_clicked();

    void on_pushButton_apply_clicked();

    void on_radioButton_builtin_clicked();

    void on_pushButton_import_clicked();

    void on_pushButton_export_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_reset_metadata_clicked();

    void on_radioButton_MP3_clicked();

    void on_pushButton_audio_preview_clicked();

    void on_radioButton_none_clicked();

    void on_radioButton_Vorbis_clicked();

    void on_radioButton_adx_clicked();

    void on_radioButton_aix_clicked();

    void on_pushButton_audio_export_clicked();

	void on_radioButton_MP3_2_clicked();

private:
    Ui::dtMgr *ui;

    void songFileCleanup();

    void installSongHack(QString dlgTitle, QString dlgFormat);

    QString getAudioPath();
};

#endif // DTMGR_H
