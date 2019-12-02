#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "runway.h"
//struct of a Node in a List of Flights on a runway
struct flightNode {
	pFlight flight_pointer;
	pNode nextFlightNode;
};
//struct of a runway - holds ID, type (DOMESTIC/INTERNATIONAL) and a pointer to a list of flight Nodes
struct runway {
	int runwayID;
	FlightType runwayType;
	pNode pList;
};
/*
	function name: createRunway
	Description: create a new runway instance with the given ID and type
	Parameters: runwayID - runway id, type - FlightType(DOMESTIC/INTERNATIONAL)
	Return Value: pointer to the new runway instance, NULL is returned if the operation is failed.
*/
pRunway createRunway(int runwayID, FlightType type) {
	if (runwayID > 0 && runwayID <= MAX_ID && (type == DOMESTIC || type == INTERNATIONAL)) {//check if ID and type are valid
		pRunway runway_pointer = (pRunway)malloc(sizeof(struct runway));
		if (runway_pointer == NULL)
			return NULL;
		runway_pointer->runwayID = runwayID;
		runway_pointer->runwayType = type;
		runway_pointer->pList = NULL;
		return runway_pointer;

	}
	return NULL;
}
/*
	function name: createTempRunway
	Description: create a new runway instance with the given ID and type - an internal use function
	Parameters: runwayID - runway id, type - FlightType(DOMESTIC/INTERNATIONAL)
	Return Value: pointer to the new runway instance, NULL is returned if the operation is failed.
*/
pRunway createTempRunway(int runwayID, FlightType type) {
	if (runwayID == MAX_ID + 1 && (type == DOMESTIC || type == INTERNATIONAL)) {//the id must be out of bounds of real id limits, in order to make sure this temp runway is always available to create
		pRunway runway_pointer = (pRunway)malloc(sizeof(struct runway));
		if (runway_pointer == NULL)
			return NULL;
		runway_pointer->runwayID = runwayID;
		runway_pointer->runwayType = type;
		runway_pointer->pList = NULL;
		return runway_pointer;

	}
	return NULL;
}
/*
	function name: destroyRunway
	Description: destroy arunway instance by given pointer
	Parameters: runway_pointer - pointer to a runway struct
	Return Value: pointer to the new runway instance, NULL is returned if the operation is failed.
*/
void destroyRunway(pRunway runway_pointer) {
	if (runway_pointer == NULL)
		return;
	pNode tmp_pointer;
	while (runway_pointer->pList != NULL) {
		tmp_pointer = runway_pointer->pList;
		runway_pointer->pList = tmp_pointer->nextFlightNode;
		destroyFlight(tmp_pointer->flight_pointer);
		free(tmp_pointer);
	}
	free(runway_pointer);
	runway_pointer = NULL;
	return;

}
BOOL isFlightExists(pRunway runway_pointer, int flight_id) {
	if (runway_pointer == NULL || flight_id < 1 || flight_id > MAX_ID)
		return FALSE;
	pNode tmp_pointer = runway_pointer->pList;
	while (tmp_pointer != NULL) {
		if (getFlightID(tmp_pointer->flight_pointer) == flight_id)// if the flight is exist
			return TRUE;
		tmp_pointer = tmp_pointer->nextFlightNode;
	}
	return FALSE; //if the flight isn't exist

}

int getFlightNum(pRunway runway_pointer) {
	if (runway_pointer == NULL)
		return -1;

	int counter = 0;
	pNode tmp_pointer = runway_pointer->pList;
	while (tmp_pointer != NULL) {
		counter++;
		tmp_pointer = tmp_pointer->nextFlightNode;
	}
	return counter;
}


