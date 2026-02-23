# Howto Use Modeled Messages in Applications

## Create a Modeled Message
* modeled messages are located in ../../dlt/DltMessage
* if the modeled message is used only by this application use filename <applicationname>-modeledmsg.arxml
* add this file to APD_DLTMESSAGE_ARXML_FILES in ../../dlt/CMakeLists.txt
## Create a Log and Trace Interface and link to it in the Application Sw Component Type
* create a package with an element LogAndTraceInterface in <applicationname>.arxml
* add the namespace to this interface
* create a RPortPrototype in the in the ApplicationSwComponentType and link it to the LogAndTraceInterface
## Map the Collection Set to RPortPrototype
* in ../../dlt/DltMessage/<applicationname>-modeledmsg.arxml
* map the LogAndTraceMessageCollectionSet to th RPortPrototype of the ApplicationSwComponentType using LtMessageCollectioToPortPrototypeMapping
* at this point the header file for messages are generated in oe-workdir/apd-<machine>-<application>-<version>/gen/include when doing a bitbake apd-<machine>-<application>
## Use Modeled Message
* #include "<namespaces>/messages.h"
* call logger.Log( <messagename>Msg, <parameters of message with correct type> );
## Add Trace Switch Configuration
* in <applicationname>.arxml
* for each modeled message that has to be routed different to log add a TraceSwitchConfiguration to the Executable/TraceSwitchConfigurations
* this TraceSwitchConfiguration has to have the reference to the message in question, the namespaces and the switch itself (TraceSwitch{Arti,ArtiAndLog,Log,None})
## Specialize the TraceArti
* add a file ara/log/ext/trace_arti_spec.h with the specialized ara::log::ext::TraceArti(<messagename>Msg, <parameters of messae with correct type>)
* the file has to be includable from the include directories, i.e. ../include/ara/log/ext/trace_arti_spec.h
* bug: currently folder/namespace ext is not used, so remove it from above

