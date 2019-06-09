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

#include "datatablemodel.h"
#include "dataentry.h"

#include <QDebug>

DataTableModel::DataTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_data = new QList<DataTableModelRow>;
    m_dataEntry = 0;
    m_receivedFont;
    m_receivedColor = Qt::black;
    m_sentFont.setItalic(true);
    m_sentColor = Qt::darkGray;
    m_goodNoteFont;
    m_goodNoteColor = Qt::darkGreen;
    m_badNoteFont;
    m_badNoteColor = Qt::darkRed;
}

DataTableModel::~DataTableModel()
{
    delete m_data;
}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_data->count();
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return MAX_COL;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case TIMESTAMP:
            return tr("Timestamp");

        case TXLABEL:
            return tr("Source/Dest");

        case DATA: return tr("Data");

        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if ((index.row() >= m_data->count()) ||
            (index.row() < 0))
        return QVariant();

    if (role == DataDirectionRole)
        return m_data->at(index.row()).direction;

    if (role == RawDataRole)
        return m_data->at(index.row()).data;

    switch (index.column())
    {
    case TIMESTAMP: return timestamp(index.row(), role);
    case TXLABEL: return txLabel(index.row(), role);
    case DATA: return bytes(index.row(), role);
    default:
        return QVariant();
    }
}

void DataTableModel::setDataEntry(DataEntry *dataEntry)
{
    m_dataEntry = dataEntry;
    refreshData();
}

void DataTableModel::refreshData()
{
    emit dataChanged(index(0,2), index(rowCount()-1, 2));
}

void DataTableModel::saveToTextStream(QTextStream &outStream)
{
    // Check can do anything
    if (m_dataEntry)
    {
        // Configure padding
//        outStream.setFieldWidth(20);
        QString dirChar;
        for(int i=0; i < m_data->count(); ++i)
        {
            switch(m_data->at(i).direction)
            {
            case DataReceived:  dirChar = " Rx : ";
                break;
            case DataSent:      dirChar = " Tx : ";
                break;
            case DataGoodNote:  dirChar = "Info: ";
                break;
            case DataBadNote:   dirChar = "Warn: ";
            default:;
            }
            outStream << m_data->at(i).timestamp.toString("hh:mm:ss:zzz ")
                      << dirChar
                      << m_data->at(i).txLabel;
            if (m_data->at(i).data.length() != 0)
            {
                outStream << " '"
                          << m_dataEntry->toString(m_data->at(i).data)
                          << "'";
            }
            outStream << '\n';
        }
    }
    else
    {
        outStream << tr("Unable to output, no formatting exists");
    }
}

void DataTableModel::clear()
{
    beginResetModel();
    m_data->clear();
    endResetModel();
}

void DataTableModel::gotReceivedData(const QDateTime timestamp,
                                     const QString &source,
                                     const QString &dest,
                                     QByteArray data)
{
    Q_UNUSED(dest);
    DataTableModelRow newRow;
    newRow.timestamp = timestamp;
    newRow.data = data;
    newRow.txLabel = source;
    newRow.direction = DataReceived;

    beginInsertRows(QModelIndex(), 0, 0);
    m_data->prepend(newRow);
    endInsertRows();
}

void DataTableModel::gotSentData(const QDateTime timestamp,
                                 const QString &source,
                                 const QString &dest,
                                 QByteArray data)
{
    Q_UNUSED(source);
    DataTableModelRow newRow;
    newRow.timestamp = timestamp;
    newRow.data = data;
    newRow.txLabel = dest;
    newRow.direction = DataSent;

    beginInsertRows(QModelIndex(), 0, 0);
    m_data->prepend(newRow);
    endInsertRows();
}

void DataTableModel::gotNote(const QDateTime timestamp,
                             const QString &note,
                             const QString &info,
                             bool good)
{
    DataTableModelRow newRow;
    newRow.timestamp = timestamp;
    newRow.txLabel = note;
    newRow.data = info.toLatin1();
    if (good)
        newRow.direction = DataGoodNote;
    else
        newRow.direction = DataBadNote;

    beginInsertRows(QModelIndex(), 0, 0);
    m_data->prepend(newRow);
    endInsertRows();
}

// Formatting
QFont DataTableModel::rowFont(DataDirection direction) const
{
    switch (direction)
    {
    case DataReceived: return m_receivedFont;
    case DataSent: return m_sentFont;
    case DataGoodNote: return m_goodNoteFont;
    case DataBadNote: return m_badNoteFont;
    default:
        return QFont();
    }
}

QColor DataTableModel::rowColor(DataDirection direction) const
{
    switch (direction)
    {
    case DataReceived: return m_receivedColor;
    case DataSent: return m_sentColor;
    case DataGoodNote: return m_goodNoteColor;
    case DataBadNote: return m_badNoteColor;
    default:
        return QColor();
    }
}

QIcon DataTableModel::rowIcon(DataDirection direction) const
{
    switch (direction)
    {
    case DataReceived: return QIcon(":/received.png");
    case DataSent: return QIcon(":/sent.png");
    case DataGoodNote: return QIcon(":/info.png");
    case DataBadNote: return QIcon(":/warning.png");
    default:
        return QIcon();
    }
}

QVariant DataTableModel::timestamp(const int row, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole: return QVariant(m_data->at(row).timestamp);
    case Qt::FontRole: return rowFont(m_data->at(row).direction);
    case Qt::ForegroundRole: return rowColor(m_data->at(row).direction);
    default:
        return QVariant();
    }
}

QVariant DataTableModel::txLabel(const int row, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole: return QVariant(m_data->at(row).txLabel);
    case Qt::FontRole: return rowFont(m_data->at(row).direction);
    case Qt::ForegroundRole: return rowColor(m_data->at(row).direction);
    case Qt::DecorationRole: return rowIcon(m_data->at(row).direction);
    default:
        return QVariant();
    }
}

QVariant DataTableModel::bytes(const int row, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        if (m_data->at(row).direction & DataNote)
            return QVariant(m_data->at(row).data);
        if (m_dataEntry)
            return QVariant(m_dataEntry->toString(m_data->at(row).data));
        return QVariant(m_data->at(row).data);
        break;
    case Qt::ToolTipRole:
        if (m_data->at(row).direction & DataRxTx)
            return tr("Doubleclick to copy into 'Data to send'");
    }
    return QVariant();
}

FilterDataModel::FilterDataModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    m_accepted = DataTableModel::DataAny;
}

void FilterDataModel::setDisplayedDirections(int visible)
{
    m_accepted = visible;
    invalidateFilter();
}

int FilterDataModel::displayedDirections() const
{
    return m_accepted;
}

bool FilterDataModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    int tmpDest = sourceModel()->data(sourceModel()->index(source_row, 0, source_parent),
                                      DataTableModel::DataDirectionRole).toInt();
    return (tmpDest & m_accepted);
}
