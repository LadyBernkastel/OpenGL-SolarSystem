#include <stdio.h>
#include "planet.h"
#include "raaMaths/raaMaths.h"
#include "raaMaths/raaVector.h"

planet *g_pHead=0;
planet *g_pTail=0;


planet* createNode() {
	planet* pNode = new planet;
	pNode->m_pLast = 0;
	pNode->m_pNext = 0;
	pNode->m_pTrailHead = 0;
	pNode->m_pTrailTail = 0;
	pNode->m_iNoTrailPos = 0;
	return pNode;
}

int pushHead(planet *pNode) {
	if (pNode) {
		if(!pNode->m_pLast && !pNode->m_pNext) {
			if (!g_pHead && !g_pTail) {
				g_pHead = pNode;
				g_pTail = pNode;
				return 0;
			}

			pNode->m_pNext = g_pHead; // set new node's next to current head
			g_pHead->m_pLast = pNode; // set head node's last to new node
			g_pHead = pNode;
			return 0;
		}
	}
	return 1; // exit with error
}

int pushTail(planet *pNode) {
	if (pNode) { // if pNode is a valid address
		if(!pNode->m_pLast && !pNode->m_pNext) {  // and if pNode's last and next are empty
			if (!g_pHead && !g_pTail) { // if there is no node in the list
				g_pHead = pNode; // set head and tail to be this node
				g_pTail = pNode;
				return 0;
			}

			pNode->m_pLast = g_pTail; // set new node's last to current tail
			g_pTail->m_pNext = pNode; // set tail node's next to new node
			g_pTail = pNode;
			return 0;
		}
	}
	return 1; // exit with error
}


int pushMiddle(planet *pNode, planet *pInsertAfter) {
	if (pNode) { // if pNode is a valid address
		if(!pNode->m_pLast && !pNode->m_pNext) {  // and if pNode's last and next are empty
			if (!g_pHead && !g_pTail) { // if there is no node in the list
				g_pHead = pNode; // set head and tail to be this node
				g_pTail = pNode;
				return 0;
			}
			planet *pNext = g_pHead;
			while (pNext) { // while there is an element in the list
				if (pNext == pInsertAfter) {
					
				}
			}
		}
	}
	return 1;
}

planet* popHead() {
	planet* pNode = new planet;
	if (g_pHead) { // if the head is not 0
		pNode = g_pHead; // pop the node
		if (g_pHead == g_pTail) { // if the head and tail are the same element		
			g_pHead = g_pTail = 0; // set the head and tail to 0		
		}
		else {
			g_pHead = pNode->m_pNext; // set the new head to be the next node
			g_pHead->m_pLast = 0; // set the new head's last to be 0
		}
		pNode->m_pLast = 0; // destroy links
		pNode->m_pNext = 0;
		return pNode;
	}
	return 0;
}

planet* popTail() {
	planet* pNode = new planet;
	if (g_pTail) { // if the tail is not 0
		pNode = g_pTail; // pop the node
		if (g_pHead == g_pTail) { // if the head and tail are the same element		
			g_pHead = g_pTail = 0; // set the head and tail to 0		
		}
		else {
			g_pTail = pNode->m_pLast; // set the new tail to be the previous node
			g_pTail->m_pNext = 0; // set the new tail's next to be 0
		}
		pNode->m_pLast = 0; // destroy links
		pNode->m_pNext = 0;		
		return pNode;
	}
	return 0;
}

planet* popMiddle(planet *pNode) {	
	if (g_pHead == g_pTail) { // if the head and tail are the same element		
		g_pHead = g_pTail = 0; // set the head and tail to 0		
	} else {
		// If the node is the head
		if (pNode == g_pHead) {
			g_pHead = pNode->m_pNext;
			g_pHead->m_pLast = 0;
		} else if (pNode == g_pTail) {
			g_pTail = pNode->m_pLast;
			g_pTail->m_pNext = 0;
		} else {
			pNode->m_pLast->m_pNext = pNode->m_pNext;
			pNode->m_pNext->m_pLast = pNode->m_pLast;
		}
		pNode->m_pLast = 0;
		pNode->m_pNext = 0;
		return pNode;
	}
	return 0;
}

bool destroy(planet *pNode) {
	if (pNode) {
		if(!pNode->m_pLast && !pNode->m_pNext) {
			if (g_pHead != pNode && g_pHead != pNode) {
				delete pNode;
				return 1;
			}
		}
	}
	return 0;
}

