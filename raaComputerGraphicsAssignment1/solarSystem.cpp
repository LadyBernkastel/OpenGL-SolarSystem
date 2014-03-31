#include <stdio.h>
#include "planet.h"
#include "solarSystem.h"
#include "raaMaths/raaMaths.h"
#include "raaMaths/raaVector.h"

void generateRandomPlanets(int number) {
	for (int i = 0; i < number; i++) {
		generateRandomPlanet();
	}
}

planet* generateRandomPlanet() {
	planet *plNew = createNode();
	plNew->m_fMass = randFloat(200.0f, 1000.0f);
	plNew->m_fSize = plNew->m_fMass / 20;
	
	plNew->m_fAcceleration[0] = 0.0f;
	plNew->m_fAcceleration[1] = 0.0f;
	plNew->m_fAcceleration[2] = 0.0f;
	plNew->m_fAcceleration[3] = 0.0f;
			
	plNew->m_fForce[0] = 0.0f;
	plNew->m_fForce[1] = 0.0f;
	plNew->m_fForce[2] = 0.0f;
	plNew->m_fForce[3] = 0.0f;

	plNew->m_fPosition[0] = randFloat(-400.0f, 400.0f); // x
	plNew->m_fPosition[1] = randFloat(-200.0f, 200.0f); // y
	plNew->m_fPosition[2] = randFloat(-400.0f, 400.0f); // z
	plNew->m_fPosition[3] = 0.0f;	
			
	float fUpVector[3] = {0.0f, 1.0f, 0.0f};
	float fDirectionVector[3] = {0.0f};
	fDirectionVector[0] = -plNew->m_fPosition[0];
	fDirectionVector[1] = -plNew->m_fPosition[1];
	fDirectionVector[2] = -plNew->m_fPosition[2];

	vecCrossProduct(fUpVector, fDirectionVector, fDirectionVector);
	vecNormalise(fDirectionVector, fDirectionVector);
	vecScalarProduct(fDirectionVector, randFloat(-40.0f, 40.0f), plNew->m_fVelocity);
	plNew->m_fVelocity[4] = 0.0f;
	
	plNew->m_fColour[0] = randFloat(0.0f, 1.0f); // r
	plNew->m_fColour[1] = randFloat(0.0f, 1.0f); // g
	plNew->m_fColour[2] = randFloat(0.0f, 1.0f); // b
	plNew->m_fColour[3] = 0.0f;
		
	pushHead(plNew);
	return plNew;
}

planet* generateSun() {
	planet *plNew = createNode();
	plNew->m_fMass = 90000.0f;
	plNew->m_fSize = plNew->m_fMass / 500;	

	plNew->m_fAcceleration[0] = 0.0f;
	plNew->m_fAcceleration[1] = 0.0f;
	plNew->m_fAcceleration[2] = 0.0f;
	plNew->m_fAcceleration[3] = 0.0f;
		
	plNew->m_fForce[0] = 0.0f;
	plNew->m_fForce[1] = 0.0f;
	plNew->m_fForce[2] = 0.0f;
	plNew->m_fForce[3] = 0.0f;

	plNew->m_fVelocity[0] = 0.0f;
	plNew->m_fVelocity[1] = 0.0f;
	plNew->m_fVelocity[2] = 0.0f;
	plNew->m_fVelocity[3] = 0.0f;

	plNew->m_fPosition[0] = 0.0f;
	plNew->m_fPosition[1] = 0.0f;
	plNew->m_fPosition[2] = 0.0f;
	plNew->m_fPosition[3] = 0.0f;

	plNew->m_fColour[0] = 1.0f; // r
	plNew->m_fColour[1] = 0.5f; // g
	plNew->m_fColour[2] = 0.0f; // b
	plNew->m_fColour[3] = 0.0f;

	return plNew;
}

solarSystem* createDefaultSolarSystem() {
	solarSystem* pSystem = new solarSystem;
	pSystem->m_iMaxPlanets = 25;
	pSystem->m_iCurrentPlanets = 0;

	pSystem->m_fDragCoefficient = 0.999999f;
	pSystem->m_fGravitationalConstant = 0.999f;

	pSystem->m_fMaxVelocity = 150.0f;
	pSystem->m_fMaxNegativeVelocity = -150.0f;

	pSystem->m_fMaxPosition = 2000.0f;
	pSystem->m_fMaxNegativePosition = -2000.0f;

	pSystem->m_fTimeStep = 0.016666f;

	pSystem->m_pSun = generateSun();
	return pSystem;
}