int getEmergencyNum(pRunway runway_pointer) {
	if (runway_pointer == NULL)
		return -1;
	int counter = 0;
	pNode tmp_pointer = runway_pointer->pList;
	while (tmp_pointer != NULL) {
		if (getIsEmergency(tmp_pointer->flight_pointer) == TRUE)// if the flight is emergency
			counter++;
		tmp_pointer = tmp_pointer->nextFlightNode;
	}
	return counter;

}
Result addFlight(pRunway runway_pointer, pFlight flight_pointer) {
	if (runway_pointer == NULL || flight_pointer == NULL || (isFlightExists(runway_pointer, getFlightID(flight_pointer))) == TRUE
		|| (runway_pointer->runwayType != getFlightType(flight_pointer)))
		return FAILURE;
	pNode pFlightNode = (pNode)malloc(sizeof(struct flightNode));
	if (pFlightNode == NULL)
		return FAILURE;
	pFlightNode->flight_pointer = flight_pointer;
	pNode tmp_pointer = runway_pointer->pList;
	if (tmp_pointer == NULL) {//if this is the first flight
		runway_pointer->pList = pFlightNode;
		pFlightNode->nextFlightNode = NULL;
		return SUCCESS;
	}
	if (getIsEmergency(flight_pointer) == TRUE) {
		if (getIsEmergency(tmp_pointer->flight_pointer) == FALSE) {//check if the first flight is emergency
			runway_pointer->pList = pFlightNode;
			pFlightNode->nextFlightNode = tmp_pointer;
			return SUCCESS;
		}
		while (tmp_pointer->nextFlightNode != NULL &&
			getIsEmergency((tmp_pointer->nextFlightNode)->flight_pointer) == TRUE) {

			tmp_pointer = tmp_pointer->nextFlightNode;
		}
		pFlightNode->nextFlightNode = tmp_pointer->nextFlightNode;
		tmp_pointer->nextFlightNode = pFlightNode;
		return SUCCESS;
	}
	else {//if the flight we want to add isn't emergency
		while (tmp_pointer->nextFlightNode != NULL)
			tmp_pointer = tmp_pointer->nextFlightNode;
		pFlightNode->nextFlightNode = NULL;
		tmp_pointer->nextFlightNode = pFlightNode;
		return SUCCESS;
	}

	return FAILURE; //shouldn't get here

}
Result removeFlight(pRunway runway_pointer, int flightID) {
	if (runway_pointer == NULL || runway_pointer->pList == NULL || flightID < 1 || flightID > MAX_ID)
		return FAILURE;
	pNode tmp_pointer = runway_pointer->pList;
	pNode tmp_pointer_last = tmp_pointer;
	while (tmp_pointer != NULL) {
		if (getFlightID(tmp_pointer->flight_pointer) == flightID) {
			if (tmp_pointer == runway_pointer->pList) //if it's the first flight
				runway_pointer->pList = tmp_pointer->nextFlightNode;
			else
				tmp_pointer_last->nextFlightNode = tmp_pointer->nextFlightNode;
			destroyFlight(tmp_pointer->flight_pointer);
			free(tmp_pointer);
			return SUCCESS;
		}
		tmp_pointer_last = tmp_pointer;//should check if it's legal when we remove the first flight
		tmp_pointer = tmp_pointer->nextFlightNode;
	}
	return FAILURE; //if it didn't find the flightID
}
Result departFromRunway(pRunway runway_pointer) {
	if (runway_pointer == NULL)
		return FAILURE;
	else if (runway_pointer->pList == NULL)
		return FAILURE;
	pNode tmp_pointer = runway_pointer->pList;
	runway_pointer->pList = tmp_pointer->nextFlightNode;
	destroyFlight(tmp_pointer->flight_pointer);
	free(tmp_pointer);
	return SUCCESS;
}
void printRunway(pRunway runway_pointer) {
	if (runway_pointer == NULL)
		return;
	pNode tmp_pointer = runway_pointer->pList;
	int flightNum = getFlightNum(runway_pointer);
	if (runway_pointer->runwayType == INTERNATIONAL)
		printf("Runway %d international\n", runway_pointer->runwayID);
	if (runway_pointer->runwayType == DOMESTIC)
		printf("Runway %d domestic\n", runway_pointer->runwayID);

	printf("%d flights are waiting:\n", flightNum);
	while (tmp_pointer != NULL) {
		printFlight(tmp_pointer->flight_pointer);
		tmp_pointer = tmp_pointer->nextFlightNode;
	}



}

