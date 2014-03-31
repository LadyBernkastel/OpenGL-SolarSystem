#ifndef RLS_SOLAR_HEADER
#define RLS_SOLAR_HEADER

struct solarSystem {
	int m_iMaxPlanets;
	int m_iCurrentPlanets;
	float m_fDragCoefficient;
	float m_fGravitationalConstant;
	float m_fMaxVelocity;
	float m_fMaxNegativeVelocity;
	float m_fMaxPosition;
	float m_fMaxNegativePosition;
	float m_fTimeStep;
	planet* m_pSun;
};

planet* generateRandomPlanet();
void generateRandomPlanets(int number);
planet* generateSun();
solarSystem* createDefaultSolarSystem();
void maintainSolarSystem(solarSystem *pSystem);
void calculateBodyForcesForEach(planet *pBody, planet *pHead, solarSystem *pSystem);
void calculateBodyForces(planet *pBody, planet *pOtherBody, float *fTotalForce, solarSystem *pSystem);
void calculateAcceleration(planet *pElement);
void calculateVelocityAndPosition(planet *pElement, solarSystem *pSystem);
int detectCollision(planet *pBody, planet *pOtherBody);
int calculateWithinPositionBounds(planet *pBody, solarSystem *pSystem);
void calculateCollisionsAndWithinBounds(planet *pBody, planet *pHead, solarSystem *pSystem);

#endif