#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QMap>

#include "commsinterface.h"
#include "dataentryinterface.h"

namespace Ui {
class AboutDialog;
}

class QTreeWidgetItem;

class AboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void addPlugin(CommsFactoryInterface *iComms, QString filename);
    void addPlugin(CommsFactoryInterface *iComms, GenericComms *commObj);
    void addPlugin(DataEntryFactoryInterface *iData, QString filename);
    void addPlugin(DataEntryFactoryInterface *iData, DataEntry *dataObj);

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *previous);

private:
    Ui::AboutDialog *ui;

    QMap<CommsFactoryInterface *, QTreeWidgetItem * > m_commPluginMap;
    QMap<DataEntryFactoryInterface *, QTreeWidgetItem * > m_dataPluginMap;
};

#endif // ABOUTDIALOG_H
