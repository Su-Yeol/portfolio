#pragma once
#include <vector>
#include <mutex>
#include <cmath>
#include <cstring>

#define rad(x) x*3.14159 / 180.0
#define MaxDistance 100000 // 50km
using namespace std;

struct PathVertex
{
	double x;
	double y;
};

class PathVertexVct : public vector<PathVertex*>
{
public:
	PathVertexVct();
	virtual ~PathVertexVct();

	void DeleteAll();
	void Move(PathVertexVct* pPathVct);

private:
};

class PathManger
{
public:
	PathManger();
	virtual ~PathManger();

	double GetDistance(PathVertex* pt1, PathVertex* pt2);
	double GetDistance(double pt1x, double pt1y, double pt2x, double pt2y);
	double DistanceP2LS(double x1, double y1
						, double x2, double y2
						, double pt_x, double pt_y
						, double* px, double* py);

	void SetPath(PathVertexVct* pPathVct);
	void InterpolationPath();	
	void ModificationPath(PathVertexVct* pPathVct, int nStartIndex, int nEndIndex);
	bool GetPath(PathVertexVct* pMainPath, double longitude, double latitude);

private:
	PathVertexVct* m_pIndirectionPath; // ������� ���� ���
	PathVertexVct* m_oInterpolationPath; // ���� ���

	PathVertexVct* m_oMainPath;

	mutex m_oMutex;

	float m_nInterval;
};