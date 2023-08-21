#include "PathManager.h"
#include <iostream>
#define M_PI       3.14159265358979323846

PathVertexVct::PathVertexVct(void)
{
}

PathVertexVct::~PathVertexVct(void)
{
}

void PathVertexVct::DeleteAll()
{
	for (iterator itr = begin(); itr != end(); itr++)
	{
		delete *itr;
	}
	clear();
}

void PathVertexVct::Move(PathVertexVct* pPathVct)
{
	for (PathVertexVct::iterator itr = pPathVct->begin(); itr != pPathVct->end(); itr++)
	{
		push_back(*itr);
	}
}


PathManger::PathManger(void)
{
	m_pIndirectionPath = new PathVertexVct();
	m_oInterpolationPath = new PathVertexVct();
	m_oMainPath = new PathVertexVct();

	m_nInterval = 0.05; // 221206 MODIFIED BY JDS 1-> 0.05
	//m_nInterval = 1; // 221206 MODIFIED BY JDS 1-> 0.05
}

PathManger::~PathManger(void)
{
}

double PathManger::GetDistance(PathVertex* pt1, PathVertex* pt2)
{
	double distance = 0;

	double startLatitudeRadian = pt1->y * M_PI / 180;   // M_PI = 3.141592.. // latitude
	double startLongitudeRadian = pt1->x * M_PI / 180; // longitude
	double endLatitudeRadian = pt2->y * M_PI / 180; // latitude
	double endLongitudeRadian = pt2->x * M_PI / 180; // longitude

	distance = acos(sin(startLatitudeRadian) * sin(endLatitudeRadian) + cos(startLatitudeRadian) * cos(endLatitudeRadian) * cos(endLongitudeRadian - startLongitudeRadian));

	distance = distance * 6371.;

	return distance * 1000.;
}

double PathManger::GetDistance(double pt1x, double pt1y, double pt2x, double pt2y)
{
	double distance = 0;

	double startLatitudeRadian = pt1y * M_PI / 180;   // M_PI = 3.141592.. // latitude
	double startLongitudeRadian = pt1x * M_PI / 180; // longitude
	double endLatitudeRadian = pt2y * M_PI / 180; // latitude
	double endLongitudeRadian = pt2x * M_PI / 180; // longitude

	distance = acos(sin(startLatitudeRadian) * sin(endLatitudeRadian) + cos(startLatitudeRadian) * cos(endLatitudeRadian) * cos(endLongitudeRadian - startLongitudeRadian));

	distance = distance * 6371;

	return distance * 1000;
}

double PathManger::DistanceP2LS(double x1, double y1
								, double x2, double y2
								, double pt_x, double pt_y
								, double* px, double* py)
{
	double	dDist1, dDist2;
	double	dx, dy;
	double	dXYInPrd, dBaseInPrd, dOriginInPrd;
	double	t;
	double	dDist;
	double	xp, yp;

	dx = x2 - x1;
	dy = y2 - y1;

	dXYInPrd = dx * dx + dy * dy;
	dBaseInPrd = dx * pt_x + dy * pt_y;
	dOriginInPrd = dx * x1 + dy * y1;

	if (dXYInPrd == 0)
	{ // ���� ( ( x1,y1 )- ( x2,y2 ) )�� ���̰� 0�ΰ�� ( ������ ��ġ�ϴ� ��� )
		if (px != NULL && py != NULL)
		{
			*px = x1;	*py = y1;
		}
		dDist = GetDistance(x1, y1, pt_x, pt_y);
		return dDist;
	}

	t = (dBaseInPrd - dOriginInPrd) / dXYInPrd;

	if (0.0 < t && t < 1.0)
	{
		if (dx == 0.0)
		{ // ���� ( ( x1,y1 )- ( x2,y2 ) )�� ������
			xp = x1;
			yp = pt_y;
		}
		else if (dy == 0.0)
		{
			xp = pt_x;
			yp = y1;
		}
		else
		{
			xp = x1 + t * dx;
			yp = y1 + t * dy;
		}
		if (px != NULL && py != NULL)
		{
			*px = xp;	*py = yp;
		}
		dDist = GetDistance(pt_x, pt_y, xp, yp);
		return dDist;
	}
	else
	{
		dDist1 = GetDistance(pt_x, pt_y, x1, y1);
		dDist2 = GetDistance(pt_x, pt_y, x2, y2);

		if (dDist1 <= dDist2)
		{
			if (px != NULL && py != NULL)
			{
				*px = x1;	*py = y1;
			}
			dDist = dDist1;
		}
		else
		{
			if (px != NULL && py != NULL)
			{
				*px = x2;	*py = y2;
			}
			dDist = dDist2;
		}
		return dDist;
	}
}

