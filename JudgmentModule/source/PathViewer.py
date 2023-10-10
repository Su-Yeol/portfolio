# %% Path Zoom Plot
import matplotlib.pyplot as plt
import plotly.offline as plyo
import plotly.graph_objects as go

# import plotly.graph_objs as go

# WSL 경로에서 파일 읽어오기
# "/home/KATECH/JudgmentModule/Path/KATECH/KATECH_SECRET_0908_23.07.03-18_27_26.txt"
with open("/home/KATECH/JudgmentModule/Path/KCITY/231004-215115_Gps.txt", 'r') as file:
    lines = file.readlines()

latitudes = []
longitudes = []

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 2:
        latitude, longitude = map(float, parts)
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
