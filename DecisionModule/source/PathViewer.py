

# %% Path Zoom Plot
import matplotlib.pyplot as plt
import plotly.offline as plyo
import plotly.graph_objects as go

# import plotly.graph_objs as go

# WSL 경로에서 파일 읽어오기
# "/home/KATECH/JudgmentModule/data/Ibeo/Incheon_Ibeo_1020_23.10.20-18_52_06.txt"
with open("/home/KATECH/JudgmentModule/Path/Incheon/GPS_23.10.24-12_20_34.txt", 'r') as file:
    lines = file.readlines()[590:601]

cnt = []
objcnt = []
localX = []
localY = []
latitudes = []
longitudes = []

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 3:
        cnt, latitude, longitude = map(float, parts)
        #cnt.append(count)
        latitudes.append(latitude)
        longitudes.append(longitude)
        # latitudes.append(37.3973630)
        # longitudes.append(126.6344241)

# Scattermapbox plot을 생성합니다.
fig = go.Figure(go.Scattermapbox(
    mode="lines+markers",
    lon=longitudes,
    lat=latitudes,
    marker={'size': 10},
    line=dict(width=3, color='blue')
))

# 지도 스타일과 중심점을 설정합니다.
fig.update_layout(
    mapbox_style="carto-positron",
    mapbox_center={"lat": sum(latitudes)/len(latitudes), "lon": sum(longitudes)/len(longitudes)},
    mapbox_zoom=15
)
# 그래프 표시
fig.show()
# %% Path Zoom Plot
import folium
import time

# import plotly.graph_objs as go

# WSL 경로에서 파일 읽어오기
# "/home/KATECH/JudgmentModule/data/Ibeo/Incheon_Ibeo_1020_23.10.20-18_52_06.txt"
with open("/home/KATECH/JudgmentModule/Path/Incheon/GPS_23.10.24-12_20_34.txt", 'r') as file:
    lines = file.readlines()[6000:8000]

locations = []
a = []
b = []
cnt = []
objcnt = []
objId = []
localX = []
localY = []
carLat = []
carLong = []
latitudes = []
longitudes = []

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 11:
        a, cnt, objcnt, objId, b, localX, localY, carLat, carLong, latitude, longitude = (parts[0]), float(parts[1]), float(parts[2]), float(parts[3]), float(parts[4]), float(parts[5]), float(parts[6]), float(parts[7]), float(parts[8]), float(parts[9]),  float(parts[10])
        # locations.append([latitude, longitude])
        locations.append([latitude, longitude])

if len(locations) > 0:
    m = folium.Map(location=[locations[0][0], locations[0][1]], zoom_start=100)  # 초기 중심과 줌 레벨 설정
else:
    print("Error: 'locations' list is empty. Please provide valid coordinates.")
# 물체의 위치에 마커 추가
for location in locations:
    folium.Marker(location, popup=f"위도: {location[0]}, 경도: {location[1]}").add_to(m)
    # folium.Marker([location[0], location[1]], popup=f"위도: {location[0]}, 경도: {location[1]}",
    #               icon=folium.DivIcon(html=f"<div>{location[2]}</div>")).add_to(m)

m.save("/home/KATECH/JudgmentModule/data/object_map_time_ic.html")
# %% 하버사인, 방위각
import math

MainCntChk = 0
MainCnts = []
IbeoCntChk = 0
IbeoChks = []
IbeoCnts = []
PathLats = []
PathLongs = []
IbeoLats = []
IbeoLongs = []

MinDist1 = 500
MinDist2 = 500
MaxDist = 0

with open("/home/KATECH/JudgmentModule/Log/Path/KCITY/KCITY_Path_1106_23.10.22-14_04_12.txt", 'r') as file:
    lines = file.readlines()

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 3:
        lat = float(parts[1])
        lon = float(parts[2])
        PathLats.append(lat)
        PathLongs.append(lon)

with open("/home/KATECH/JudgmentModule/Log/Ibeo/KCITY/KCITY_Ibeo_1106_23.10.22-14_04_12.txt", 'r') as file2:
    lines2 = file2.readlines()

for line2 in lines2:
    parts2 = line2.strip().split('/')
    if len(parts2) == 11:
        MainCnt = int(parts2[1])
        IbeoChk = int(parts2[2])
        IbeoChks.append(IbeoChk)

        if MainCnt > MainCntChk:
            MainCntChk = MainCnt
            IbeoMaxCnt = max(IbeoChks)
            IbeoCnts.append((IbeoMaxCnt+1))
            IbeoChks = []

        ibeolat = float(parts2[-2])
        ibeolon = float(parts2[-1])
        IbeoLats.append(ibeolat)
        IbeoLongs.append(ibeolon)