void maintainSolarSystem(solarSystem *pSystem) {
	while(pSystem->m_iCurrentPlanets < pSystem->m_iMaxPlanets) {
		generateRandomPlanet();
		pSystem->m_iCurrentPlanets++;
	}
}

void calculateBodyForcesForEach(planet *pBody, planet *pHead, solarSystem *pSystem) {
	float fTotalForce[4] = {0.0f};
	// Calculate the sun's force
	calculateBodyForces(pBody, pSystem->m_pSun, fTotalForce, pSystem);

	// calculate other forces
	for (planet *pOtherBody = pHead; pOtherBody;) {		
		if (pBody != pOtherBody) {
			calculateBodyForces(pBody, pOtherBody, fTotalForce, pSystem);
		}
		pOtherBody = pOtherBody->m_pNext;
	}	

	vecCopy(fTotalForce, pBody->m_fForce);
}

void calculateBodyForces(planet *pBody, planet *pOtherBody, float *fTotalForce, solarSystem *pSystem) {
	pBody->m_fForce[0] = 0.0f;
	pBody->m_fForce[1] = 0.0f;
	pBody->m_fForce[2] = 0.0f;
	
	float fCalculatedForce[3] = {0.0f};

	float fCalculatedMass = pBody->m_fMass * pOtherBody->m_fMass;
	
	float fDistanceBetweenPos[3] = {0.0f};
	vecSub(pOtherBody->m_fPosition, pBody->m_fPosition, fDistanceBetweenPos);
	
	float fDistanceSquared = (fDistanceBetweenPos[0] * fDistanceBetweenPos[0]) + (fDistanceBetweenPos[1] * fDistanceBetweenPos[1]) + (fDistanceBetweenPos[2] * fDistanceBetweenPos[2]);
	
	// Calculate the unit vector bisecting the two positions
	float fUnitVector[3] = {0.0f};
	vecSub(pOtherBody->m_fPosition, pBody->m_fPosition, fUnitVector);
	vecNormalise(fUnitVector, fUnitVector);
	
	float fGravitationalForce = pSystem->m_fGravitationalConstant * (fCalculatedMass / fDistanceSquared);

	vecScalarProduct(fUnitVector, fGravitationalForce, fCalculatedForce);

	vecAdd(fTotalForce, fCalculatedForce, fTotalForce);	
}

void calculateAcceleration(planet *pElement) {
	pElement->m_fAcceleration[0] = pElement->m_fForce[0] / pElement->m_fMass;
	pElement->m_fAcceleration[1] = pElement->m_fForce[1] / pElement->m_fMass;
	pElement->m_fAcceleration[2] = pElement->m_fForce[2] / pElement->m_fMass;
}

