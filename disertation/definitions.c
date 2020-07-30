#include "definitions.h"

int rec;
int count;

int pointInTriangle(Triangle * T, Point * p){
	float a, b, c;
	a = ((T->points[1]->y.f - T->points[2]->y.f) * (p->x.f-T->points[2]->x.f) + (T->points[2]->x.f - T->points[1]->x.f) * (p->y.f - T->points[2]->y.f)) /
			((T->points[1]->y.f - T->points[2]->y.f) * (T->points[0]->x.f - T->points[2]->x.f) + (T->points[2]->x.f - T->points[1]->x.f) * (T->points[0]->y.f - T->points[2]->y.f));
	b = ((T->points[2]->y.f - T->points[0]->y.f) * (p->x.f-T->points[2]->x.f) + (T->points[0]->x.f - T->points[2]->x.f) * (p->y.f - T->points[2]->y.f)) /
			((T->points[1]->y.f - T->points[2]->y.f) * (T->points[0]->x.f - T->points[2]->x.f) + (T->points[2]->x.f - T->points[1]->x.f) * (T->points[0]->y.f - T->points[2]->y.f));
	c = 1.0 - a - b;

	if		(CCW(T->edges[0]->a, p, T->edges[0]->b) == 0 && P_ON_E(T->edges[0], p)){	return 0;}
	else if (CCW(T->edges[1]->a, p, T->edges[1]->b) == 0 && P_ON_E(T->edges[1], p)){	return 1;}
	else if (CCW(T->edges[2]->a, p, T->edges[2]->b) == 0 && P_ON_E(T->edges[2], p)){	return 2;}
	else if (a > 0 && b > 0 && c > 0){		return -1;}
	else{									return -2;}
}

void legalizeEdge(Point * point, Edge * edge, Triangle * T){
	rec++;
	Circle * circle;
	Triangle * A = (edge->t[0] == T ? edge->t[1] : edge->t[0]);
	int ap, ae, tp, te;
	if(!A){
		rec--;
		return;
	}

	if(!validateTriangle(T)){
		DEBUG("-5\nLegalize Edge got false triangle T!");
		exit(0);
	}
	if(!validateTriangle(A)){
		DEBUG("-5\nLegalize Edge found false triangle A!");
		exit(0);
	}
	if(compareTriangles(T, A)){
		DEBUG("-5");
		PRINTRI(T, " T false input");
		PRINTRI(A, " A false input");
		failure(-5);
	}
	//PRINTRI(A, "A B4 LE");
	
	circle = calculateCircleCenter(T->points[0], T->points[1], T->points[2]);
	if(!circle){
		DEBUG("-2");
		PRINTRI(T, " Circle Triangle ");
		failure(-2);
	}
    for(ae=0; edge!=A->edges[ae]; ++ae);	//find common edge in adjacent triangle
    for(te=0; edge!=T->edges[te]; ++te);	//find common edge in triangle
    for(ap=0; EDGE_CONTAINS(edge, A->points[ap]); ++ap);	//find non common point in adjacent triangle
    for(tp=0; EDGE_CONTAINS(edge, T->points[tp]); ++tp);	//find non common point in triangle

	if( DISQR(A->points[ap], circle->center) >= circle->r.f){
		free(circle->center);
		free(circle);
		rec--;
		return;
	}
	free(circle->center);
	free(circle);
	//Switch points
	A->points[NEXT(ap)] = T->points[tp];
	T->points[(T->points[NEXT(tp)] == A->points[PREV(ap)] ? NEXT(tp) : PREV(tp))] = A->points[ap];
	//Change points in common edge
	edge->a = T->points[tp];
	edge->b = A->points[ap];
	//Find the edge in T to pass to A (the edge that contains the (previously common) point that will now be in A and not in T)
	te = (EDGE_CONTAINS(T->edges[NEXT(te)], A->points[PREV(ap)]) ? NEXT(te) : PREV(te));
	//Find the edge in A that doesnt contain the new unique point
	ae = (EDGE_CONTAINS(A->edges[NEXT(ae)], A->points[PREV(ap)]) ? PREV(ae) : NEXT(ae));
	
	//Switch edges
	edge = T->edges[te];
	T->edges[te] = A->edges[ae];
	A->edges[ae] = edge;
	//Fix adjecency values
	A->edges[ae]->t[(A->edges[ae]->t[0] == T ? 0 : 1)] = A;
	T->edges[te]->t[(T->edges[te]->t[0] == A ? 0 : 1)] = T;

	if( !validateTriangle(T)){
		DEBUG("-3");
		PRINTRI(T, " FAIL T B4 legalizeEdge ");
		PRINTRI(A, " A B4 legalizeEdge ");
		failure(-4);
	}
	if( !validateTriangle(A) || CCW(A->points[0], A->points[1], A->points[2]) == 0){
		DEBUG("-3");
		PRINTRI(T, " T B4 legalizeEdge ");
		PRINTRI(A, " FAIL A B4 legalizeEdge ");
		failure(-4);
	}

	//find edge in T that doesnt contain point
	for(te=0; EDGE_CONTAINS(T->edges[te], point); ++te);
	legalizeEdge(point, T->edges[te], T);
	//find edge in A that doesnt contain point
	for(ae=0; EDGE_CONTAINS(A->edges[ae], point); ++ae);
	legalizeEdge(point, A->edges[ae], A);

	//Check new triangles
	if( !validateTriangle(T)){
		DEBUG("-3");
		PRINTRI(T, " FAIL T legalizeEdge ");
		PRINTRI(A, " A legalizeEdge ");
		failure(-4);
	}
	if( !validateTriangle(A)){
		DEBUG("-3");
		PRINTRI(T, " T legalizeEdge ");
		PRINTRI(A, " FAIL A legalizeEdge ");
		failure(-4);
	}
	rec--;
}

