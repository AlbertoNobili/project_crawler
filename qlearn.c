#include <math.h>
#include <stdlib.h>

// Global constants
#define MAXSTA	100		// max # of states
#define MAXACT	100		// max # of actions
#define ALPHA0	1.0		// default learning rate
#define EPSINI	0.9 	// initial exploration factor
#define EPSFIN	0.1 	// final exploration factor
#define GAMMA0	0.9 	// default discount factor
#define DECAY0	0.95	// default epsilon decay rate

// QL matrices
static int T[MAXSTA][MAXACT];	// transition matrix	INUTILE
static int R[MAXSTA][MAXACT];	// reward matrix		INUTILE
static int Q[MAXSTA][MAXACT];	// Q matric

// Global variables
static int nsta;		// actual # of states
static int nact;		// actual number of actions
static int goalstate;	// store the goal state			INUTILE
static float alpha;		// learning rate
static float gam;		// discount factor
static float epsilon; 	// actual exploration probability
static float decay;		// decay rate for epsilon
static float eps_norm;	// normalized exploration probability
static float eps_ini;	// initial exploration probability
static float eps_fin;	// final exploration probability


// Auxiliary functions
float frand(float xmin, float xmax)
{
float range;
	range = (xmax - xmin);
	return (xmin + range*(float)rand()/RAND_MAX);
}

// Initialization
void ql_init(int ns, int na)
{
int s, a;
	
	nsta = ns;
	nact = na;

	if(nsta > MAXSTA){
		printf("NUmber of states too big\n");
		exit(1);
	}
	if(nact > MAXACT){
		printf("Number of actions too big\n");
		exit(1);
	}

	alpha = ALPHA0;
	gam = GAMMA0;
	eps_ini = EPSINI;
	eps_fin = EPSFIN;
	decay = DECAY0;

	for (s=0; s<nsta; s++)
		for(a=0;a<nact;a++)
			Q[s][a] = 0;
}

// Reduce exploration
static float esp_norm = 1.0;
void ql_reduce_exploration()
{
	eps_norm = decay*eps_norm;
	epsilon = eps_fin + eps_norm*(eps_ini - eps_fin);
}

// Maximum Q value in a given state s
float ql_maxQ(int s)
{
int a;
float m;

	m = Q[s][0];	// initialized with Q value for action 0
	for (a=1; a<nact; a++)
		if (Q[s][a] > m)
			m = Q[s][a];
	return m;
}

// Best action in a given state s
float ql_best_action(int s)
{
int a, ba;
float m;

	m = Q[s][0];	// initialized with Q value for action 0
	ba = 0;			// initialized best action with action 0

	for (a=1; a<nact; a++)
		if (Q[s][a] > m){
			m = Q[s][a];
			ba = a;
		}
	return ba;
}

// Epsilon-greedy policy in a given state s
int  ql_greedy_policy (int s)
{
int ra, ba;
float x;

	ba = ql_best_action(s);
	ra = rand()%nact;
	x = frand(0, 1);
	if (x < epsilon) return ra;
	else return ba;
}

// Update Q value DA METTERE IL VALORE DI RITORNO IN FLOAT
int  ql_updateQ(int s, int a, int r, int snew)
{
float Qtarget, TDerr;

	Qtarget = r + gam*ql_maxQ(snew);
	TDerr = Qtarget - Q[s][a];
	Q[s][a] = Q[s][a] + alpha*TDerr;
	return fabs(TDerr);
}
