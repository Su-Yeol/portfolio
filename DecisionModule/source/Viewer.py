import math
import socket
import threading
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pynput import keyboard

KSYIP = "192.168.100.12" # 11, 12
# KSYIP = "127.0.0.1"
GPSPORT = 6612

GlobalLat = []
GlobalLon = []
GPSLat = []
GPSLon = []
GPSAzi = []
ObjLat = []
ObjLon = []
RecvRange = 0

ZoomLevel = 0.001
KeyFlag = 0 # key control

class GPSReceiver(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.receiver_socket.bind((KSYIP, GPSPORT))

        self.PathLat = []
        self.PathLon = []
        self.VehicleLat = []
        self.VehicleLon = []
        self.VehicleAzi = []
        self.ObjectLat = []
        self.ObjectLon = []

    def run(self):
        self._receive()

    def _receive(self):
        print("Now Start Receiving...")
        while True:
            msg, _ = self.receiver_socket.recvfrom(8388068)
            try:
                RecvBuffer = bytearray(msg)
                # Path
                for i in range(128):
                    # byterorder = big(상위~하위), little(하위~상위)
                    pathLat = int.from_bytes(RecvBuffer[(8 * i):(8 * i) + 4], byteorder='little', signed=False)
                    pathLon = int.from_bytes(RecvBuffer[(8 * i) + 4:(8 * i) + 8], byteorder='little', signed=False)
                    pathLat = (float)(pathLat / 10000000.0)
                    pathLon = (float)(pathLon / 10000000.0)
                    self.PathLat.append(pathLat)
                    self.PathLon.append(pathLon)
                # Vehicle
                vehicleLat = int.from_bytes(RecvBuffer[1024:1028], byteorder='little', signed=False)
                vehicleLon = int.from_bytes(RecvBuffer[1028:1032], byteorder='little', signed=False)
                vehicleAzi = int.from_bytes(RecvBuffer[1032:1036], byteorder='little', signed=False)
                vehicleLat = (float)(vehicleLat / 10000000.0)
                vehicleLon = (float)(vehicleLon / 10000000.0)
                vehicleAzi = (float)((vehicleAzi / 100.0) * (-1) + 90) # toRadian
                self.VehicleLat.append(vehicleLat)
                self.VehicleLon.append(vehicleLon)
                self.VehicleAzi.append(vehicleAzi)
                # Object
                objectCnt = (int)(RecvBuffer[1036])
                # self.ObjectCnt.append(objectCnt)
                for j in range(objectCnt):
                    objectLat = int.from_bytes(RecvBuffer[1037 + (8 * j):1037 + (8 * j) + 4], byteorder='little', signed=False)
                    objectLon = int.from_bytes(RecvBuffer[1037 + (8 * j) + 4:1037 + (8 * j) + 8], byteorder='little', signed=False)
                    
                    if (objectLat > 0 and objectLon > 0):
                        objectLat = (float)(objectLat / 10000000.0)
                        objectLon = (float)(objectLon / 10000000.0)
                        if(objectLat < 38 and objectLon < 129):
                            self.ObjectLat.append(objectLat)
                            self.ObjectLon.append(objectLon)
                
                # test
                if len(RecvBuffer) == 1276:
                    self.PathLat = []
                    self.PathLon = []
                    self.VehicleLat = []
                    self.VehicleLon = []
                    self.VehicleAzi = []
                    self.ObjectLat = []
                    self.ObjectLon = []

            except Exception as e:
                pass
            if KeyFlag == 1:
                break
        print("Close Socket...")
        self.receiver_socket.close()

    def getCurrentGPS(self):
        return (
            self.PathLat, self.PathLon, self.VehicleLat, self.VehicleLon, self.VehicleAzi, self.ObjectLat, self.ObjectLon,
        )


def zoom_out():
    global ZoomLevel
    if ZoomLevel < 0.01:
        ZoomLevel = ZoomLevel + 0.001

def zoom_in():
    global ZoomLevel
    if ZoomLevel >= 0.001:
        ZoomLevel = ZoomLevel - 0.001

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
    GlobalLat, GlobalLon, GPSLat, GPSLon, GPSAzi, ObjLat, ObjLon = MyGPSReceiver.getCurrentGPS()
    try:
        idx = 0
        if(len(GPSLon) > 0 and len(GPSAzi) > 0 and len(ObjLat) == len(ObjLon)):
            ax.set_xlim([GPSLon[idx] - (ZoomLevel * 3), GPSLon[idx] + (ZoomLevel * 3)])
            ax.set_ylim([GPSLat[idx] - (ZoomLevel * 3), GPSLat[idx] + (ZoomLevel * 3)])
            ax.cla()
            plt.grid(True)
            plt.axis("equal")       
            # PATH
            ax.plot(GlobalLon, GlobalLat, color="black", linestyle='--', label="Global_Path ")
            # Vehicle
            ax.scatter(GPSLon[idx], GPSLat[idx], color='red', s=50, marker='D', label='Vehicle')
            # Arrow
            ax.arrow(GPSLon[idx], GPSLat[idx], 0.0002 * math.cos(math.radians(GPSAzi[idx])), 0.0002 * math.sin(math.radians(GPSAzi[idx])), 
                     color="g", head_width=0.00003, head_length=0.00003, width=0.000001)
            # Object
            ax.scatter(ObjLon, ObjLat, color='blue', marker='x', label='Object')
            ax.legend()

            plt.pause(0.05)
            ax.figure.canvas.draw()
            plt.tight_layout()

    except Exception as e:
        print(e)

