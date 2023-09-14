#%%
import matplotlib.pyplot as plt

# WSL 경로에서 파일 읽어오기
with open('./Path/KATECH/KATECH_0914.txt', 'r') as file:
    lines = file.readlines()

latitudes = []
longitudes = []

for line in lines:
    parts = line.strip().split('/')
    if len(parts) == 2:
        latitude, longitude = map(float, parts)
        latitudes.append(latitude)
        longitudes.append(longitude)

# 경로 그리기
plt.plot(longitudes, latitudes, marker='o', linestyle='-')
plt.xlabel('Longitude')
plt.ylabel('Latitude')
plt.title('Path')
plt.grid(True)

# 그래프 표시
plt.show()
# %%
