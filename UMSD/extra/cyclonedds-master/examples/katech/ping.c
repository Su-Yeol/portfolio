#include "dds/dds.h"
#include "dds/ddsrt/misc.h"
#include "katech.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>

#define TIME_STATS_SIZE_INCREMENT 50000
#define MAX_SAMPLES 100
#define US_IN_ONE_SEC 1000000LL

/* Forward declaration */

static dds_entity_t prepare_dds(dds_entity_t *writer, dds_entity_t *reader, dds_entity_t *readCond, dds_listener_t *listener);
static void finalize_dds(dds_entity_t participant);

static dds_entity_t waitSet;

#ifdef _WIN32
#include <Windows.h>
static bool CtrlHandler (DWORD fdwCtrlType)
{
  (void)fdwCtrlType;
  dds_waitset_set_trigger (waitSet, true);
  return true; //Don't let other handlers handle this key
}
#elif !DDSRT_WITH_FREERTOS && !__ZEPHYR__
static void CtrlHandler (int sig)
{
  (void)sig;
  dds_waitset_set_trigger (waitSet, true);
}
#endif

static dds_entity_t writer;
static dds_entity_t reader;
static dds_entity_t participant;
static dds_entity_t readCond;

static KatechSensorModule_SensorData pub_data;
static KatechSensorModule_SensorData sub_data[MAX_SAMPLES];
static void *samples[MAX_SAMPLES];
static dds_sample_info_t info[MAX_SAMPLES];

static dds_time_t startTime;
static dds_time_t elapsed = 0;

static bool warmUp = true;

static void data_available(dds_entity_t rd, void *arg)
{
  dds_time_t difference = 0;
  dds_time_t postTakeTime;
  static int trncnt = 1;
  int status;
  (void)arg;
  /* Take sample and check that it is valid */
  status = dds_take (rd, samples, info, MAX_SAMPLES, MAX_SAMPLES);
  if (status < 0)
    DDS_FATAL("dds_take: %s\n", dds_strretcode(-status));
  postTakeTime = dds_time ();

  if (!warmUp) {
    /* Print stats each second */
    difference = (postTakeTime - startTime)/DDS_NSECS_IN_USEC;
    if (difference > US_IN_ONE_SEC)
    {
      elapsed++;
    }
    printf("Translate cnt : %d , Data : \"",trncnt);
    for(int i = 0; i <  10; i++) {
      if((pub_data.payload._buffer[i] >= 32) && (pub_data.payload._buffer[0] <= 126))
        printf("%c", pub_data.payload._buffer[i]);
      else
        printf("%c", 177); // dark shade
    }
    KatechSensorModule_SensorData * valid_sample = &sub_data[0];
    printf("\" and received \"");
    for(int i = 0; i <  10; i++) {
      if((valid_sample->payload._buffer[i] >= 32) && (valid_sample->payload._buffer[0] <= 126))
        printf("%c", valid_sample->payload._buffer[i]);
      else
        printf("%c", 177); // dark shade
    }
    printf("\"\r");
    if(trncnt++ % 500 == 0) printf("\n");
  }

  postTakeTime = dds_time ();
  dds_time_t now = dds_time ()/DDS_NSECS_IN_USEC;
  pub_data.payload._buffer[0] = ((now%10000000000)/1000000000)+'0';
  pub_data.payload._buffer[1] = ((now%1000000000)/100000000)+'0';
  pub_data.payload._buffer[2] = ((now%100000000)/10000000)+'0';
  pub_data.payload._buffer[3] = ((now%10000000)/1000000)+'0';
  pub_data.payload._buffer[4] = ((now%1000000)/100000)+'0';
  pub_data.payload._buffer[5] = ((now%100000)/10000)+'0';
  pub_data.payload._buffer[6] = ((now%10000)/1000)+'0';
  pub_data.payload._buffer[7] = ((now%1000)/100)+'0';
  pub_data.payload._buffer[8] = ((now%100)/10)+'0';
  pub_data.payload._buffer[9] = ((now%10)/1)+'0';

  status = dds_write_ts (writer, &pub_data, postTakeTime);
  if (status < 0)
    DDS_FATAL("dds_write_ts: %s\n", dds_strretcode(-status));
}

static void usage(void)
{
  printf ("Usage (parameters must be supplied in order):\n"
          "./ping [timeOut (seconds, 0 = infinite)]\n"
          "Defaults:\n"
          "./ping\n");
  exit(EXIT_FAILURE);
}

