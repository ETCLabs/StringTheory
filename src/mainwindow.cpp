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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"

#include "version.h"

#include <QPluginLoader>

#include <QTime>
#include <QSpinBox>
#include <QFileDialog>
#include <QTextStream>
#include <QListWidgetItem>

#include <QMessageBox>

#include <QDebug>

#include "textformat.h"

const QString timeFormat = "hh:mm:ss.zzz";
const int typeRx   = 1001;
const int typeSent = 1002;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_currentComm(-1)  // No comms handlers yet
{
    m_about = new AboutDialog();

    // Default data handler that always exists
    DataEntry *tmpHandler = new TextFormat();
    m_dataHandlers.append(tmpHandler);
    m_currentData = 0;   // Default to TextFormat

    loadPlugins();

    // Setup user interface
    ui->setupUi(this);
    ui->toolBtnAutoSend->setDefaultAction(ui->actionAutoSend);
    m_autoSend = ui->actionAutoSend->isChecked();

    // Set title etc
    this->setWindowTitle(QString("%1 v%2")
                         .arg(qApp->applicationName(), qApp->applicationVersion()));

    m_dataModel = new DataTableModel(this);
    m_dataModel->setDataEntry(m_dataHandlers[m_currentData]);

    m_filterDataModel = new FilterDataModel(this);
    m_filterDataModel->setSourceModel(m_dataModel);

    ui->tableView->setModel(m_filterDataModel);

    // Add plugin data handlers to dropdown and stackedwidget
    foreach(tmpHandler, m_dataHandlers)
    {
        ui->stackedData->addWidget(tmpHandler);
        ui->cmbDataSelect->addItem(QString("%1 (v%2)")
                                   .arg(tmpHandler->dataEntryName())
                                   .arg(tmpHandler->dataEntryVersion(),0, 'f', 1));
        connect(tmpHandler, SIGNAL(transmit()),
                this, SLOT(on_btnSend_clicked()));
        connect(tmpHandler, SIGNAL(stringFormatChanged()),
                m_dataModel, SLOT(refreshData()));
    }

    // Add to dropdown and stackedwidget
    GenericComms *tmpComms;
    foreach(tmpComms, m_commsHandlers)
    {
        ui->cmbModeSelect->addItem(QString("%1 (v%2)")
                                           .arg(tmpComms->commType())
                                           .arg(tmpComms->commVersion(), 0, 'f', 1));
        ui->stackedWidget->addWidget(tmpComms->configWidget());
        connect(tmpComms, SIGNAL(note(QDateTime,QString,QString,bool)),
                m_dataModel, SLOT(gotNote(QDateTime,QString,QString,bool)));
        connect(tmpComms, SIGNAL(receivedData(QDateTime,QString,QString,QByteArray)),
                m_dataModel, SLOT(gotReceivedData(QDateTime,QString,QString,QByteArray)));
        connect(tmpComms, SIGNAL(sentData(QDateTime,QString,QString,QByteArray)),
                m_dataModel, SLOT(gotSentData(QDateTime,QString,QString,QByteArray)));
    }

    // Initial Bind to first option
    if (m_commsHandlers.count() > 0)
    {
        ui->cmbModeSelect->setCurrentIndex(0);
    }
    else
    {
        m_dataModel->gotNote(QDateTime::currentDateTime(),
                             tr("No Serial Plugins Found!"),
                             tr("Check your /plugins folder"),
                             false);
    }

    logStartTime = QDateTime::currentDateTime();
    ui->tableView->resizeColumnsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug() << "Closing down";
    qDeleteAll(m_commsHandlers);
    qDeleteAll(m_dataHandlers);
}

void MainWindow::loadPlugins()
{
    // Discover plugins and add their contents to the list of comms handlers
    QDir pluginsDir = QDir::currentPath();
    pluginsDir.setNameFilters(QStringList("stPlugin_*"));
    loadPluginFolder(pluginsDir);
}

void MainWindow::loadPluginFolder(QDir pluginsDir)
{
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        qDebug() << "Trying to load:" << pluginsDir.absoluteFilePath(fileName);
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
            handlePlugin(plugin, fileName);
        else
            qDebug() << "Plugin Error:" << loader.errorString();
    }
}

