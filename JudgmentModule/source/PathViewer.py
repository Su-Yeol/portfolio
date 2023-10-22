# %% Path Zoom Plot
import matplotlib.pyplot as plt
import plotly.offline as plyo
import plotly.graph_objects as go

# import plotly.graph_objs as go

# WSL 경로에서 파일 읽어오기
# "/home/KATECH/JudgmentModule/data/Ibeo/Incheon_Ibeo_1020_23.10.20-18_52_06.txt"
with open("/home/KATECH/JudgmentModule/Path/Incheon/231022-102202_Gps.txt", 'r') as file:
    lines = file.readlines()#[10000:50000]

cnt = []
objcnt = []
localX = []
localY = []
latitudes = []
longitudes = []

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 2:
        latitude, longitude = map(float, parts)
        #cnt.append(count)
        latitudes.append(latitude)
        longitudes.append(longitude)

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
with open("/home/KATECH/JudgmentModule/data/Ibeo/Incheon_Ibeo_1021_23.10.21-22_04_49.txt", 'r') as file:
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
# %% Haversine Formula Check
import math

def haversine(lat1, lon1, lat2, lon2):
    # Radius of the Earth in kilometers
    earth_radius = 6371

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
    distance = earth_radius * c * 1000

    return distance

# Example usage 37.3963496/126.6348176/37.3963396/126.6348925
lat1 = 37.3963496  # Latitude of Point 1
lon1 = 126.6348176  # Longitude of Point 1
lat2 = 500  # Latitude of Point 2
lon2 = 500   # Longitude of Point 2

distance = haversine(lat1, lon1, lat2, lon2)
print(f"Distance: {distance} m")

# %%
import matplotlib.pyplot as plt

# 위도와 경도를 저장할 리스트
latitudes = []
longitudes = []

# txt 파일 경로
file_path = "/home/KATECH/JudgmentModule/Path/Incheon/231022-102202_Gps.txt"

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
