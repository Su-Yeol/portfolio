import socket
import threading
from enum import Enum
import numpy as np
import math
import struct
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation
from struct import *
from parse import *
from pynput import keyboard
import os
import time

_ref_path = "./ref/221017-PG1.txt"

VIEWER_IP = '192.168.100.255'
#VIEWER_IP = '127.0.0.1'
GPSPORT = 4092

_log_dir = "./log"
try:
    os.makedirs(_log_dir)
except OSError:
    if not os.path.isdir(_log_dir):
        raise
time_str = time.strftime("%y%m%d-%H%M%S")
print(time_str)

packet_data_len = 25600
global_kst = float(0.0)
global_azi = float(0.0)
global_lat = float(0.0)
global_lon = float(0.0)
global_stat = 'A'

path_log_len = 500

path_50m_origin_x = np.array([], float)
path_50m_origin_y = np.array([], float)


zoom_level = 3
terminator = 0

class GPS_Receiver(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.receiver_socket.bind((VIEWER_IP, GPSPORT))
        
        self.current_kst = 0.
        self.current_azimuth = 0.
        self.current_latitude = 0
        self.current_longitude = 0
        self.current_stat = 'R'

    def run(self):
        self._receive()

    def _receive(self):
        #f = open(_log_dir + "/" + time_str + ".txt", "w")
        print("Now Start Receiving...")
        while True:
            msg, clientAddress = self.receiver_socket.recvfrom(8388068)
            try:
                msg = msg.decode('utf-8')
                if(msg[0] == '4' and msg[1] == '0' and msg[2] == '9' and msg[3] == '2'):
                    _msg = msg.split('/')
                    utc = (_msg[1])
                    azi = (_msg[4])
                    lat = (_msg[2])
                    lon = (_msg[3])
                    stat = (_msg[5])
                    
                    self.current_kst = float(utc)
                    self.current_azimuth = (float(azi) + 270) * -1
                    self.current_latitude = float(lat[0:2]) + float(lat[2:]) / 60.0
                    self.current_longitude = float(lon[0:3]) + float(lon[3:]) / 60.0
                    self.current_stat = (stat)

                    '''self.current_kst      = round(self.current_utc , 2)
                    self.current_azimuth  = round(self.current_azimuth, 2)
                    self.current_latitude = round(self.current_latitude , 7)
                    self.current_longitude= round(self.current_longitude, 7)
                    '''
                    
                    print(self.current_kst, self.current_azimuth, self.current_latitude, self.current_longitude, self.current_stat)
                    
            except Exception as e:
                pass
            if terminator == 1:
                break
        print("Close Socket...")
        self.receiver_socket.close()

    def getCurrentGPS(self):
        return (
            self.current_azimuth, self.current_azimuth, self.current_latitude, self.current_longitude, self.current_stat,
        )


def zoom_out():
    global zoom_level
    if zoom_level < 100:
        zoom_level = zoom_level + 1


def zoom_in():
    global zoom_level
    if zoom_level > 1:
        zoom_level = zoom_level - 1


def on_press(key):
    global terminator
    try:
        if key.char == "+":
            zoom_in()
        if key.char == "-":
            zoom_out()

        if key.char == "q":
            terminator = 1
    except AttributeError:
        print("", end="")


def keythread():
    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()

'''
ref = open(_ref_path, "r")
ref_latitude = np.array([], float)
ref_longitude = np.array([], float)
ref_waypoint_cnt = 0
ref_distance = np.array([], float)


while True:
    line = ref.readline()
    if not line:
        break
    _,_,_Latitude,_Longitude, _ = parse("{}/{}/{}/{}/{}", line)
    ref_latitude = np.append(ref_latitude, float(_Latitude) * 111000.)
    ref_longitude = np.append(ref_longitude, float(_Longitude) * 88800.)
ref_waypoint_cnt = ref_latitude.size
print("Read waypoint : ", ref_waypoint_cnt)


if ref_waypoint_cnt >= 1:
    for i in range(ref_waypoint_cnt - 1):
        x1 = ref_longitude[i]
        x2 = ref_longitude[i + 1]
        y1 = ref_latitude[i]
        y2 = ref_latitude[i + 1]
        ref_distance = np.append(
            ref_distance, math.sqrt(math.pow(x2 - x1, 2) + math.pow(y2 - y1, 2))
        )
ref_distance = np.append(ref_distance, float(0.0))

'''
trajectory_lat = np.array([], float)
trajectory_lon = np.array([], float)

# fig, ax = plt.subplots()
fig, ax = plt.subplots(2, 1, gridspec_kw={"height_ratios": [1.5, 3]})
# for debug

plt.grid(True)

plt.axis("equal")

MyGPSReceiver = GPS_Receiver()
MyGPSReceiver.start()
threading.Thread(target=keythread).start()

'''
<Viewer Data>
- Global PATH
- Velocity
- Traffic Light
- 
'''


global_kst, global_azi, global_lat, global_lon, global_stat = MyGPSReceiver.getCurrentGPS()
global_lat_utm = global_lat * 111000.
global_lon_utm = global_lon * 88800.

def animate(i):
    global trajectory_lat, trajectory_lon
    global global_kst, global_azi, global_lat, global_lon, global_stat
    global global_lat_utm, global_lon_utm

    try:
        # ax.cla()
        ax[0].cla()
        ax[1].cla()

        global_kst, global_azi, global_lat, global_lon, global_stat = MyGPSReceiver.getCurrentGPS()
        global_lat_utm = global_lat * 111000.
        global_lon_utm = global_lon * 88800.

        # find zero_cross point
        #ax[0].plot(temp_x, temp_y, marker="o")
        ax[0].arrow(0, 0, 2, 0, color="r", head_width=0.5, head_length=1)
        ax[0].set_xlim(-5, 30)
        ax[0].set_ylim(-10, 10)

        # 차량 이동 궤적 생성 - 붉은선
        if global_lat_utm != 0:
            trajectory_lat = np.append(trajectory_lat, global_lat_utm)
            trajectory_lon = np.append(trajectory_lon, global_lon_utm)
        if trajectory_lat.size > path_log_len:
            trajectory_lat = np.delete(trajectory_lat, 0)
            trajectory_lon = np.delete(trajectory_lon, 0)

        #ax[1].plot(ref_longitude, ref_latitude)  # 전체 경로
        ax[1].plot(trajectory_lon, trajectory_lat, color="r")  # 지나온 궤적
        ax[1].arrow(
            global_lon_utm,
            global_lat_utm,
            7 * math.cos(math.radians(global_azi)),
            7 * math.sin(math.radians(global_azi)),
            color="r",
            head_width=2,
            head_length=4,
        )  # 차량 헤딩 방향 화살표
        ax[1].plot(
            [global_lon_utm],
            [global_lat_utm],
            color="k",
            marker="o",
        )

        # 그래프 Zoom 설정
        ax[1].set_xlim(
            [global_lon_utm - (zoom_level * 20),
            global_lon_utm + (zoom_level * 20)]
        )
        ax[1].set_ylim(
            [global_lat_utm - (zoom_level * 20),
            global_lat_utm + (zoom_level * 20)]
        )

        # estimation path
        '''temp_x = np.resize(temp_x, 50)
        temp_y = np.resize(temp_y, 50)
        for L in range(50):
            temp_x[L] = L
            temp_y[L] = (0 - math.tan(Out_C1)) * L + (0 - Out_C0)
        rotate_theta = Vehicle_Azimuth
        for index in range(50):
            x = temp_x[index]
            y = temp_y[index]
            temp_x[index] = (
                x * math.cos(math.radians(rotate_theta))
                + y * -1 * math.sin(math.radians(rotate_theta))
                + Vehicle_Longi_m
            )
            temp_y[index] = (
                x * math.sin(math.radians(rotate_theta))
                + y * math.cos(math.radians(rotate_theta))
                + Vehicle_Lati_m
            )'''
    except Exception as e:
        print(e)

ani = FuncAnimation(plt.gcf(), animate, interval=50)
plt.tight_layout()
plt.show()