int getRunwayID(pRunway runway_pointer) {
	return runway_pointer->runwayID;

}
FlightType getRunwayType(pRunway runway_pointer) {
	return runway_pointer->runwayType;
}

void changeDestRunway(pRunway runway_pointer, char* old_dest, char* new_dest) {
	if (runway_pointer == NULL)
		return;
	pNode tmp_pointer = runway_pointer->pList;
	while (tmp_pointer != NULL) {
		if (strcmp(getFlightDest(tmp_pointer->flight_pointer), old_dest) == 0) {
			changeDestFlight(tmp_pointer->flight_pointer, new_dest);
		}
		tmp_pointer = tmp_pointer->nextFlightNode;
	}
	return;
}

pNode unlinkFlight(pNode previousNode) {
	if (previousNode == NULL || previousNode->nextFlightNode == NULL)
		return NULL;
	pNode tmp_node = previousNode->nextFlightNode;
	previousNode->nextFlightNode = tmp_node->nextFlightNode;
	return tmp_node;

}


void runwayDelay(pRunway runway_pointer, char* dest) {
	if (runway_pointer == NULL)
		return;
	pRunway tmpRunway = createRunway(1, runway_pointer->runwayType);
	//pNode head = NULL;
	//pNode tmp_pointer_delay = NULL;
	pNode tmp_pointer = runway_pointer->pList;
	//int first_inserted = 0;

	//if (strcmp(getFlightDest(tmp_pointer->flight_pointer), dest) == 0) {
	//	head = tmp_pointer;
	//	runway_pointer->pList = tmp_pointer->nextFlightNode;
	//	head->nextFlightNode = NULL;
	//	first_inserted = 1;
	//	tmp_pointer_delay = head;
	//}
	int flightID;
	FlightType type;
	BOOL isEmergency;
	char* tempFlightDest;
	char flightDest[4];
	pFlight tmpFlight;
	while (tmp_pointer != NULL) {
		if (strcmp(getFlightDest(tmp_pointer->flight_pointer), dest) == 0) {
			flightID = getFlightID(tmp_pointer->flight_pointer);
			type = getFlightType(tmp_pointer->flight_pointer);
			isEmergency = getIsEmergency(tmp_pointer->flight_pointer);
			tempFlightDest = getFlightDest(tmp_pointer->flight_pointer);
			int i;
			for (i = 0; i <= 3; i++) {
				flightDest[i] = tempFlightDest[i];
			}
			//printf("1234:%s\n", flightDest); //for debug
			tmp_pointer = tmp_pointer->nextFlightNode;//must do it before removing flight
			removeFlight(runway_pointer, flightID);
			//printf("1234:%s\n", flightDest);
			tmpFlight = createFlight(flightID, type, flightDest, isEmergency);
			addFlight(tmpRunway, tmpFlight); // no handling of addflight return value
		}
		else
			tmp_pointer = tmp_pointer->nextFlightNode;

	}
	tmp_pointer = tmpRunway->pList;
	while (tmp_pointer != NULL) {
		flightID = getFlightID(tmp_pointer->flight_pointer);
		type = getFlightType(tmp_pointer->flight_pointer);
		isEmergency = getIsEmergency(tmp_pointer->flight_pointer);
		tempFlightDest = getFlightDest(tmp_pointer->flight_pointer);
		int i;
		for (i = 0; i <= 3; i++) {
			flightDest[i] = tempFlightDest[i];
		}
		tmp_pointer = tmp_pointer->nextFlightNode;//must do it before removing flight
		removeFlight(tmpRunway, flightID);
		tmpFlight = createFlight(flightID, type, flightDest, isEmergency);
		addFlight(runway_pointer, tmpFlight);

	}
	destroyRunway(tmpRunway);
	return;
}