int s_cnt = 0;
void PathManger::SetPath(PathVertexVct* pPathVct)
{	
	m_pIndirectionPath->DeleteAll();
	m_pIndirectionPath->Move(pPathVct);	
	// printf("eeeeeeeeeeeee %d",pPathVct->size());

	// for(int i=0; i<pPathVct->size() -1; i++)
	// {
	// 	int nInterval = GetDistance(pPathVct->at(i), pPathVct->at(i + 1));
	// 	if(nInterval > 10){
	// 		std::cout << "nInterval : " << nInterval << std::endl;
	// 		printf("index : %d/%d \n", i, i+1);
	// 		printf("dVertex ----  %.7lf / %.7lf\n", pPathVct->at(i)->x, pPathVct->at(i)->y);
	// 		printf("dNextVertex ----  %.7lf / %.7lf\n", pPathVct->at(i+1)->x, pPathVct->at(i+1)->y);

	// 	}
	// }

	//m_pIndirectionPath

	// char p_buf[100];
	// sprintf(p_buf, "./data/Path/pPathVct->%d.txt", s_cnt);
	// FILE *f_MainPath;
	// f_MainPath = fopen(p_buf, "w");
	// if (f_MainPath == NULL)
	// {
	// 	printf("Error opening file!\n");
	// 	exit(1);
	// }

	// for (int i=0; i<pPathVct->size()-1; i++)
	// {
		
	// 	fprintf(f_MainPath, "%.7f/%.7f\n", pPathVct->at(i)->x, pPathVct->at(i)->y);
	// }
	

	m_oMutex.lock();
	m_oInterpolationPath->DeleteAll();	
	InterpolationPath();

	// sprintf(p_buf, "./data/Path/SetPath2_%d.txt", s_cnt);
	// FILE *f_MainPath1;
	// f_MainPath1 = fopen(p_buf, "w");
	// if (f_MainPath1 == NULL)
	// {
	// 	printf("Error opening file!\n");
	// 	exit(1);
	// }

	// for (int i=0; i<m_oInterpolationPath->size(); i++)
	// {
	// 	fprintf(f_MainPath1, "%.7f/%.7f\n", m_oInterpolationPath->at(i)->x, m_oInterpolationPath->at(i)->y);
	// }
	//222 m_oInterpolationPath
	m_oMutex.unlock();

	s_cnt++;
}

void PathManger::InterpolationPath()
{
	for (int i = 0; i < m_pIndirectionPath->size()-1; i++)
	{
		double dVertexX = m_pIndirectionPath->at(i)->x;
		double dVertexY = m_pIndirectionPath->at(i)->y;

		double dNextVertexX = m_pIndirectionPath->at(i+1)->x;
		double dNextVertexY = m_pIndirectionPath->at(i+1)->y;


		// [int -> double] MODIFIED BY JDS 230106 
		int nInterval = GetDistance(m_pIndirectionPath->at(i), m_pIndirectionPath->at(i + 1));
		
		// if(nInterval > 10){
		// 	std::cout << "eeee nInterval : " << nInterval << std::endl;
		// 	printf("e index : %d/%d \n", i, i+1);
		// 	printf("e dVertex ----  %.7lf / %.7lf\n", dVertexX, dVertexY);
		// 	printf("e dNextVertex ----  %.7lf / %.7lf\n", dNextVertexX, dNextVertexY);

		// }
			
		if (nInterval > m_nInterval)
		{
			// [int -> double]
			int nNum = nInterval / m_nInterval;

			double dGadDistanceX = (dNextVertexX - dVertexX) / (double)nNum;
			double dGadDistanceY = (dNextVertexY - dVertexY) / (double)nNum;
			
			for (int j = 0; j < nNum; j++)
			{
				PathVertex* pPathVertex = new PathVertex();
				pPathVertex->x = dVertexX + (dGadDistanceX * (double)j);
				pPathVertex->y = dVertexY + (dGadDistanceY * (double)j);

				m_oInterpolationPath->push_back(pPathVertex);
			}
		}
		else
		{
			PathVertex* pPathVertex = new PathVertex();
			pPathVertex->x = m_pIndirectionPath->at(i)->x;
			pPathVertex->y = m_pIndirectionPath->at(i)->y;
			m_oInterpolationPath->push_back(pPathVertex);
		}
	}

	PathVertex* pPathVertex = new PathVertex();
	pPathVertex->x = m_pIndirectionPath->at(m_pIndirectionPath->size() - 1)->x;
	pPathVertex->y = m_pIndirectionPath->at(m_pIndirectionPath->size() - 1)->y;

	//m_oInterpolationPath->push_back(pPathVertex);
}

