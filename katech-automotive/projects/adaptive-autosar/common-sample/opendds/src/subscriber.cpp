#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include "MessengerTypeSupportImpl.h"
#include <iostream>
#include <ace/OS_NS_time.h>

int main(int argc, char *argv[])
{
    try
    {
        // 1. ParticipantFactory 초기화
        DDS::DomainParticipantFactory_var dpf = TheParticipantFactoryWithArgs(argc, argv);
        if (!dpf)
        { // 이 체크문이 없어서 15번 라인에서 죽는 것입니다.
            std::cerr << "Error: Could not get DomainParticipantFactory!" << std::endl;
            return 1;
        }

        // 2. DomainParticipant 생성
        DDS::DomainParticipant_var participant = dpf->create_participant(
            42,
            PARTICIPANT_QOS_DEFAULT,
            0,
            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        // [중요] 생성 실패 시 예외 처리 추가
        if (!participant)
        {
            std::cerr << "ERROR: create_participant failed! Check your discovery settings (rtps.ini)." << std::endl;
            return 1;
        }

        // 3. TypeSupport 등록
        MessageTypeSupport_var ts = new MessageTypeSupportImpl();
        if (ts->register_type(participant, "") != DDS::RETCODE_OK)
        {
            std::cerr << "ERROR: register_type failed!" << std::endl;
            return 1;
        }

        // 4. Topic 생성
        CORBA::String_var type_name = ts->get_type_name();
        DDS::Topic_var topic = participant->create_topic(
            "MyTopic",
            type_name,
            TOPIC_QOS_DEFAULT,
            0,
            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!topic)
        {
            std::cerr << "ERROR: create_topic failed!" << std::endl;
            return 1;
        }

        // 5. Subscriber 생성
        DDS::Subscriber_var sub = participant->create_subscriber(
            SUBSCRIBER_QOS_DEFAULT,
            0,
            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        // 6. DataReader 생성
        DDS::DataReader_var dr = sub->create_datareader(
            topic,
            DATAREADER_QOS_DEFAULT,
            0,
            OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!dr)
        {
            std::cerr << "ERROR: create_datareader failed!" << std::endl;
            return 1;
        }

        // 7. Narrowing (타입 변환)
        MessageDataReader_var message_dr = MessageDataReader::_narrow(dr);
        if (!message_dr)
        {
            std::cerr << "ERROR: _narrow failed!" << std::endl;
            return 1;
        }

        // 8. 데이터 수신 루프
        std::cout << "Waiting for messages (Domain: 42, Topic: MyTopic)..." << std::endl;

        while (true)
        {
            Message msg;
            DDS::SampleInfo info;
            DDS::ReturnCode_t status = message_dr->take_next_sample(msg, info);

            if (status == DDS::RETCODE_OK)
            {
                if (info.valid_data)
                {
                    std::cout << "Received ID: " << msg.id
                              << " Contents: " << msg.contents << std::endl;
                }
            }
            else if (status != DDS::RETCODE_NO_DATA)
            {
                // 데이터가 없는 경우 외의 에러 발생 시 출력
                std::cerr << "ERROR: take_next_sample failed with status: " << status << std::endl;
            }

            ACE_OS::sleep(ACE_Time_Value(0, 500000)); // 0.5초 대기
        }

        // 9. 정리 (무한 루프라 실제론 도달하지 않음)
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