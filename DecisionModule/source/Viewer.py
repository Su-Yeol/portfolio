import math
import socket
import threading
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pynput import keyboard

KSYIP = "192.168.100.11" # 11, 12
GPSPORT = 6612

GlobalLat = []
GlobalLon = []
GlobalLatLine1 = []
GlobalLonLine1 = []
GlobalLatLine2 = []
GlobalLonLine2 = []
GPSLat = []
GPSLon = []
GPSAzi = []
ObjLat = []
ObjLon = []
PathObjLat = []
PathObjLon = []
PathObjDist = []

ZoomLevel = 0.001
KeyFlag = 0

class GPSReceiver(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.receiver_socket.bind((KSYIP, GPSPORT))

        self.PathLat = []
        self.PathLon = []
        self.PathLatLine1 = []
        self.PathLonLine1 = []
        self.PathLatLine2 = []
        self.PathLonLine2 = []
        self.VehicleLat = []
        self.VehicleLon = []
        self.VehicleAzi = []
        self.ObjectLat = []
        self.ObjectLon = []
        self.PathObjectLat = []
        self.PathObjectLon = []
        self.PathObjectDist = []
        self.PathObjectIndex = []

    def run(self):
        self._receive()

    def _receive(self):
        print("Now Start Receiving...")
        while True:
            msg, _ = self.receiver_socket.recvfrom(8388068)
            try:
                RecvBuffer = bytearray(msg)
                # Path 위도 1도 = 111.19km > 0.00001 = 1.11119m / 경도 1도 = 88.80km > 0.00001 = 0.888m
                for i in range(128):
                    # byterorder = big(상위~하위), little(하위~상위)
                    pathLat = int.from_bytes(RecvBuffer[(8 * i):(8 * i) + 4], byteorder='little', signed=False)
                    pathLon = int.from_bytes(RecvBuffer[(8 * i) + 4:(8 * i) + 8], byteorder='little', signed=False)
                    pathLat = (float)(pathLat / 10000000.0)
                    pathLon = (float)(pathLon / 10000000.0)
                    self.PathLat.append(pathLat)
                    self.PathLon.append(pathLon)
                    # Line
                    pathLatLine1 = pathLat - 0.0000169884
                    pathLonLine1 = pathLon - 0.0000202702
                    pathLatLine2 = pathLat + 0.0000169884
                    pathLonLine2 = pathLon + 0.0000202702
                    self.PathLatLine1.append(pathLatLine1)
                    self.PathLonLine1.append(pathLonLine1)
                    self.PathLatLine2.append(pathLatLine2)
                    self.PathLonLine2.append(pathLonLine2)

                # Vehicle
                vehicleLat = int.from_bytes(RecvBuffer[1024:1028], byteorder='little', signed=False)
                vehicleLon = int.from_bytes(RecvBuffer[1028:1032], byteorder='little', signed=False)
                vehicleAzi = int.from_bytes(RecvBuffer[1032:1036], byteorder='little', signed=False)
                vehicleLat = (float)(vehicleLat / 10000000.0)
                vehicleLon = (float)(vehicleLon / 10000000.0)
                vehicleAzi = (float)((vehicleAzi / 100.0) * (-1) + 90)
                self.VehicleLat.append(vehicleLat)
                self.VehicleLon.append(vehicleLon)
                self.VehicleAzi.append(vehicleAzi)
                # 11.29 : 1036~1039(추가) 1040 / 1041(1036) / 1042(1037)
                pathObjectDist = int.from_bytes(RecvBuffer[1036:1040], byteorder='little', signed=False)
                pathObjectDist = (float)(pathObjectDist / 100)
                self.PathObjectDist.append(pathObjectDist)
                pathObjectIndex = int.from_bytes(RecvBuffer[1040:1042], byteorder='little', signed=False)
                self.PathObjectIndex.append(pathObjectIndex)
                # Object
                objectCnt = (int)(RecvBuffer[1042])
                for j in range(objectCnt):
                    objectLat = int.from_bytes(RecvBuffer[1043 + (8 * j) : 1043 + (8 * j) + 4], byteorder='little', signed=False)
                    objectLon = int.from_bytes(RecvBuffer[1043 + (8 * j) + 4 : 1043 + (8 * j) + 8], byteorder='little', signed=False)
                    
                    if (objectLat > 0 and objectLon > 0):
                        objectLat = (float)(objectLat / 10000000.0)
                        objectLon = (float)(objectLon / 10000000.0)
                        if(objectLat > 36 and objectLat < 38 and objectLon > 126 and objectLon < 129):
                            # 11.29
                            if( j == pathObjectIndex):
                                self.PathObjectLat.append(objectLat)
                                self.PathObjectLon.append(objectLon)
                            else:
                                self.ObjectLat.append(objectLat)
                                self.ObjectLon.append(objectLon)
                                self.PathObjectLat.append(500)
                                self.PathObjectLon.append(500)

                if len(RecvBuffer) == 1283:
                    self.PathLat = []
                    self.PathLon = []
                    self.PathLatLine1 = []
                    self.PathLonLine1 = []
                    self.PathLatLine2 = []
                    self.PathLonLine2 = []
                    self.VehicleLat = []
                    self.VehicleLon = []
                    self.VehicleAzi = []
                    self.ObjectLat = []
                    self.ObjectLon = []
                    self.PathObjectLat = []
                    self.PathObjectLon = []
                    self.PathObjectDist = []
                    self.PathObjectIndex = []

            except Exception as e:
                pass
            if KeyFlag == 1:
                break
        print("Close Socket...")
        self.receiver_socket.close()
 
    def getCurrentGPS(self):
        return (
            self.PathLat, self.PathLon, self.PathLatLine1, self.PathLonLine1, self.PathLatLine2, self.PathLonLine2, 
            self.VehicleLat, self.VehicleLon, self.VehicleAzi, self.ObjectLat, self.ObjectLon, 
            self.PathObjectLat, self.PathObjectLon, self.PathObjectDist, 
        )

def zoom_out():
    global ZoomLevel
    if ZoomLevel < 0.01:
        ZoomLevel = ZoomLevel + 0.0001

def zoom_in():
    global ZoomLevel
    if ZoomLevel >= 0.001:
        ZoomLevel = ZoomLevel - 0.0001

def on_press(key):
    global KeyFlag
    try:
        if key.char == "+":
            zoom_in()
        if key.char == "-":
            zoom_out()
        if key.char == "q":
            KeyFlag = 1
    except AttributeError:
        print("", end="")

def keythread():
    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()

MyGPSReceiver = GPSReceiver()
MyGPSReceiver.start()
threading.Thread(target=keythread).start()

fig, ax = plt.subplots(figsize = (13, 13))
ax.set_title("KATECH")
while True:
    # GlobalLat, GlobalLon, GPSLat, GPSLon, GPSAzi, ObjLat, ObjLon, PathObjLat, PathObjLon, PathObjDist = MyGPSReceiver.getCurrentGPS()
    GlobalLat, GlobalLon, GlobalLatLine1, GlobalLonLine1, GlobalLatLine2, GlobalLonLine2, GPSLat, GPSLon, GPSAzi, ObjLat, ObjLon, PathObjLat, PathObjLon, PathObjDist = MyGPSReceiver.getCurrentGPS()
    try:
        idx = 0
        if(len(GPSLon) > 0 and len(GPSLat) > 0 and len(GPSAzi) > 0 and len(ObjLat) > 0 and len(ObjLon) > 0 
           and len(PathObjLon) > 0 and len(PathObjLat) > 0 and len(PathObjDist) > 0 and len(ObjLat) == len(ObjLon)):
            ax.cla()
            plt.grid(True)
            plt.axis("equal")
            ax.set_xlim([GPSLon[idx] - (ZoomLevel * 1.25), GPSLon[idx] + (ZoomLevel * 1.25)])
            ax.set_ylim([GPSLat[idx] - (ZoomLevel * 1.25), GPSLat[idx] + (ZoomLevel * 1.25)])
            # PATH
            ax.plot(GlobalLon, GlobalLat, color="black", linestyle='--', label='Global_Path') # Global coordinate
            # Line 크기 비교
            if(GlobalLonLine1 > GlobalLonLine2 and GlobalLatLine1 > GlobalLatLine2):
                ax.plot(GlobalLonLine1, GlobalLatLine1, color="orange", linestyle='-')
                ax.plot(GlobalLonLine2, GlobalLatLine2, color="orange", linestyle='-')
            elif(GlobalLonLine1 > GlobalLonLine2 and GlobalLatLine1 < GlobalLatLine2):
                ax.plot(GlobalLonLine1, GlobalLatLine2, color="orange", linestyle='-')
                ax.plot(GlobalLonLine2, GlobalLatLine1, color="orange", linestyle='-')
            elif(GlobalLonLine1 < GlobalLonLine2 and GlobalLatLine1 > GlobalLatLine2):
                ax.plot(GlobalLonLine2, GlobalLatLine1, color="orange", linestyle='-')
                ax.plot(GlobalLonLine1, GlobalLatLine2, color="orange", linestyle='-')
            elif(GlobalLonLine1 < GlobalLonLine2 and GlobalLatLine1 < GlobalLatLine2):
                ax.plot(GlobalLonLine2, GlobalLatLine2, color="orange", linestyle='-')
                ax.plot(GlobalLonLine1, GlobalLatLine1, color="orange", linestyle='-')

            # ax.plot(GlobalLonLine1, GlobalLatLine1, color="orange", linestyle='-') # Line
            # ax.plot(GlobalLonLine2, GlobalLatLine2, color="orange", linestyle='-') # Line
            # Vehicle
            ax.scatter(GPSLon[idx], GPSLat[idx], color='red', s=50, marker='D', label='Vehicle')
            # Arrow
            ax.arrow(GPSLon[idx], GPSLat[idx], 0.0002 * math.cos(math.radians(GPSAzi[idx])), 0.00012 * math.sin(math.radians(GPSAzi[idx])), 
                     color="g", head_width=0.00003, head_length=0.00003, width=0.000002)
            # LiDAR Angle
            # ax.arrow(GPSLon[idx], GPSLat[idx], 0.0002 * math.cos(math.radians(GPSAzi[idx]-50)), 0.00012 * math.sin(math.radians(GPSAzi[idx]-50)), 
            #          color="yellow", head_width=0.00000001, head_length=0.00000001, width=0.00000001)
            # ax.arrow(GPSLon[idx], GPSLat[idx], 0.0002 * math.cos(math.radians(GPSAzi[idx]+60)), 0.00012 * math.sin(math.radians(GPSAzi[idx]+60)), 
            #          color="yellow", head_width=0.00000001, head_length=0.00000001, width=0.00000001)
            # Object
            if (PathObjDist[idx] != 100):
                ax.scatter(PathObjLon, PathObjLat, color='purple', marker='o', label='Path Object')
            ax.scatter(ObjLon, ObjLat, color='blue', s=20, marker='x', label='Object')
            
            # Path Object Distance 11.29
            ax.text(GPSLon[idx], GPSLat[idx]-0.00055, f'PathObjDist: {PathObjDist[idx]}', ha='center', va='center', bbox=dict(facecolor='white', alpha=0.5))

            ax.legend()
            plt.pause(0.05)
            ax.figure.canvas.draw()
            plt.tight_layout()

    except Exception as e:
        print(e)