def haversine(lat1, lon1, lat2, lon2):
    # Radius of the Earth in kilometers
    earth_radius = 6378135#6371000  6378135

    # Convert latitude and longitude from degrees to radians
    lat1 = math.radians(lat1)
    lon1 = math.radians(lon1)
    lat2 = math.radians(lat2)
    lon2 = math.radians(lon2)

    # Haversine formula
    dlat = lat2 - lat1
    dlon = lon2 - lon1
    a = math.sin(dlat/2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon/2)**2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
    distance = earth_radius * c

    return distance

def calculate_bearing(lat1, lon1, lat2, lon2):
    deg2rad = math.pi / 180.0
    rad2deg = 180.0 / math.pi

    lat1 = lat1 * deg2rad
    lon1 = lon1 * deg2rad
    lat2 = lat2 * deg2rad
    lon2 = lon2 * deg2rad

    dLon = lon2 - lon1

    y = math.sin(dLon) * math.cos(lat2)
    x = math.cos(lat1) * math.sin(lat2) - math.sin(lat1) * math.cos(lat2) * math.cos(dLon)

    bearing = math.atan2(y, x)
    bearing2 = (bearing * rad2deg + 360) % 360  # Ensure the result is in the range [0, 360) degrees

    
    print(f"Bearing: {bearing} degrees||Bearing2: {bearing2} degrees")

    return bearing2

def calculate_relative_bearing(car_bearing, target_bearing):
    # Calculate the relative bearing where left is positive and right is negative
    relative_bearing = target_bearing

    if relative_bearing > 180.0:
        relative_bearing -= 360.0
    elif relative_bearing < -180.0:
        relative_bearing += 360.0

    return relative_bearing

# D = (math.pi*6378137)/180.0 # WGS84
# C = math.cos(37*math.pi/180.0)*D
# lat1 = 37.2388051  # Latitude of Point 1
# lon1 = 126.7734112  # Longitude of Point 1

# lat2 = 37.2388036 + y/D
# lon2 = 126.7734110 + x/C

# lat3 = 37.2388036 + y/D
# lon3 = 126.7734110  + x/(C * math.cos(37.2388036))

# lat4 = 37.2388042
# lon4 = 126.7734132* math.cos(37.2388036*(math.pi/180.0))
# dist2 = haversine(lat1, lon1, lat2, lon2)
# print(f"Distance1: {dist} m")
# print(f"Distance1: {dist2} m")


for i in range(MainCntChk):
    # for k in range(i*30, (i+1)*30):
    for k in range(i*IbeoCnts[i], (i+1)*IbeoCnts[i]):
        for j in range(i*128, (i+1)*128):
            distance = haversine(PathLats[j], PathLongs[j], IbeoLats[k], IbeoLongs[k])
            dist = math.acos((math.sin(PathLats[j] * (math.pi/180.0)) * math.sin(IbeoLats[k] * (math.pi/180.0))) + 
                             (math.cos(PathLats[j] * (math.pi/180.0)) * math.cos(IbeoLats[k] * (math.pi/180.0)) * math.cos((IbeoLongs[k]-PathLongs[j])*(math.pi/180.0)))) * 6378137
            
            if distance < MinDist1:
                MinDist1 = distance

            if dist < MinDist2:
                MinDist2 = dist

    print(f"Haversine Distance    : {MinDist1} m")
    print(f"Spherical Cosines Dist: {MinDist2} m")
    
    if MinDist1 != MinDist2:
        if ((MinDist2-MinDist1)) > MaxDist:
            MaxDist = (MinDist2-MinDist1)

print(f"거리 차이: {MaxDist:.7f}m\n")
# car_bearing = 90.0  # Assume the car's heading is 90 degrees
# target_bearing = calculate_bearing(lat1, lon1, lat2, lon2)
# print(f"target Bearing: {target_bearing} degrees")

# relative_bearing = calculate_relative_bearing(car_bearing, 90)
# print(f"Relative Bearing: {relative_bearing} degrees")
# if(relative_bearing < 0):
#     print(f"Distance: {-distance} m")
# else:
#     print(f"Distance: {distance} m")


# %%
import matplotlib.pyplot as plt

# 위도와 경도를 저장할 리스트
latitudes = []
longitudes = []

# txt 파일 경로
file_path = "/home/KATECH/JudgmentModule/Log/Path/Incheon/231022-102202_Gps.txt"

# 파일 읽기
try:
    with open(file_path, 'r') as file:
        for line in file:
            tokens = line.strip().split('/')
            if tokens != 0:
                if len(tokens) == 11:  # 필요에 따라 토큰 수를 조정하세요
                    latitude = float(tokens[-2])
                    longitude = float(tokens[-1])
                    latitudes.append(latitude)
                    longitudes.append(longitude)
