# %% Path Zoom Plot
import matplotlib.pyplot as plt
import plotly.offline as plyo
import plotly.graph_objects as go

# import plotly.graph_objs as go

# WSL 경로에서 파일 읽어오기
# "/home/KATECH/JudgmentModule/data/Ibeo/Incheon_Ibeo_1020_23.10.20-18_52_06.txt"
with open("/home/KATECH/JudgmentModule/data/Ibeo/Incheon_Ibeo_1020_23.10.20-18_52_06.txt", 'r') as file:
    lines = file.readlines()#[10000:50000]

cnt = []
objcnt = []
localX = []
localY = []
latitudes = []
longitudes = []

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 6:
        cnt, objcnt, localX, localY, latitude, longitude = map(float, parts)
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
# %%
