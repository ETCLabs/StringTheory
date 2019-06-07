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

#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include <QTextStream>
#include <QDateTime>
#include <QFont>
#include <QColor>
#include <QIcon>

class DataEntry;

class FilterDataModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilterDataModel(QObject *parent = 0);
    void setDisplayedDirections(int visible);
    int displayedDirections() const;
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent = QModelIndex()) const;
private:
    int m_accepted;
};

class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns{TIMESTAMP, TXLABEL, DATA, MAX_COL};

    static const int DataDirectionRole = Qt::UserRole + 1;
    static const int RawDataRole = DataDirectionRole + 1;
    enum DataDirection
    {
        DataReceived = 1,
        DataSent = 2,
        DataGoodNote = 4,
        DataBadNote = 8,
        DataNote = DataGoodNote | DataBadNote,
        DataRxTx = DataReceived | DataSent,
        DataAny = DataNote | DataRxTx
    };

    explicit DataTableModel(QObject *parent = 0);
    ~DataTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    // flags() as default

    void setDataEntry(DataEntry * dataEntry);

    void saveToTextStream(QTextStream &outStream);

signals:
    
public slots:
    void clear();
    void refreshData();

    void gotReceivedData(const QDateTime timestamp,
                         const QString &source,
                         const QString &dest,
                         QByteArray data);

    void gotSentData(const QDateTime timestamp,
                     const QString &source,
                     const QString &dest,
                     QByteArray data);

    void gotNote(const QDateTime timestamp,
                 const QString &note,
                 const QString &info,
                 bool good);

private:
    struct DataTableModelRow
    {
        QDateTime timestamp;
        QString txLabel;
        DataDirection direction;
        QByteArray data;
    };

    // Storage of history data
    QList<DataTableModelRow> *m_data;

    // Pointer to data entry widget
    DataEntry * m_dataEntry;
    // Storage of current formatting
    QFont m_receivedFont;
    QColor m_receivedColor;
    QFont m_sentFont;
    QColor m_sentColor;
    QFont m_goodNoteFont;
    QColor m_goodNoteColor;
    QFont m_badNoteFont;
    QColor m_badNoteColor;

    QFont rowFont(DataDirection direction) const;
    QColor rowColor(DataDirection direction) const;
    QIcon rowIcon(DataDirection direction) const;
    QVariant timestamp(const int row, int role) const;
    QVariant txLabel(const int row, int role) const;
    QVariant bytes(const int row, int role) const;
};

#endif // DATATABLEMODEL_H