except FileNotFoundError:
    print(f"파일 '{file_path}'을 찾을 수 없습니다.")
    exit(1)

a = min(longitudes)
b= max(longitudes)

c=min(latitudes)
d=max(latitudes)
print(f"{a}, {b}, {c}, {d}")
# 시간 순서대로 좌표를 그립니다.
plt.plot(longitudes, latitudes, marker='o', linestyle='-', markersize=2)

# 그래프 제목과 라벨 설정 (필요에 따라 수정하세요)
plt.title("경로 시간 순서대로 표시")
plt.xlabel("경도")
plt.ylabel("위도")

# 그래프 표시
plt.show()

# %%
import math

# 핸들 각도를 라디안에서 도로 변환하는 함수
def degrees_to_radians(degrees):
    return degrees * math.pi / 180.0

# 두 지점 사이의 방향을 계산하는 함수
def calculate_heading(lat1, lon1, lat2, lon2, handle_angle):
    # 핸들 각도를 라디안으로 변환
    handle_angle_rad = degrees_to_radians(handle_angle)

    # 위도 및 경도를 라디안으로 변환
    lat1_rad = degrees_to_radians(lat1)
    lon1_rad = degrees_to_radians(lon1)
    lat2_rad = degrees_to_radians(lat2)
    lon2_rad = degrees_to_radians(lon2)

    # 방향을 계산
    y = math.sin(lon2_rad - lon1_rad) * math.cos(lat2_rad)
    x = math.cos(lat1_rad) * math.sin(lat2_rad) - math.sin(lat1_rad) * math.cos(lat2_rad) * math.cos(lon2_rad - lon1_rad)
    direction_rad = math.atan2(y, x)
    print(direction_rad - handle_angle_rad)
    # 결과를 라디안에서 도로 변환하고 핸들 각도를 보정
    heading = (direction_rad * 180.0 / math.pi) - handle_angle

    # 결과가 -180도에서 180도 범위에 있도록 보정
    if heading > 180.0:
        heading -= 360.0
    elif heading < -180.0:
        heading += 360.0

    return heading

# 핸들 각도 및 GPS 정보 설정
handle_angle = 0.0  # 예시 핸들 각도 (도)
latitude1 = 37.3939982  # 시작 위도 3939982
longitude1 = 126.6344839  # 시작 경도 6344839
latitude2 = 37.3939908  # 끝 위도 3939908
longitude2 = 126.6344885  # 끝 경도 6344885

# 방향(Heading) 계산
heading = calculate_heading(latitude1, longitude1, latitude2, longitude2, handle_angle)

# 결과 출력
print("차량의 Heading:", heading, "도")
# %%
import math
# 1/37.2388051/126.7734112
# 1/37.2388042/126.7734132
Alat = 37.2388051
Alon = 126.7734112
Blat = 37.2388042
Blon = 126.7734132
EarthRadius = 6371000
D = 110979.309#(math.pi*EarthRadius)/180.0
C = 88907.949#math.cos(((Alat+Blat)/2)*math.pi/180.0) * D
print(f"C: {C} || D: {D}")

# 분, 도, 초
a = Alat // 1
amin = ((Alat - a) * 60) // 1
asec = (((Alat - a) * 60) % 1) * 60
print(f"{a, amin, asec}")
b = Blat // 1
bmin = ((Blat - b) * 60) // 1
bsec = (((Blat - b) * 60) % 1) * 60
print(f"{b, bmin, bsec}")

latdgr = a - b
latmin = amin - bmin
latsec = asec - bsec
print(f"{latdgr, latmin, latsec}")

alon = Alon // 1
alonmin = ((Alon - alon) * 60) // 1
alonsec = (((Alon - alon) * 60) % 1) * 60
print(f"{alon, alonmin, alonsec}")
blon = Blon // 1
blonmin = ((Blon - blon) * 60) // 1
blonsec = (((Blon - blon) * 60) % 1) * 60
print(f"{blon, blonmin, blonsec}")

londgr = alon - blon
lonmin = alonmin - blonmin
lonsec = alonsec - blonsec
print(f"{londgr, lonmin, lonsec}")

dist = math.sqrt(math.pow((latdgr*D)+(latmin*(D/60))+(latsec*((D/60)/60)) , 2) + math.pow((londgr*C)+(lonmin*(C/60))+(lonsec*((C/60)/60)) , 2))

print(f"{latdgr*D, D, latmin*(D/60), (D/60), latsec*((D/60)/60), (D/60)/60}")
print(f"{(londgr*C), C, (lonmin*(C/60)), (C/60), lonsec*((C/60)/60), (C/60)/60}")
print(f"dist : {dist}")
# %%
