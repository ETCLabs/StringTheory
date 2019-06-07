#ifndef CORETOOLS_H
#define CORETOOLS_H

#include "commsinterface.h"
#include "dataentryinterface.h"

class CoreTools : public QObject,
        public CommsFactoryInterface,
        public DataEntryFactoryInterface
{
    Q_OBJECT
    Q_INTERFACES(CommsFactoryInterface DataEntryFactoryInterface)
    Q_PLUGIN_METADATA(IID CommsInterfacePlugin_iid)
    //Q_PLUGIN_METADATA(IID DataEntryInterfacePlugin_iid)

public:
    QString pluginName() {return QString("Core Tools");}
    float pluginVersion() {return 3.0f;}
    QString pluginAbout();

    QList<GenericComms *> communicationDevices();
    QList<DataEntry *> dataEntryMethods();
};

#endif // CORETOOLS_H
