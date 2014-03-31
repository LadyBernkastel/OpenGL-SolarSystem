#ifndef RLS_PLANET_HEADER
#define RLS_PLANET_HEADER

const static int cs_iMAX_TRAIL_POS = 500;

struct trailPosition {
	float m_fPosition[4];
	trailPosition *m_pNext;
	trailPosition *m_pLast;
};

struct planet {	
	planet *m_pNext;
	planet *m_pLast;
	float m_fSize;
	float m_fMass;
	float m_fVelocity[4];
	float m_fAcceleration[4];
	float m_fForce[4];
	float m_fPosition[4];
	float m_fColour[4];
	trailPosition *m_pTrailHead;
	trailPosition *m_pTrailTail;
	int m_iNoTrailPos;
	float m_vfTrailVertex[cs_iMAX_TRAIL_POS * 3];
	float m_vfColourVertex[cs_iMAX_TRAIL_POS * 3];
};

void printList();
planet* createNode();
int pushHead(planet *pNode);
int pushTail(planet *pNode);
planet* popHead();
planet* popTail();
planet* popMiddle(planet *pNode);
bool destroy(planet *pNode);
int pushMiddle(planet *pNode, planet *pInsertAfter);
trailPosition* createTrailPosition(float* fPosition);
int pushTrailHead(planet *pPlanet, trailPosition *pPos);
trailPosition* popTrailTail(planet *pPlanet);
bool destroyTrailPos(planet *pPlanet, trailPosition *pNode);
void generateColourVertex(planet *pPlanet);
void generatePositionVertex(planet *pPlanet);

#endif