Circle * calculateCircleCenter(Point * p1, Point * p2, Point * p3){
	float m21,m32, m13;
	Circle * c;


	m21 = (p2->y.f - p1->y.f) / (p2->x.f - p1->x.f);
	m32 = (p3->y.f - p2->y.f) / (p3->x.f - p2->x.f);
	m13 = (p1->y.f - p3->y.f) / (p1->x.f - p3->x.f);

	if(m21==m32 || m21==m13 || m32==m13){
		return NULL;
	}
	c = (Circle*) malloc(sizeof(Circle));
	c->center =(Point*) malloc(sizeof(Point));

	if(!VERTICAL(p1, p2) && !VERTICAL(p2, p3) && m21!=m32){//p2 common point
		c->center->x.f = (m21 * m32 * (p3->y.f - p1->y.f) + m21 * (p2->x.f + p3->x.f) - m32 * (p1->x.f + p2->x.f)) / (2 * (m21- m32));
		c->center->y.f = (-(1/(m21?m21:m32))) * (c->center->x.f - ((m21?p1->x.f:p3->x.f) + p2->x.f)/2) + ((m21?p1->y.f:p3->y.f) + p2->y.f) / 2;
	}
	else if(!VERTICAL(p1, p3) && !VERTICAL(p2, p3) && m32!=m13){//p3 common point
		c->center->x.f = (m32 * m13 * (p1->y.f - p2->y.f) + m32 * (p3->x.f + p1->x.f) - m13 * (p2->x.f + p3->x.f)) / (2 * (m32- m13));
		c->center->y.f = (-(1/(m32?m32:m13))) * (c->center->x.f - ((m32?p2->x.f:p1->x.f) + p3->x.f)/2) + ((m32?p2->y.f:p1->y.f) + p3->y.f) / 2;
	}
	else{//p1 common point
		c->center->x.f = (m13 * m21 * (p2->y.f - p3->y.f) + m13 * (p1->x.f + p2->x.f) - m21 * (p3->x.f + p1->x.f)) / (2 * (m13- m21));
		c->center->y.f = (-(1/(m13?m13:m21))) * (c->center->x.f - ((m13?p3->x.f:p2->x.f) + p1->x.f)/2) + ((m13?p3->y.f:p2->y.f) + p1->y.f) / 2;
	}
	c->r.f = DISQR(p1, c->center);
	return c;
}



