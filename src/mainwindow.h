/*
Copyright (c) 2010, Electronic Theatre Controls, Ltd.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of Electronic Theatre Controls, Ltd. nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDateTime>
#include <QDir>

#include "datatablemodel.h"

#include "dataentryinterface.h"
#include "commsinterface.h"

namespace Ui {
    class MainWindow;
}

class AboutDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    DataTableModel *m_dataModel;
    FilterDataModel *m_filterDataModel;

    void loadPlugins();
    void loadPluginFolder(QDir pluginsDir);
    void handlePlugin(QObject *plugin, QString fileName);

    // Communication Handler List
    QList<GenericComms *> m_commsHandlers;

    int m_currentComm;

    // Data entry handlers
    QList<DataEntry *> m_dataHandlers;
    int m_currentData;

    AboutDialog *m_about;

    QDateTime logStartTime;

    bool m_autoSend;

private slots:

    void on_btnSend_clicked();
    void on_actionClear_triggered();
    void on_actionSave_triggered();

    void on_cmbModeSelect_currentIndexChanged(int index);

    void on_actionShow_Sent_triggered(bool checked);
    void on_actionAbout_triggered();
    void on_cmbDataSelect_currentIndexChanged(int index);
    void on_actionShow_Received_triggered(bool checked);
    void on_actionShow_Info_triggered(bool checked);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_actionAutoSend_toggled(bool checked);
};

#endif // MAINWINDOW_H