int main (int argc, char *argv[])
{
  uint32_t payloadSize = 10;
  uint64_t numSamples = 0;
  bool invalidargs = false;
  dds_time_t timeOut = 0;
  dds_time_t time;
  dds_time_t difference = 0;

  dds_attach_t wsresults[1];
  size_t wsresultsize = 1U;
  dds_time_t waitTimeout = DDS_SECS (1);
  unsigned long i;
  int status;

  dds_listener_t *listener = NULL;
  bool use_listener = false;
  int argidx = 1;

  /* poor man's getopt works even on Windows */
  if (argc > argidx && strcmp(argv[argidx], "-l") == 0)
  {
    argidx++;
    use_listener = true;
  }

  /* Register handler for Ctrl-C */
#ifdef _WIN32
  DDSRT_WARNING_GNUC_OFF(cast-function-type)
  SetConsoleCtrlHandler ((PHANDLER_ROUTINE)CtrlHandler, TRUE);
  DDSRT_WARNING_GNUC_ON(cast-function-type)
#elif !DDSRT_WITH_FREERTOS && !__ZEPHYR__
  struct sigaction sat, oldAction;
  sat.sa_handler = CtrlHandler;
  sigemptyset (&sat.sa_mask);
  sat.sa_flags = 0;
  sigaction (SIGINT, &sat, &oldAction);
#endif

  memset (&sub_data, 0, sizeof (sub_data));
  memset (&pub_data, 0, sizeof (pub_data));

  for (i = 0; i < MAX_SAMPLES; i++)
  {
    samples[i] = &sub_data[i];
  }

  participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  if (use_listener)
  {
    listener = dds_create_listener(NULL);
    dds_lset_data_available(listener, data_available);
  }
  prepare_dds(&writer, &reader, &readCond, listener);

  if (argc - argidx == 1 && strcmp (argv[argidx], "quit") == 0)
  {
    printf ("Sending termination request.\n");
    fflush (stdout);
    /* pong uses a waitset which is triggered by instance disposal, and
      quits when it fires. */
    dds_sleepfor (DDS_SECS (1));
    pub_data.payload._length = 0;
    pub_data.payload._buffer = NULL;
    pub_data.payload._release = true;
    pub_data.payload._maximum = 0;
    status = dds_writedispose (writer, &pub_data);
    if (status < 0)
      DDS_FATAL("dds_writedispose: %s\n", dds_strretcode(-status));
    dds_sleepfor (DDS_SECS (1));
    goto done;
  }

  if (argc - argidx >= 1)
  {
    timeOut = atol (argv[argidx+2]);
  }
  if (invalidargs || (argc - argidx == 1 && (strcmp (argv[argidx], "-h") == 0 || strcmp (argv[argidx], "--help") == 0)))
    usage();
  printf ("# timeOut: %" PRIi64 "\n\n", timeOut);
  fflush (stdout);

  pub_data.payload._length = payloadSize;
  pub_data.payload._buffer = payloadSize ? dds_alloc (payloadSize) : NULL;
  pub_data.payload._release = true;
  pub_data.payload._maximum = 0;
  for (i = 0; i < payloadSize; i++)
  {
    pub_data.payload._buffer[i] = 'a';
  }

  startTime = dds_time ();
  printf ("# Waiting for startup jitter to stabilise\n");
  fflush (stdout);
  /* Write a sample that pong can send back */
  while (!dds_triggered (waitSet) && difference < DDS_SECS(5))
  {
    printf(" Wait : %d seconds\r", 5 - (difference / DDS_NSECS_IN_USEC / US_IN_ONE_SEC) );
    fflush (stdout);
    status = dds_waitset_wait (waitSet, wsresults, wsresultsize, waitTimeout);
    if (status < 0)
      DDS_FATAL("dds_waitset_wait: %s\n", dds_strretcode(-status));

    if (status > 0 && listener == NULL) /* data */
    {
      status = dds_take (reader, samples, info, MAX_SAMPLES, MAX_SAMPLES);
      if (status < 0)
        DDS_FATAL("dds_take: %s\n", dds_strretcode(-status));
    }

    time = dds_time ();
    difference = time - startTime;
  }
  if (!dds_triggered (waitSet))
  {
    warmUp = false;
    printf("# Warm up complete.\n\n");
    fflush (stdout);
  }


  startTime = dds_time ();
  dds_time_t now = dds_time ()/DDS_NSECS_IN_USEC;
  pub_data.payload._buffer[0] = ((now%10000000000)/1000000000)+'0';
  pub_data.payload._buffer[1] = ((now%1000000000)/100000000)+'0';
  pub_data.payload._buffer[2] = ((now%100000000)/10000000)+'0';
  pub_data.payload._buffer[3] = ((now%10000000)/1000000)+'0';
  pub_data.payload._buffer[4] = ((now%1000000)/100000)+'0';
  pub_data.payload._buffer[5] = ((now%100000)/10000)+'0';
  pub_data.payload._buffer[6] = ((now%10000)/1000)+'0';
  pub_data.payload._buffer[7] = ((now%1000)/100)+'0';
  pub_data.payload._buffer[8] = ((now%100)/10)+'0';
  pub_data.payload._buffer[9] = ((now%10)/1)+'0';
  /* Write a sample that pong can send back */
  status = dds_write_ts (writer, &pub_data, startTime);
  if (status < 0)
    DDS_FATAL("dds_write_ts: %s\n", dds_strretcode(-status));
  for (i = 0; !dds_triggered (waitSet) && (!numSamples || i < numSamples) && !(timeOut && elapsed >= timeOut); i++)
  {
    status = dds_waitset_wait (waitSet, wsresults, wsresultsize, waitTimeout);
    if (status < 0)
      DDS_FATAL("dds_waitset_wait: %s\n", dds_strretcode(-status));
    if (status != 0 && listener == NULL) {
      data_available(reader, NULL);
    }
  }

  if (!warmUp)
  {
    printf("# Transmit complete\n");
    fflush (stdout);
  }

done:

#ifdef _WIN32
  SetConsoleCtrlHandler (0, FALSE);
#elif !DDSRT_WITH_FREERTOS && !__ZEPHYR__
  sigaction (SIGINT, &oldAction, 0);
#endif

  finalize_dds(participant);

  for (i = 0; i < MAX_SAMPLES; i++)
  {
    KatechSensorModule_SensorData_free (&sub_data[i], DDS_FREE_CONTENTS);
  }
  KatechSensorModule_SensorData_free (&pub_data, DDS_FREE_CONTENTS);

  return EXIT_SUCCESS;
}