void delaunay(void){
	Point * points = head->next->next->next, * p;
	Edge * ab;
	Triangle * T = (Triangle*) malloc(sizeof(Triangle)), * A, * B, * C;
	int i, e, ae, be, te;
	triHead = T;
	T->next = NULL;

	tcnt = 1;
	T->points[0] = head;
	T->points[1] = head->next;
	T->points[2] = head->next->next;

	for(i=0; i<3; ++i){
		T->edges[i] = (Edge*) malloc(sizeof(Edge));
		T->edges[i]->a = T->points[i];
		T->edges[i]->b = T->points[NEXT(i)];
		T->edges[i]->t[0] = T;
		T->edges[i]->t[1] = (Triangle*) 0;
	}

	for(i=1, points = head->next->next->next; points; points = points->next, i++){
		for(T = triHead; T; T = T->next){
			e = pointInTriangle(T, points);
			switch( e ){
				case -2: continue;
				case -1: goto pit;
				default: goto poe;
			}
		}
///*
		DEBUG("-1");
		DDEBUG("%d\n", pcnt+2);
		PRINTPO(points);
		printf("low: "); PRINTPO(low);
		while(points->next)
			points = points->next;
		//slopeQsort(head->next, points, head);
		points = head;
		while(points){
			printf("%d %.2f %.2f\n", points->id, points->x.f, points->y.f);
			points = points->next;
		}
		T = triHead;
		DDEBUG("%d\n", tcnt);
		while(T){
			DDEBUG("%d %d %d\n", T->points[0]->id, T->points[1]->id, T->points[2]->id);
			T = T->next;
		}
//*/
		//POINT ON EDGE CASE
		poe:
		tcnt+=2;

//		PRINTRI(T, "T in POE");
		A = T->edges[e]->t[(T->edges[e]->t[0] == T? 1 : 0)];
		if(!A){
			failure(-4);
		}
/*
		DEBUG("Triangle T adjacencies: ");
		for(te=0; te<3; ++te){
			DDEBUG("edge: %d - %p & %p\n",te,  T->edges[te]->t[0], T->edges[te]->t[1]);
		}
		DEBUG("Triangle A adjacencies: ");
		for(te=0; te<3; ++te){
			DDEBUG("edge: %d - %p & %p\n",te,  A->edges[te]->t[0], A->edges[te]->t[1]);
		}
		PRINTRI(A, "A in POE");
*/
		B = (Triangle*) malloc(sizeof(Triangle));
		B->next = triHead;
		triHead = B;
		C = (Triangle*) malloc(sizeof(Triangle));
		C->next = triHead;
		triHead = C;

		//Save mutual, to-be-switched point of A and T[j]
		p = T->edges[e]->a;
		//Replace points position with the new point
		T->edges[e]->a = points;
		for(te=0; T->points[te] != p; ++te);
		T->points[te] = points;
		for(ae=0; A->points[ae]!= p; ++ae);
		A->points[ae] = points;
		//Create mutual edge of T[j] and C
		ab = (Edge*) malloc(sizeof(Edge));
		ab->a = points;
		ab->b = T->points[(T->points[NEXT(te)] == T->edges[e]->b? PREV(te) : NEXT(te))];
		ab->t[0] = T;
		ab->t[1] = C;
		C->edges[0] = ab;
		//Find edge in T that doesnt contain the mutual not-to-be-switched point of A and B
		for(te=0; EDGE_CONTAINS(T->edges[te], T->edges[e]->b); ++te);
		ab = T->edges[te];
		T->edges[te] = C->edges[0];
		C->edges[1] = ab;
		C->edges[1]->t[(C->edges[1]->t[0] == T? 0 : 1)] = C;
		//Create mutual edge of A and B
		ab = (Edge*) malloc(sizeof(Edge));
		ab->a = points;
		ab->b = A->points[(A->points[NEXT(ae)] == T->edges[e]->b?PREV(ae) : NEXT(ae))];
		ab->t[0] = A;
		ab->t[1] = B;
		B->edges[0] = ab;
		//Find edge in T[j] that doesnt contain the mutual not-to-be-switched point of A and B
		for(te=0; EDGE_CONTAINS(A->edges[te], T->edges[e]->b); ++te);
		ab = A->edges[te];
		A->edges[te] = B->edges[0];
		B->edges[1] = ab;
		B->edges[1]->t[(B->edges[1]->t[0] == A? 0 : 1)] = B;
		//Create mutual edge of C and B
		ab = (Edge*) malloc(sizeof(Edge));
		ab->a= p;
		ab->b = points;
		ab->t[0] = B;
		ab->t[1] = C;
		C->edges[2] = B->edges[2] = ab;

		B->points[0] = C->points[0] = points;
		B->points[1] = B->edges[1]->a;
		B->points[2] = B->edges[1]->b;
		C->points[1] = C->edges[1]->a;
		C->points[2] = C->edges[1]->b;

/*		ae = 0;
		DEBUG("Triangle T adjacencies: ");
		for(te=0; te<3; ++te){
			DDEBUG("edge: %d - %p & %p\n",te,  T->edges[te]->t[0], T->edges[te]->t[1]);
		}
		DEBUG("Triangle A adjacencies: ");
		for(te=0; te<3; ++te){
			DDEBUG("edge: %d - %p & %p\n",te,  A->edges[te]->t[0], A->edges[te]->t[1]);
		}
		DEBUG("Triangle B adjacencies: ");
		for(te=0; te<3; ++te){
			DDEBUG("edge: %d - %p & %p\n",te,  B->edges[te]->t[0], B->edges[te]->t[1]);
		}
		DEBUG("Triangle C adjacencies: ");
		for(te=0; te<3; ++te){
			DDEBUG("edge: %d - %p & %p\n",te,  C->edges[te]->t[0], C->edges[te]->t[1]);
		}
/ *
		PRINTRI(T, " T after quadratic split");
		PRINTRI(A, " A after quadratic split");
		PRINTRI(B, " B after quadratic split");
		PRINTRI(C, " C after quadratic split");
*/

		if(compareTriangles(T, A)){
			DEBUG("-6\nT == A in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(T, B)){
			DEBUG("-6\nT == B in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(T, C)){
			DEBUG("-6\nT == C in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(A, B)){
			DEBUG("-6\nA == B in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(A, C)){
			DEBUG("-6\nA == C in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(B, C)){
			DEBUG("-6\nB == C in POE [phivos]");
			exit(0);
		}


///*		
		if(!validateTriangle(A) || CCW(A->points[0], A->points[1], A->points[2]) == 0){
			DEBUG("-4\nTRIANGLE A INVALID");
			PRINTRI(A, "A after quadradic split");
			failure(-4);
		}
		if(!validateTriangle(T) || CCW(T->points[0], T->points[1], T->points[2]) == 0){
			DEBUG("-4\nTRIANGLE T[j] INVALID");
			PRINTRI(T, "T[j] after quadradic split");
			failure(-4);
		}
		if(!validateTriangle(C) || CCW(C->points[0], C->points[1], C->points[2]) == 0){
			DEBUG("-4\nTRIANGLE C INVALID");
			PRINTRI(C, "C after quadradic split");
			failure(-4);
		}
		if(!validateTriangle(B) || CCW(B->points[0], B->points[1], B->points[2]) == 0){
			DEBUG("-4\nTRIANGLE B INVALID");
			PRINTRI(B, "B after quadradic split");
			failure(-4);
		}
//*/
		for(te=0; EDGE_CONTAINS(T->edges[te], points); ++te);
		legalizeEdge(points, T->edges[te], T);
		for(te=0; EDGE_CONTAINS(A->edges[te], points); ++te);
		legalizeEdge(points, A->edges[te], A);
		for(te=0; EDGE_CONTAINS(B->edges[te], points); ++te);
		legalizeEdge(points, B->edges[te], B);
		for(te=0; EDGE_CONTAINS(C->edges[te], points); ++te);
		legalizeEdge(points, C->edges[te], C);
		
		if(!validateTriangle(A) || CCW(A->points[0], A->points[1], A->points[2]) == 0){
			DEBUG("-4\nTRIANGLE A INVALID");
			PRINTRI(A, "A after quadradic split");
			failure(-4);
		}
		if(!validateTriangle(T) || CCW(T->points[0], T->points[1], T->points[2]) == 0){
			DEBUG("-4\nTRIANGLE T[j] INVALID");
			PRINTRI(T, "T[j] after quadradic split");
			failure(-4);
		}
		if(!validateTriangle(C) || CCW(C->points[0], C->points[1], C->points[2]) == 0){
			DEBUG("-4\nTRIANGLE C INVALID");
			PRINTRI(C, "C after quadradic split");
			failure(-4);
		}
		if(!validateTriangle(B) || CCW(B->points[0], B->points[1], B->points[2]) == 0){
			DEBUG("-4\nTRIANGLE B INVALID");
			PRINTRI(B, "B after quadradic split");
			failure(-4);
		}
		if(compareTriangles(T, A)){
			DEBUG("-6\nT == A in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(T, B)){
			DEBUG("-6\nT == B in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(T, C)){
			DEBUG("-6\nT == C in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(A, B)){
			DEBUG("-6\nA == B in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(A, C)){
			DEBUG("-6\nA == C in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(B, C)){
			DEBUG("-6\nB == C in POE [phivos]");
			exit(0);
		}
		//exit(0);
		continue;
		//POINT IN TRIANGLE CASE
		pit:
		tcnt+=2;

//		PRINTRI(T[j], "T[j] before split");

		A = (Triangle*) malloc(sizeof(Triangle));
		A->next = triHead;
		triHead = A;
		A->points[0] = T->points[1];
		A->points[1] = T->points[2];
		A->points[2] = points;

		B = (Triangle*) malloc(sizeof(Triangle));
		B->next = triHead;
		triHead = B;
		B->points[0] = T->points[2];
		B->points[1] = T->points[0];
		B->points[2] = points;

		/*Find the edge that doesnt contain the point given to triangle A*/
		for(ae=0; EDGE_CONTAINS(T->edges[ae], T->points[0]) ; ++ae);
		/*Find the edge that doesnt contain the point given to triangle B*/
		for(be=0; EDGE_CONTAINS(T->edges[be], T->points[1]) ; ++be);

		T->points[2] = points;

		A->edges[0] = T->edges[ae];
		A->edges[0]->t[(A->edges[0]->t[0] == T? 0 : 1)] = A;
		B->edges[0] = T->edges[be];
		B->edges[0]->t[(B->edges[0]->t[0] == T? 0 : 1)] = B;

		/*Create common edge between T and A*/
		T->edges[ae] = A->edges[1] = (Edge*) malloc(sizeof(Edge));
		A->edges[1]->a = points;
		A->edges[1]->b = T->points[1];
		A->edges[1]->t[0] = A;
		A->edges[1]->t[1] = T;
		/*Create common edge between T and B*/
		T->edges[be] = B->edges[1] = (Edge*) malloc(sizeof(Edge));
		B->edges[1]->a = points;
		B->edges[1]->b = T->points[0];
		B->edges[1]->t[0] = B;
		B->edges[1]->t[1] = T;
		/*Create common edge between A and B*/
		A->edges[2] = B->edges[2] = (Edge*) malloc(sizeof(Edge));
		A->edges[2]->a = points;
		A->edges[2]->b = A->points[1];
		A->edges[2]->t[0] = A;
		A->edges[2]->t[1] = B;

		if(!validateTriangle(A) || CCW(A->points[0], A->points[1], A->points[2]) == 0){
			DEBUG("-4\nTRIANGLE A INVALID - PIT - B4 LE");
			failure(-4);
		}
		if(!validateTriangle(B) || CCW(B->points[0], B->points[1], B->points[2]) == 0){
			DEBUG("-4\nTRIANGLE B INVALID - PTI - B4 LE");
			failure(-4);
		}
		if(!validateTriangle(T) || CCW(T->points[0], T->points[1], T->points[2]) == 0){
			DEBUG("-4\nTRIANGLE T[j] INVALID - PIT - B4 LE");
			DDEBUG("Triangles CCW: %f\n", CCW(T->points[0], T->points[1], T->points[2]));
			PRINTRI(T, " T ");
			failure(-4);
		}
		if(compareTriangles(T, A)){
			DEBUG("T == A in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(T, B)){
			DEBUG("T == B in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(A, B)){
			DEBUG("A == B in POE [phivos]");
			exit(0);
		}
		for(te=0; EDGE_CONTAINS(T->edges[te], points); ++te);
		legalizeEdge(points, T->edges[te], T);
		for(te=0; EDGE_CONTAINS(A->edges[te], points); ++te);
		legalizeEdge(points, A->edges[te], A);
		for(te=0; EDGE_CONTAINS(B->edges[te], points); ++te);
		legalizeEdge(points, B->edges[te], B);

		if(!validateTriangle(A) || CCW(A->points[0], A->points[1], A->points[2]) == 0){
			DEBUG("-4\nTRIANGLE A INVALID - PIT - AFTER LE");
			failure(-4);
		}
		if(!validateTriangle(B) || CCW(B->points[0], B->points[1], B->points[2]) == 0){
			DEBUG("-4\nTRIANGLE B INVALID - AFTER LE");
			failure(-4);
		}
		if(!validateTriangle(T) || CCW(T->points[0], T->points[1], T->points[2]) == 0){
			DEBUG("-4\nTRIANGLE T[j] INVALID - AFTER LE");
			PRINTRI(T, " T ");
			failure(-4);
		}
		if(compareTriangles(T, A)){
			DEBUG("-6\nT == A in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(T, B)){
			DEBUG("-6\nT == B in POE [phivos]");
			exit(0);
		}
		if(compareTriangles(A, B)){
			DEBUG("-6\nA == B in POE [phivos]");
			exit(0);
		}
	}
//Comment the following code to get triangulation with p-1 and p-2 inserted
/*
	points = head->next;
	//free(points->prev);
	points = points->next;
	//free(points->prev);
	//head = points;
	T = triHead;
	A = (Triangle *) 0;
	ae=0;
	while(T){
		for(te=0; te<3 && T->points[te]->id != -1 && T->points[te]->id != -2; ++te);
		if(te<3){
			tcnt--;

			for(e=0; e<3; ++e)
				T->edges[e]->t[ T->edges[e]->t[0] == T ? 0 : 1 ] = NULL;

			if(!A){
				B = triHead = T->next;
				free(T);
				T = triHead;
			}
			else{
				B = T;
				A->next = T->next;
				free(T);
				T = A->next;
			}
*/
/*
			for(e=0; EDGE_CONTAINS(B->edges[e], B->points[te]); ++e);
			C = B->edges[NEXT(e)]->t[0] == B ? B->edges[NEXT(e)]->t[1] : B->edges[NEXT(e)]->t[0];
			if(C){
				for(ae=0; EDGE_CONTAINS(C->edges[ae], B->points[te]); ++ae);
				//find unique
				points = (EDGE_CONTAINS(B->edges[e], C->edges[ae]->a) ? C->edges[ae]->b : C->edges[ae]->a);
				//find mutual
				p = (EDGE_CONTAINS(B->edges[e], C->edges[ae]->a) ? C->edges[ae]->a : C->edges[ae]->b);
				a.f =	CCW(
								(points->y.f > p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a),
								p,
								(points->y.f < p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a)
							);
				if(B->points[te]->id == -1 && a.f > 0){
					PRINTRI(C, " ");
					PRINTED(C->edges[ae]);
					PRINTPO((points->y.f > p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					PRINTPO(p);
					PRINTPO((points->y.f < p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					DDEBUG("1 Found for:%c", ' '); PRINTPO(B->points[te]);
				}
				else if(B->points[te]->id == -2 && a.f < 0){
					PRINTRI(B, " ");
					PRINTPO((points->y.f > p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					PRINTPO(p);
					PRINTPO((points->y.f < p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					DDEBUG("2 Found for:%c", ' '); PRINTPO(B->points[te]);
				}
			}
			C = B->edges[PREV(e)]->t[0] == B ? B->edges[PREV(e)]->t[1] : B->edges[PREV(e)]->t[0];
			if(C){
				for(ae=0; EDGE_CONTAINS(C->edges[ae], B->points[te]); ++ae);
				//find unique
				points = (EDGE_CONTAINS(B->edges[e], C->edges[ae]->a) ? C->edges[ae]->b : C->edges[ae]->a);
				//find mutual
				p = (EDGE_CONTAINS(B->edges[e], C->edges[ae]->a) ? C->edges[ae]->a : C->edges[ae]->b);
				a.f =	CCW(
								(points->y.f > p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a),
								p,
								(points->y.f < p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a)
							);

				if(B->points[te]->id == -1 && a.f > 0){
					PRINTRI(B, " ");
					PRINTPO((points->y.f > p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					PRINTPO(p);
					PRINTPO((points->y.f < p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					DDEBUG("3 Found for:%c", ' '); PRINTPO(B->points[te]);
				}
				else if(B->points[te]->id == -2 &&a.f < 0){
					PRINTRI(B, " ");
					PRINTPO((points->y.f > p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					PRINTPO(p);
					PRINTPO((points->y.f < p->y.f ? points : B->edges[e]->a == p ? B->edges[e]->b : B->edges[e]->a));
					DDEBUG("4 Found for:%c", ' '); PRINTPO(B->points[te]);
				}
			}
		}
*/
/*
		}else{
			A = T;
			T = T->next;
		}

	}
	//validateTriangulation();
//*/
}

void slopeQsort(Point * h, Point * t, Point * vantage){
	Point * piv = h, * temp = h, t1;
	if(h == t)
		return;
	while(temp != t->next){
		if(CCW(vantage, piv, temp) > 0 /*|| (CCW(vantage, piv, temp) == 0 && (piv->y.f < temp->y.f || piv->x.f > temp->x.f))*/){
			t1.x.f = temp->x.f;
			t1.y.f = temp->y.f;
			t1.id = temp->id;
			temp->x.f = piv->x.f;
			temp->y.f = piv->y.f;
			temp->id = piv->id;
			piv->x.f = t1.x.f;
			piv->y.f = t1.y.f;
			piv->id = t1.id;
			temp = temp->next;
		}
		else{
			temp = temp->next;
		}
	}
	slopeQsort(h, piv, vantage);
	//slopeQsort(piv->next, t, vantage);
}

bool validateTriangle(Triangle * t){
	int i, j, c, a;
	bool valid;

	if(CCW(t->points[0], t->points[1], t->points[2]) == 0){
		return false;
	}

	for(i=0; i<3; ++i){
		c=0;
		for(j=0; j<3; ++j){
			//point contained in edge
			if( EDGE_CONTAINS(t->edges[j], t->points[i])){
				c++;
			}
			//duplicated point in edge
			if(t->edges[j]->a == t->points[i] && t->edges[j]->b == t->points[i]){
				return false;
			}
		}
		//point not contained in exactly 2 edges
		if(c!=2){
			return false;
		}
	}
	c=0;
	for (i = 0; i < 3; ++i){
		if(t->edges[i]->t[0] == t || t->edges[i]->t[1] == t)
			c++;
		/*for (j = 0; j < 2; ++j){
			if(t->edges[i]->t[j] == t)
				c++;
		}*/
	}
	//triangle adjecent to 2 edges
	if(c!=3)
		return false;
	for(i=0; i<3; ++i){
		a = (t->edges[i]->t[0] == t? 1 : 0);
		//if there is no adjacent triangle continue
		if(!t->edges[i]->t[a])
			continue;
		valid = false;
		for(j=0; j<3; ++j)
			if(EDGE_CONTAINS(t->edges[i], t->edges[i]->t[a]->points[j]) && EDGE_CONTAINS(t->edges[i], t->edges[i]->t[a]->points[NEXT(j)]))
				valid = true;
		//check wether adjecent triangle has 2 same points
		if(!valid)
			return false;
	}
	return true;
}

bool validateTriangulation(void){
	int i=0, j, p, e, o=0;
	Triangle * t, * T;
	
	for(T = triHead; T; T = T->next){
		if(!validateTriangle(T)){
			return false;
		}
		Circle * c = calculateCircleCenter(T->points[0], T->points[1], T->points[2]);
		for(j=0; j<3; ++j){
			for(e=0; EDGE_CONTAINS(T->edges[e], T->points[j]); ++e);
			if(EDGE_CONTAINS(T->edges[e], T->points[j])){
				DEBUG("MAJOR ERROR!");
				return false;
			}
			t = T->edges[e]->t[T->edges[e]->t[0] == T? 1 : 0];
			if(t==NULL){
				o++;
				continue;
			}
			for(p=0; EDGE_CONTAINS(T->edges[e], t->points[p]); ++p);
			if(DISQR(t->points[p], c->center) /*distance(t->points[p], &c->center)*/ < c->r.f){
				DDEBUG("Triangle #%d\n", i);
				PRINTRI(T, "Triangle T[i]");
				PRINTRI(t, "Adjacent Triangle");
				PRINTED(T->edges[e]);
				PRINTPO(t->points[p]);
				DDEBUG("Point in circle\npoint->center: %.2f\nradius: %.2f\n", DISQR(t->points[p], c->center)/*distance(t->points[p], &c->center)*/, c->r.f);
				printf("Center: (%.2f %.2f)\n", c->center->x.f, c->center->y.f-p);
				return false;
			}
		}
		//i++;
	}
	//DDEBUG("Outer edges found: %d\n", o);
	return true;
}

void addPoint(Point * P){
	if(!head || (head->y.f < P->y.f) || (head->y.f == P->y.f && head->x.f < P->x.f) ){
		P->next = head;
		head = P;
		low = (head->next ? low : head);
	}
	else{
		P->next = head->next;
		head->next = P;
		low = (P->y.f < low->y.f || (P->y.f == low->y.f && P->x.f > low->x.f) ? P : low);
	}
	pcnt++;
}

void initializePoints(void){

	Point * temp = head->next->next, * point = head->next;

	while(temp){
		if(CCW(head, point, temp) > 0){
			point = temp;
		}
		temp = temp->next;
	}

	temp = (Point*) malloc(sizeof(Point));
	temp->id = -1;
	temp->y.f = low->y.f - 2 * ABS(head->y.f - low->y.f) - 200;
	temp->x.f = (head->y.f - point->y.f) / (head->x.f - (point->x.f + 10));
	temp->x.f = (!VERTICAL(head, point)? ((temp->y.f - head->y.f + temp->x.f * head->x.f) / temp->x.f) : head->x.f) + 200;

	temp->next = head;
	head = temp;

	point = head->next;
	temp = point->next;


	while(temp){
		if(CCW(head, point, temp) > 0){
			point = temp;
		}
		temp = temp->next;
	}
	temp = (Point*) malloc(sizeof(Point));
	temp->id = -2;
	temp->y.f = head->next->y.f + 2 * ABS(head->next->y.f - low->y.f) + 200;
	temp->x.f = (head->y.f - point->y.f) / (head->x.f - (point->x.f - 10));

	temp->x.f = (!VERTICAL(head, point)? ((temp->y.f - head->y.f + temp->x.f * head->x.f) / temp->x.f) : head->x.f) - 200;
	temp->next = head;
	head = temp;
	Triangle * tri = (Triangle*) malloc(sizeof(Triangle));
	tri->points[0] = head;
	tri->points[1] = head->next;
	tri->points[2] = head->next->next;
	int i;
	for(i=0; i<3; ++i)
		tri->edges[i] = (Edge*) malloc(sizeof(Edge));
	for (i = 0; i < 3; ++i){
		tri->edges[i]->a = tri->points[i];
		tri->edges[i]->b = tri->points[NEXT(i)];
	}
	temp = head->next->next->next;
	while(temp){
		if(pointInTriangle(tri, temp) != -1){
			failure(-1);
		}
		temp = temp->next;
	}
}

void legalTriangulation(){
	Triangle * T = triHead;
	int p, e;
	while(T){
		for(p=0; p<3; ++p){
			for(e=0; EDGE_CONTAINS(T->edges[e], T->points[p]); ++p);
			legalizeEdge(T->points[p],T->edges[e], T);
		}
		T = T->next;
	}
}

bool compareTriangles(Triangle * A, Triangle * B)
{
	int i, j, a=0;
	for(i=0; i<3; ++i)
	{
		for(j=0; j<3; ++j)
		{
			if(A->points[i] == B->points[j])
			{
				if(++a > 2)
				{
					return true;
				}
				break;
			}
		}
	}
	return false;
}

void failure(int code){
	Point * temp = head;
	printf("%d\nlow: ", code); PRINTPO(low);
	while(temp){
		printf("%d %f %f\n", temp->id, temp->x.f, temp->y.f);
		temp = temp->next;
	}
	exit(0);
}
