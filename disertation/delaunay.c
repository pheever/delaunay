#include "definitions.h"

//Point * l;
int pcnt = 0;

int main(int argc, char *args[]){

	FILE * file;
	Point * temp;	
	Triangle * T;
	temp = (Point*) calloc(1, sizeof(Point));
	int i, j;

	if(argc < 2){
		printf("No input file given!\n");
		exit(0);
	}

	if ( !(file = fopen(args[1], "r"))){
		printf("Unable to open file: %s\n", args[1]);
		exit(0);
	}

	while (fscanf(file, "%d %f %f", &temp->id, &temp->x.f, &temp->y.f) == 3){
		addPoint(temp);
		temp = (Point*) malloc(sizeof(Point));
	}
	if(!head){
		printf("File format error or file empty.\n");
		exit(0);
	}
/*
	temp = head;
	while(temp){
		PRINTPO(temp);
		temp = temp->next;
	}

//*/

	initializePoints();
	delaunay();
//	legalTriangulation();	
	temp = head;//->next->next;
	//uncomment the following code to get triangulation with p-1 and p-2 inserted
	DDEBUG("%d %d\n", pcnt+2, tcnt);
	while(temp){
		DDEBUG("%d %.2f %.2f\n", temp->id, temp->x.f, temp->y.f);
		temp = temp->next;
	}

	T = triHead;
	while(T){
		for(i=0; i<3; ++i){
			temp = head;//->next->next;
			for(j=1; temp->id != T->points[i]->id; ++j, temp = temp->next);
			printf("%d ", j);
		}
		printf("\n");
		T = T->next;
	}

	while(head){
		temp = head->next;
		free(head);
		head = temp;
	}
	while(triHead){
		T = triHead->next;
		free(triHead);
		triHead = T;
	}
//*/	
	return 1;
}

