#include "coretools.h"

#include <QtPlugin>

// Communication plugins
#include "serialcomms.h"
#include "udpcomms.h"
#include "tcpcomms.h"

// Data entry plugins
#include "escformat.h"
#include "hexformat.h"
#include "congoformat.h"
#include "rawhexformat.h"


QString CoreTools::pluginAbout()
{
    return QString("<p>Copyright (C) 2011-2013 ETC Ltd<br/>All rights reserved</p>");
}

QList<GenericComms *> CoreTools::communicationDevices()
{
    QList<GenericComms *> result;
    result.append(new UdpComms());
    result.append(new TcpComms());
    result.append(new SerialComms());
    return result;
}

QList<DataEntry *> CoreTools::dataEntryMethods()
{
    QList<DataEntry *> result;
    result.append(new EscFormat());
    result.append(new HexFormat());
    result.append(new CongoFormat());
    result.append(new RawHexFormat());
    return result;
}