static dds_entity_t prepare_dds(dds_entity_t *wr, dds_entity_t *rd, dds_entity_t *rdcond, dds_listener_t *listener)
{
  dds_return_t status;
  dds_entity_t topic;
  dds_entity_t publisher;
  dds_entity_t subscriber;

  const char *pubPartitions[] = { "ping" };
  const char *subPartitions[] = { "pong" };
  dds_qos_t *pubQos;
  dds_qos_t *subQos;
  dds_qos_t *tQos;
  dds_qos_t *wQos;

  /* A DDS_Topic is created for our sample type on the domain participant. */
  tQos = dds_create_qos ();
  dds_qset_reliability (tQos, DDS_RELIABILITY_RELIABLE, DDS_SECS (10));
  topic = dds_create_topic (participant, &KatechSensorModule_SensorData_desc, "Katech", tQos, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));
  dds_delete_qos (tQos);

  /* A DDS_Publisher is created on the domain participant. */
  pubQos = dds_create_qos ();
  dds_qset_partition (pubQos, 1, pubPartitions);

  publisher = dds_create_publisher (participant, pubQos, NULL);
  if (publisher < 0)
    DDS_FATAL("dds_create_publisher: %s\n", dds_strretcode(-publisher));
  dds_delete_qos (pubQos);

  /* A DDS_DataWriter is created on the Publisher & Topic with a modified Qos. */
  wQos = dds_create_qos ();
  dds_qset_writer_data_lifecycle (wQos, false);
  *wr = dds_create_writer (publisher, topic, wQos, NULL);
  if (*wr < 0)
    DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-*wr));
  dds_delete_qos (wQos);

  /* A DDS_Subscriber is created on the domain participant. */
  subQos = dds_create_qos ();

  dds_qset_partition (subQos, 1, subPartitions);

  subscriber = dds_create_subscriber (participant, subQos, NULL);
  if (subscriber < 0)
    DDS_FATAL("dds_create_subscriber: %s\n", dds_strretcode(-subscriber));
  dds_delete_qos (subQos);
  /* A DDS_DataReader is created on the Subscriber & Topic with a modified QoS. */
  *rd = dds_create_reader (subscriber, topic, NULL, listener);
  if (*rd < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-*rd));

  waitSet = dds_create_waitset (participant);
  if (listener == NULL) {
    *rdcond = dds_create_readcondition (*rd, DDS_ANY_STATE);
    status = dds_waitset_attach (waitSet, *rdcond, *rd);
    if (status < 0)
      DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-status));
  } else {
    *rdcond = 0;
  }
  status = dds_waitset_attach (waitSet, waitSet, waitSet);
  if (status < 0)
    DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-status));

  return participant;
}

static void finalize_dds(dds_entity_t ppant)
{
  dds_return_t status;
  status = dds_delete (ppant);
  if (status < 0)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-status));
}