trailPosition* createTrailPosition(float* fPosition) {
	trailPosition* tTrail = new trailPosition;
	tTrail->m_pLast = 0;
	tTrail->m_pNext = 0;
	vecCopy(fPosition, tTrail->m_fPosition);
	return tTrail;
}

int pushTrailHead(planet *pPlanet, trailPosition *pPos) {
	if (pPos) {
		if(!pPos->m_pLast && !pPos->m_pNext) {
			if (!pPlanet->m_pTrailHead && !pPlanet->m_pTrailTail) {
				pPlanet->m_pTrailHead = pPos;
				pPlanet->m_pTrailTail = pPos;
				pPlanet->m_iNoTrailPos++;
				return 0;
			}
			pPos->m_pNext = pPlanet->m_pTrailHead; // set new node's next to current head
			pPlanet->m_pTrailHead->m_pLast = pPos; // set head node's last to new node
			pPlanet->m_pTrailHead = pPos;
			pPlanet->m_iNoTrailPos++;
			return 0;
		}
	}
	return 1; // exit with error
}

trailPosition* popTrailTail(planet *pPlanet) {
	trailPosition* pNode = new trailPosition;
	if (pPlanet->m_pTrailTail) { // if the tail is not 0
		pNode = pPlanet->m_pTrailTail; // pop the node
		if (pPlanet->m_pTrailHead == pPlanet->m_pTrailTail) { // if the head and tail are the same element		
			pPlanet->m_pTrailHead = pPlanet->m_pTrailTail = 0; // set the head and tail to 0		
		}
		else {
			pPlanet->m_pTrailTail = pNode->m_pLast; // set the new tail to be the previous node
			pPlanet->m_pTrailTail->m_pNext = 0; // set the new tail's next to be 0
		}
		pNode->m_pLast = 0; // destroy links
		pNode->m_pNext = 0;			
		pPlanet->m_iNoTrailPos--;
		return pNode;
	}
	return 0;
}

bool destroyTrailPos(planet *pPlanet, trailPosition *pNode) {
	if (pNode) {
		if(!pNode->m_pLast && !pNode->m_pNext) {
			if (pPlanet->m_pTrailHead != pNode && pPlanet->m_pTrailHead != pNode) {
				delete pNode;
				return 1;
			}
		}
	}
	return 0;
}

void generateColourVertex(planet *pPlanet) {
	for (int i = 0; i < pPlanet->m_iNoTrailPos * 3; i +=3) {
		pPlanet->m_vfColourVertex[i] = pPlanet->m_fColour[0];
		pPlanet->m_vfColourVertex[i+1] = pPlanet->m_fColour[1];
		pPlanet->m_vfColourVertex[i+2] = pPlanet->m_fColour[2];
	}
		
	if (pPlanet->m_iNoTrailPos == cs_iMAX_TRAIL_POS) {
		int iStartFade = (pPlanet->m_iNoTrailPos * 3) - 150;

		float fRFade = -(pPlanet->m_fColour[0] / 50);
		float fGFade = -(pPlanet->m_fColour[1] / 50);
		float fBFade = -(pPlanet->m_fColour[2] / 50);

		int iFadeNo = 1;
		for (int i = iStartFade; i < pPlanet->m_iNoTrailPos * 3; i += 3) {
			pPlanet->m_vfColourVertex[i] = pPlanet->m_fColour[0] + fRFade * iFadeNo;
			pPlanet->m_vfColourVertex[i+1] = pPlanet->m_fColour[1] + fGFade * iFadeNo;
			pPlanet->m_vfColourVertex[i+2] = pPlanet->m_fColour[2] + fBFade * iFadeNo;
			iFadeNo++;
		}
	}		
}

void generatePositionVertex(planet *pPlanet) {		
	trailPosition *pPos = pPlanet->m_pTrailHead;
	pPos = pPlanet->m_pTrailHead;
	for (int i = 0; i < pPlanet->m_iNoTrailPos * 3; i +=3) {
		pPlanet->m_vfTrailVertex[i] = pPos->m_fPosition[0];
		pPlanet->m_vfTrailVertex[i+1] = pPos->m_fPosition[1];
		pPlanet->m_vfTrailVertex[i+2] = pPos->m_fPosition[2];
		pPos = pPos->m_pNext;
	}
}

void printList() {
	unsigned int uiCount = 0;
	for (planet *pElement = g_pHead; pElement; pElement = pElement->m_pNext) {
		printf("Element: %d\n", uiCount++);
		printf("Mass: %f\n", pElement->m_fMass);
	}
}

