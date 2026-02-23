#include <dds/DdsDcpsPublicationC.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include "MessengerTypeSupportImpl.h"
#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        // 1. 서비스 초기화
        DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);
        if (!dpf)
        { // 이 체크문이 없어서 15번 라인에서 죽는 것입니다.
            std::cerr << "Error: Could not get DomainParticipantFactory!" << std::endl;
            return 1;
        }
        DDS::DomainParticipant_var participant = dpf->create_participant(42, PARTICIPANT_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        // 2. TypeSupport 등록
        MessageTypeSupport_var ts = new MessageTypeSupportImpl();
        ts->register_type(participant, "");

        // 3. Topic 생성
        CORBA::String_var type_name = ts->get_type_name();
        DDS::Topic_var topic = participant->create_topic("MyTopic", type_name, TOPIC_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        // 4. Publisher 및 DataWriter 생성
        DDS::Publisher_var pub = participant->create_publisher(PUBLISHER_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        DDS::DataWriter_var dw = pub->create_datawriter(topic, DATAWRITER_QOS_DEFAULT, 0, OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        MessageDataWriter_var message_dw = MessageDataWriter::_narrow(dw);

        // 5. 데이터 전송 (10번 반복)
        Message msg;
        msg.id = 0;
        for (int i = 0; i < 10; ++i)
        {
            msg.id++;
            msg.contents = "Hello OpenDDS from Target!";
            std::cout << "Sending Message ID: " << msg.id << std::endl;
            message_dw->write(msg, DDS::HANDLE_NIL);
            ACE_OS::sleep(1);
        }

        participant->delete_contained_entities();
        dpf->delete_participant(participant);
        TheServiceParticipant->shutdown();
    }
    catch (const CORBA::Exception &e)
    {
        std::cerr << "Exception caught in main: " << e << std::endl;
        return 1;
    }
    return 0;
}