void MainWindow::handlePlugin(QObject *plugin, QString fileName)
{
    CommsFactoryInterface *iComms = qobject_cast<CommsFactoryInterface *>(plugin);
    DataEntryFactoryInterface *iDataEntry = qobject_cast<DataEntryFactoryInterface *>(plugin);
    if (iComms)
    {
        // TODO: Check version!
        m_about->addPlugin(iComms, fileName);
        QList<GenericComms *> pluginCommsHandlers = iComms->communicationDevices();
        GenericComms *tmpComms;
        foreach(tmpComms, pluginCommsHandlers)
        {
            m_about->addPlugin(iComms, tmpComms);
            m_commsHandlers.append(tmpComms);
        }
    }
    if (iDataEntry)
    {
        // TODO: Check version!
        m_about->addPlugin(iDataEntry, fileName);
        QList<DataEntry *> pluginDataEntryHandlers = iDataEntry->dataEntryMethods();
        DataEntry *tmpDataEntry;
        foreach(tmpDataEntry, pluginDataEntryHandlers)
        {
            m_about->addPlugin(iDataEntry, tmpDataEntry);
            m_dataHandlers.append(tmpDataEntry);
        }
    }
}

void MainWindow::on_btnSend_clicked()
{
    if (m_currentComm != -1)
        m_commsHandlers[m_currentComm]->transmit(m_dataHandlers[m_currentData]->byteArray());
}

void MainWindow::on_actionClear_triggered()
{
    m_dataModel->clear();
    logStartTime = QDateTime::currentDateTime();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Log As",
                                                    "", "Text Files (*.txt)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;
    QTextStream out(&file);
    // Header
    out << "StringTheory Log File" << '\n';
    out << "=====================" << '\n';
    out << "Logging Started At: " << logStartTime.toString("yyyy MMM dd - hh:mm:ss:zzz") << '\n';
    out << "File Saved At:      " << QDateTime::currentDateTime().toString("yyyy MMM dd - hh:mm:ss:zzz") << '\n';
    out << "File Data Format: " << m_dataHandlers[m_currentData]->dataEntryName() << '\n';
    out << "==================" << '\n';

    m_dataModel->saveToTextStream(out);
}

void MainWindow::on_cmbModeSelect_currentIndexChanged(int index)
{
    // Ensure sanity
    if ((index < 0) ||
        (index == m_currentComm))
        return;
    if (index >= m_commsHandlers.count())
    {
        m_dataModel->gotNote(QDateTime::currentDateTime(),
                             tr("Application Error!"),
                             tr("Non-existent communication handler #%1 - report this!").arg(index),
                             false);
        return;
    }

    if (m_currentComm >= 0)
    {
        m_commsHandlers[m_currentComm]->close();
    }
    m_commsHandlers[index]->open();
    ui->btnSend->setEnabled(true);  // Enable sending

    m_currentComm = index;
}

void MainWindow::on_actionShow_Sent_triggered(bool checked)
{
    // Show or hide all Sent items in the list
    if (checked)
        m_filterDataModel->setDisplayedDirections(m_filterDataModel->displayedDirections() | DataTableModel::DataSent);
    else
        m_filterDataModel->setDisplayedDirections(m_filterDataModel->displayedDirections() & (~DataTableModel::DataSent));
}

void MainWindow::on_actionShow_Received_triggered(bool checked)
{
    // Show or hide all Received items in the list
    if (checked)
        m_filterDataModel->setDisplayedDirections(m_filterDataModel->displayedDirections() | DataTableModel::DataReceived);
    else
        m_filterDataModel->setDisplayedDirections(m_filterDataModel->displayedDirections() & (~DataTableModel::DataReceived));
}

void MainWindow::on_actionShow_Info_triggered(bool checked)
{
    // Show or hide all Info items in the list
    if (checked)
        m_filterDataModel->setDisplayedDirections(m_filterDataModel->displayedDirections() | DataTableModel::DataNote);
    else
        m_filterDataModel->setDisplayedDirections(m_filterDataModel->displayedDirections() & (~DataTableModel::DataNote));
}

void MainWindow::on_actionAbout_triggered()
{
    m_about->show();
}

void MainWindow::on_cmbDataSelect_currentIndexChanged(int index)
{
    // Ensure sanity
    if ((index < 0) ||
        (index == m_currentData))
        return;
    if (index >= m_dataHandlers.count())
    {
        m_dataModel->gotNote(QDateTime::currentDateTime(),
                             tr("Application error!"),
                             tr("Non-existent text entry handler #%1 - report this!").arg(index),
                             false);
        return;
    }

    m_dataHandlers[index]->setByteArray(m_dataHandlers[m_currentData]->byteArray());
    ui->stackedData->setCurrentIndex(index);
    m_dataModel->setDataEntry(m_dataHandlers[index]);

    m_currentData = index;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid() &&
            (index.column() == DataTableModel::DATA) &&
            (index.data(DataTableModel::DataDirectionRole).toInt() & DataTableModel::DataRxTx))
    {   // Is data that was sent or received
        m_dataHandlers[m_currentData]->setByteArray(index.data(DataTableModel::RawDataRole).toByteArray());
        if (m_autoSend)
            on_btnSend_clicked();
    }
}

void MainWindow::on_actionAutoSend_toggled(bool checked)
{
    m_autoSend = checked;
}