void calculateVelocityAndPosition(planet *pElement, solarSystem *pSystem) {
	// Get starting position coordinates
	float fStartPosition[4] = {0.0f};
	vecCopy(pElement->m_fPosition, fStartPosition);

	//Acceleration * timestep squared halved
	float fAccelerationTime[3] = {0.0f};
	float fTimeStepSquared = pSystem->m_fTimeStep * pSystem->m_fTimeStep;
	vecScalarProduct(pElement->m_fAcceleration, fTimeStepSquared, fAccelerationTime);
	vecScalarProduct(fAccelerationTime, 0.5f, fAccelerationTime);

	//Velocity * interval
	float fVelocityInterval[3] = {0.0f};	
	vecScalarProduct(pElement->m_fVelocity, pSystem->m_fTimeStep, fVelocityInterval);
	
	// Sum the two
	float fTotalCalculated[3] = {0.0f};
	vecAdd(fVelocityInterval, fAccelerationTime, fTotalCalculated);
		
	// Calculate the new position
	vecAdd(fStartPosition, fTotalCalculated, pElement->m_fPosition);
		
	// Calculate the resulting velocity 
	pElement->m_fVelocity[0] = (pElement->m_fPosition[0] - fStartPosition[0]) / pSystem->m_fTimeStep;
	pElement->m_fVelocity[1] = (pElement->m_fPosition[1] - fStartPosition[1]) / pSystem->m_fTimeStep;
	pElement->m_fVelocity[2] = (pElement->m_fPosition[2] - fStartPosition[2]) / pSystem->m_fTimeStep;
	
	if (pElement->m_fVelocity[0] > pSystem->m_fMaxVelocity)
		pElement->m_fVelocity[0] = pSystem->m_fMaxVelocity;
	else if (pElement->m_fVelocity[0] < pSystem->m_fMaxNegativeVelocity)
		pElement->m_fVelocity[0] = pSystem->m_fMaxNegativeVelocity;

	if (pElement->m_fVelocity[1] > pSystem->m_fMaxVelocity)
		pElement->m_fVelocity[1] = pSystem->m_fMaxVelocity;
	else if (pElement->m_fVelocity[1] < pSystem->m_fMaxNegativeVelocity)
		pElement->m_fVelocity[1] = pSystem->m_fMaxNegativeVelocity;

	if (pElement->m_fVelocity[2] > pSystem->m_fMaxVelocity)
		pElement->m_fVelocity[2] = pSystem->m_fMaxVelocity;
	else if (pElement->m_fVelocity[2] < pSystem->m_fMaxNegativeVelocity)
		pElement->m_fVelocity[2] = pSystem->m_fMaxNegativeVelocity;

	// Dampen the velocity
	vecScalarProduct(pElement->m_fVelocity, pSystem->m_fDragCoefficient, pElement->m_fVelocity);
}

/*
	Returns 0 if the main body was removed
	Returns 1 if not 
*/
int calculateWithinPositionBounds(planet *pBody, solarSystem *pSystem) {
	if (pBody->m_fPosition[0] > pSystem->m_fMaxPosition || pBody->m_fPosition[0] < pSystem->m_fMaxNegativePosition) {
		destroy(popMiddle(pBody));
		return 0;
	} else if (pBody->m_fPosition[1] > pSystem->m_fMaxPosition || pBody->m_fPosition[1] < pSystem->m_fMaxNegativePosition) {
		destroy(popMiddle(pBody));
		return 0;
	} else if (pBody->m_fPosition[2] > pSystem->m_fMaxPosition || pBody->m_fPosition[2] < pSystem->m_fMaxNegativePosition) {
		destroy(popMiddle(pBody));
		return 0;
	}
	return 1;
}

/*
	Returns 0 if the main body was removed
	Returns 1 if not 
*/
int detectCollision(planet *pBody, planet *pOtherBody) {
	if (pBody != pOtherBody) {
		float fDistanceBetweenPos[3] = {0.0f};
		vecSub(pOtherBody->m_fPosition, pBody->m_fPosition, fDistanceBetweenPos);	
		float fDistance = (fDistanceBetweenPos[0] * fDistanceBetweenPos[0]) + (fDistanceBetweenPos[1] * fDistanceBetweenPos[1]) + (fDistanceBetweenPos[2] * fDistanceBetweenPos[2]);

		float fBodySize = pBody->m_fSize * pBody->m_fSize;
		float fOtherSize = pOtherBody->m_fSize * pOtherBody->m_fSize;		
			
		if (fDistance < (fBodySize + fOtherSize)) {
			if (pOtherBody->m_fMass > pBody->m_fMass) {
				pOtherBody->m_fMass += pBody->m_fMass / 2;
				destroy(popMiddle(pBody));
				return 0;
			}
		}
	}
	return 1;
}

void calculateCollisionsAndWithinBounds(planet *pBody, planet *pHead, solarSystem *pSystem) {
	if (calculateWithinPositionBounds(pBody, pSystem) > 0) {
		if (detectCollision(pBody, pSystem->m_pSun) == 0) {			
			pSystem->m_iCurrentPlanets--;
			return;
		}
		for (planet *pOtherBody = pHead; pOtherBody;) {			
			planet *pNext = pOtherBody->m_pNext;
			if (detectCollision(pBody, pOtherBody) == 0) {
				pSystem->m_iCurrentPlanets--;
				return;
			}
			pOtherBody = pNext;
		}	
	}	
	else		
		pSystem->m_iCurrentPlanets--;
}