void PathManger::ModificationPath(PathVertexVct* pPathVct, int nStartIndex, int nEndIndex)
{
	if (m_pIndirectionPath->size() == 0)
	{
		return;
	}

	PathVertexVct* pTempFrontVct = new PathVertexVct();
	pTempFrontVct->resize(nStartIndex);
	copy(m_pIndirectionPath->begin(), m_pIndirectionPath->begin() + nStartIndex, pTempFrontVct->begin());

	PathVertexVct* pTempDeleteMiddleVct = new PathVertexVct();
	pTempDeleteMiddleVct->resize(nStartIndex + nEndIndex + 1);
	copy(m_pIndirectionPath->begin() + nStartIndex, m_pIndirectionPath->begin() + nEndIndex + 1, pTempDeleteMiddleVct->begin());

	pTempDeleteMiddleVct->DeleteAll();

	PathVertexVct* pTempMiddleVct = new PathVertexVct();
	pTempMiddleVct->Move(pPathVct);

	PathVertexVct* pTempBackVct = new PathVertexVct();
	pTempBackVct->resize(m_pIndirectionPath->size() - nEndIndex - 1);
	copy( (m_pIndirectionPath->begin() + nEndIndex + 1), m_pIndirectionPath->end(), pTempBackVct->begin());
	
	m_pIndirectionPath->clear();
	PathVertexVct().swap(*m_pIndirectionPath);

	m_pIndirectionPath->reserve(pTempFrontVct->size() + pTempMiddleVct->size() + pTempBackVct->size());
	
	m_pIndirectionPath->insert(m_pIndirectionPath->begin(), pTempBackVct->begin(), pTempBackVct->end());
	m_pIndirectionPath->insert(m_pIndirectionPath->begin(), pTempMiddleVct->begin(), pTempMiddleVct->end());
	m_pIndirectionPath->insert(m_pIndirectionPath->begin(), pTempFrontVct->begin(), pTempFrontVct->end());
	
	delete pTempFrontVct;
	delete pTempMiddleVct;
	delete pTempBackVct;
	delete pTempDeleteMiddleVct;
	
	m_oMutex.lock();
	m_oInterpolationPath->DeleteAll();
	InterpolationPath();
	m_oMutex.unlock();
}

bool PathManger::GetPath(PathVertexVct* pMainPath, double longitude, double latitude)
{
	
	m_oMutex.lock();	
	
	
	if (m_oInterpolationPath->size() == 0)
	{
		m_oMutex.unlock();
		return false;
	}

	PathVertex* pPathStartVertex = new PathVertex();
	double dLeastDistance = 10000.0;
	int nLeastDistanceIndex = 0;

	for (int i = 0; i < m_oInterpolationPath->size() - 1; i++)
	{
		double MatchX = 0.0;
		double MatchY = 0.0;
		double dDistance = DistanceP2LS(m_oInterpolationPath->at(i)->x, m_oInterpolationPath->at(i)->y, m_oInterpolationPath->at(i+1)->x, m_oInterpolationPath->at(i+1)->y, longitude, latitude, &MatchX, &MatchY);

		if (dDistance < dLeastDistance)
		{
			// pPathStartVertex->x = longitude;
			// pPathStartVertex->y = latitude;
			// dLeastDistance = dDistance;
			// nLeastDistanceIndex = i;
			pPathStartVertex->x = MatchX;
			pPathStartVertex->y = MatchY;
			dLeastDistance = dDistance;
			nLeastDistanceIndex = i;
		}
	}
	
	//pMainPath->push_back(pPathStartVertex);

	double dTotalDistance = 0.0;

	for (int i = nLeastDistanceIndex+1; i < m_oInterpolationPath->size() - 1; i++)
	{
		dTotalDistance = dTotalDistance + GetDistance(m_oInterpolationPath->at(i), m_oInterpolationPath->at(i+1));

		if (dTotalDistance > MaxDistance)
		{
			m_oMutex.unlock();
			return true;
		}

		PathVertex* pPathVertex = new PathVertex();
		memcpy(pPathVertex, m_oInterpolationPath->at(i), sizeof(PathVertex));
		pMainPath->push_back(pPathVertex);
	}

	PathVertex* pPathEndVertex = new PathVertex();
	memcpy(pPathEndVertex, m_oInterpolationPath->at(m_oInterpolationPath->size() - 1), sizeof(PathVertex));
	//pMainPath->push_back(pPathEndVertex);
	
	m_oMutex.unlock();
	return true;
}