#include <logger.h>
#include <queue>
#include <mutex>


#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>



#include <qcarcam_client.h>

#include <qcarcam_client.h>

#include "qcarcam.h"
#include "logger.h"
#include "katech_zlib.h"

#include <pthread.h>
#include <fstream>

namespace katech
{
namespace sensor
{
namespace camera
{

QCarCam::img_queue QCarCam::camera_queue[CameraCount];
unsigned int QCarCam::cam_count[CameraCount] = {0,};
unsigned int QCarCam::cam_fps[CameraCount];
QCarCam::qcarcam_camera_callback QCarCam::callback[CameraCount] = {NULL,};
QCarCam::qcarcam_Event_Callback QCarCam::onCallbackDraw[CameraCount] = {NULL,};;
QCarCam::qcarcam_Event_Callback QCarCam::onCallbackFrameChange[CameraCount] = {NULL,};;

void QCarCam::inqueue(int id, std::shared_ptr<xImgBuffer> buffer)
{
	std::lock_guard<std::mutex> guard(camera_queue[id].m);
	camera_queue[id].queue.push(buffer);
	camera_queue[id].cv.notify_all();
}

std::shared_ptr<QCarCam::xImgBuffer> QCarCam::dequeue(int id)
{
	std::lock_guard<std::mutex> guard(camera_queue[id].m);
	std::shared_ptr<xImgBuffer> temp;
	temp = camera_queue[id].queue.front();
	camera_queue[id].queue.pop();
	return temp;
}

std::shared_ptr<QCarCam::xImgBuffer> QCarCam::flash_queue(int id)
{
	std::lock_guard<std::mutex> guard(camera_queue[id].m);
	std::shared_ptr<xImgBuffer> temp;
	temp = camera_queue[id].queue.back();
	camera_queue[id].queue = std::queue<std::shared_ptr<xImgBuffer>>();
	return temp;
}


void QCarCam::qcarcam_client_event_callback(int input_id, unsigned char * buf_ptr, size_t buf_len)
{
	UNUSED(buf_len);
	//INFO("id = %d , buflen = %ld", input_id, buf_len);
#if 0
	if(input_id == 8)
	{
		input_id = 6;
	}
#endif
	if(callback[input_id] != NULL)
	{
		std::shared_ptr<xImgBuffer> imgbuffer = std::make_shared<xImgBuffer>();
		imgbuffer->width = IMG_BUF_WIDTH;
		imgbuffer->height = IMG_BUF_HEIGHT;
		//imgbuffer->buffer.assign(buf_ptr,buf_ptr+buf_len);
		#if 0
		if(onCallbackFrameChange[input_id] != NULL)
		{
			onCallbackFrameChange[input_id](input_id, buf_ptr);
		}
		#endif
		imgbuffer->buffer.assign(buf_ptr, buf_ptr + (IMG_BUF_WIDTH * IMG_BUF_HEIGHT * IMG_BUF_PIXEL_SIZE));
		//INFO("push %d", input_id);
		inqueue(input_id, imgbuffer);
		if(onCallbackDraw[input_id] != NULL)
		{
			onCallbackDraw[input_id](input_id, buf_ptr);
		}
		//INFO("queue[%d] = %d", input_id, camera_queue[input_id].queue.size());
	}
}



void QCarCam::thread_camProcess_waitfor(int id)
{
	std::shared_ptr<xImgBuffer> temp;
	int rx_cnt;
	bool wait_init = true;
	auto instance = getInstance();
	std::cv_status result;
	{
		int policy, s;
		struct sched_param param;
		s = pthread_getschedparam(pthread_self(), &policy, &param);
		if(s != 0)
		{
			ERROR("pthread_getschedparam");
		}
		INFO("    policy=%s, priority=%d\n",
		     (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
		     (policy == SCHED_RR)    ? "SCHED_RR" :
		     (policy == SCHED_OTHER) ? "SCHED_OTHER" :
		     "???", param.sched_priority);
		const int max = sched_get_priority_max(SCHED_RR);
		param.sched_priority = max;
		s = pthread_setschedparam(pthread_self(), policy, &param);
		if(s != 0)
		{
			ERROR("pthread_getschedparam");
		}
		s = pthread_getschedparam(pthread_self(), &policy, &param);
		if(s != 0)
		{
			ERROR("pthread_getschedparam");
		}
		INFO("    policy=%s, priority=%d\n",
		     (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
		     (policy == SCHED_RR)    ? "SCHED_RR" :
		     (policy == SCHED_OTHER) ? "SCHED_OTHER" :
		     "???", param.sched_priority);
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(id, &mask);
		pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
	}
	while(instance->continueExecution && wait_init)
	{
		std::unique_lock<std::mutex> lk(camera_queue[id].m_cv);
		if(camera_queue[id].cv.wait_for(lk, std::chrono::milliseconds(120)) != std::cv_status::timeout)
		{
			wait_init = false;
		}
	}
	while(instance->continueExecution)
	{
		{
			std::unique_lock<std::mutex> lk(camera_queue[id].m_cv);
			result = camera_queue[id].cv.wait_for(lk, std::chrono::milliseconds(120));
		}
		if(result == std::cv_status::timeout)
		{
			ERROR("Camera[%d] time out", id);
		}
		else
		{
			rx_cnt = camera_queue[id].queue.size();
			if(rx_cnt != 0)
			{
				temp = flash_queue(id);
				cam_count[id] += rx_cnt;
				if(callback[id] != NULL)
				{
					callback[id](id, temp);
				}
				if(rx_cnt > 1)
				{
					ERROR("Camera image drop[%d] = %d", id, rx_cnt - 1);
				}
			}
			else
			{
				ERROR("!-----------------defence!! (%d)-------------", id);
			}
		}
	}
}

void QCarCam::Thred_info()
{
	auto instance = getInstance();
	while(instance->continueExecution)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		for(int i = 0; i < CameraCount; i++)
		{
			cam_fps[i] = cam_count[i];
			cam_count[i] = 0;
		}
	}
}

std::shared_ptr<QCarCam> QCarCam::getInstance()
{
	static std::shared_ptr<QCarCam> instance = std::make_shared<QCarCam>();
	return instance;
}
void QCarCam::setCallback(int id, qcarcam_camera_callback cb)
{
	callback[id] = cb;
}

void QCarCam::setCallback_FrameChange(int id, qcarcam_Event_Callback cb)
{
	onCallbackFrameChange[id] = cb;
}

void QCarCam::setCallback_Draw(int id, qcarcam_Event_Callback cb)
{
	onCallbackDraw[id] = cb;
}

void QCarCam::start()
{
	continueExecution = true;
	const char * szInputConfPath = "./etc/camera_settings.xml";
	//const char* szInputConfPath = "/data/misc/camera/8cam_display0.xml";
	INFO("config = %s", szInputConfPath);
	qcarcam_client_start_preview(szInputConfPath, qcarcam_client_event_callback);
	mCamInfo = std::make_shared<std::thread>(Thred_info);
	for(int i = 0; i < CameraCount; i++)
	{
		if(callback[i] != NULL)
		{
			INFO("Cam ID %d recv thread init", i);
			mCamReceiver[i] = std::make_shared<std::thread>(thread_camProcess_waitfor, i);
		}
	}
}
void QCarCam::stop()
{
	continueExecution = false;
	mCamInfo->join();
	for(int i = 0; i < CameraCount; i++)
	{
		if(callback[i] != NULL)
		{
			mCamReceiver[i]->join();
		}
	}
	qcarcam_client_stop_preview();
}

int QCarCam::getFPS(int id)
{
	return cam_fps[id];
}

qcarcam_status QCarCam::getStatus(int id)
{
	return qcarcam_client_getCamStatus(id);
}

void QCarCam::saveRaw(char * path, std::shared_ptr<QCarCam::xImgBuffer> buf)
{
	static int count = 0;
	std::fstream fs;
	char filename[64] = "";
	sprintf(filename, "/home/root/img/%014lld.bin",path, katech::Log::getUptime());
	fs.open(filename, std::ios::out | std::ios::binary);
	if(fs.is_open())
	{
		fs.write(reinterpret_cast<const char *>(buf->buffer.data()), buf->buffer.size());
		fs.close();
	}
	count++;
}

void QCarCam::savePng(char * path,cv::Mat & buf)
{
	static int count = 0;
	std::fstream fs;
	char filename[64] = "";
	sprintf(filename, "/home/root/img/%s/%014lld.png", path,katech::Log::getUptime());
	cv::imwrite(filename, buf);
	count++;
}


}
}
}
