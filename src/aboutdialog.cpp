#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "version.h"

enum COLUMNS
{
    COL_MAIN,
    COL_ABOUT
};

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle(tr("About %1").arg(qApp->applicationName()));
    ui->lblAppName->setText(qApp->applicationName());
    ui->lblVersion->setText(tr("Version %1").arg(qApp->applicationVersion()));
    ui->lblQt->setText(tr("<html><head/><body><p>This application uses Qt v%1 by the Qt Project, "
                          "which is licenced under the "
                          "<a href=""http://www.gnu.org/licenses/lgpl-2.1.txt"">"
                          "<span style=""text-decoration: underline; color:#0000ff;"">"
                          "GNU LGPL licence</span></a>.</p>"
                          "<p>For more information on Qt, click <a href=""https://qt-project.org"">"
                          "<span style=""text-decoration: underline; color:#0000ff;"">"
                          "here</span></a>.</p></body></html>")
                       .arg(qVersion()));
    QString bsdLicence = tr("<html><head/><body><p><b>%1<br>All rights reserved.</b></p>"
                            "<p>Redistribution and use in source and binary forms, with or without "
                            "modification, are permitted provided that the following conditions are "
                            "met:"
                            "<ul>"
                            "<li>Redistributions of source code must retain the above copyright "
                            "notice, this list of conditions and the following disclaimer.</li>"
                            "<li>Redistributions in binary form must reproduce the above copyright "
                            "notice, this list of conditions and the following disclaimer in the "
                            "documentation and/or other materials provided with the distribution.</li>"
                            "<li>Neither the name of %2 nor the names of "
                            "its contributors may be used to endorse or promote products derived "
                            "from this software without specific prior written permission.</li>"
                            "</ul></p>"
                            "<p>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ""AS "
                            "IS"" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED "
                            "TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
                            "PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER "
                            "OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, "
                            "EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, "
                            "PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR "
                            "PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF "
                            "LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING "
                            "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS "
                            "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</p>"
                            "</body></html>")
            .arg(VER_LEGALCOPYRIGHT_STR)
            .arg(VER_COMPANYNAME_STR);
    ui->txtLicence->setHtml(bsdLicence);

    // Connect slots
    connect(ui->treeComms, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui->treeDataEntry, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::addPlugin(CommsFactoryInterface *iComms, QString filename)
{
    QTreeWidgetItem *pluginItem = new QTreeWidgetItem(ui->treeComms);
    m_commPluginMap.insert(iComms, pluginItem);

    pluginItem->setToolTip(COL_MAIN, filename);
    pluginItem->setText(COL_MAIN, tr("%1 v%2")
                       .arg(iComms->pluginName())
                       .arg(iComms->pluginVersion(), 0, 'f', 1));
    QFont boldFont = pluginItem->font(COL_MAIN);
    boldFont.setBold(true);
    pluginItem->setFont(COL_MAIN, boldFont);
    pluginItem->setExpanded(true);
    pluginItem->setText(COL_ABOUT, iComms->pluginAbout());
}

void AboutDialog::addPlugin(CommsFactoryInterface *iComms, GenericComms *commObj)
{
    QTreeWidgetItem *parent = m_commPluginMap.value(iComms);
    if (!parent)
        return;
    QTreeWidgetItem *pluginItem = new QTreeWidgetItem(parent);
    pluginItem->setText(COL_MAIN, tr("%1 v%2")
                        .arg(commObj->commType())
                        .arg(commObj->commVersion(), 0, 'f', 1));
}

void AboutDialog::addPlugin(DataEntryFactoryInterface *iData, QString filename)
{
    QTreeWidgetItem *pluginItem = new QTreeWidgetItem(ui->treeDataEntry);
    m_dataPluginMap.insert(iData, pluginItem);

    pluginItem->setToolTip(COL_MAIN, filename);
    pluginItem->setText(COL_MAIN, tr("%1 v%2")
                       .arg(iData->pluginName())
                       .arg(iData->pluginVersion(), 0, 'f', 1));
    QFont boldFont = pluginItem->font(COL_MAIN);
    boldFont.setBold(true);
    pluginItem->setFont(COL_MAIN, boldFont);
    pluginItem->setExpanded(true);
    pluginItem->setText(COL_ABOUT, iData->pluginAbout());
}

void AboutDialog::addPlugin(DataEntryFactoryInterface *iData, DataEntry *dataObj)
{
    QTreeWidgetItem *parent = m_dataPluginMap.value(iData);
    if (!parent)
        return;
    QTreeWidgetItem *pluginItem = new QTreeWidgetItem(parent);
    pluginItem->setText(COL_MAIN, tr("%1 v%2")
                        .arg(dataObj->dataEntryName())
                        .arg(dataObj->dataEntryVersion(), 0, 'f', 1));
}

void AboutDialog::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (!current)
    {
        ui->txtPlugLicence->clear();
        return;
    }
    QTreeWidgetItem * parent = current->parent();
    if (!parent) parent = current;
    ui->txtPlugLicence->setHtml(QString("<html><head/><body><p><b>%1</b></p>%2</body></html>")
                                        .arg(parent->text(COL_MAIN),
                                             parent->text(COL_ABOUT)));
}
