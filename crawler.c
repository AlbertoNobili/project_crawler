#include <stdlib.h> 

// Global definitions
#define L1  0.5     // link1 length
#define L2  0.5     // link2 length
#define RH  0.4     // robot height
#define RL  1       // robot length

// Global variables
static float t1min, t2min;
static float t1max, t2max;
static float dt1, dt2;

// manca una funzione che inizializzi le variabili globali

// State variables
typedef struct{
    float x, y;     // current end effector position
    float ox, oy;   // old end effector position
    float z;        // robot position
    float dz;       // position increment
    float th1;      // angle of link 1
    float th2;      // angle of link 2
    float space;    // space covered
} state;
static state rob;

void compute_end_point()
{
float t1, t2, t12;

    t1 = rob.th1;
    t2 = rob.th2; 
    t12 = rob.th1  + rob.th2;
    rob.ox = rob.x;
    rob.oy = rob.y;
    rob.x = rob.z + L1*cos(t1) + L2*cos(t12);
    rob.y = RH + L1*sin(t1) + L2*sin(t12);
}

// Angles to state
int ang2state(float t1, float t2)
{
int i, j, n2;

    i = (t1 - t1min)/dt1;
    j = (t2 - t2min)/dt2;
    n2 = (t2max - t2min)/dt2 + 1;
    return i*n2 + j;
}

// Action to angles
void action2angles(int a)
{
    switch(a){
        case TH1UP: rob.th1 += DTH1; break;
        case TH1DW: rob.th1 -= DTH1; break;
        case TH2UP: rob.th2 += DTH2; break;
        case TH2DW: rob.th2 -= DTH2; break;
        default: break;
    }
    if (rob.th1 > TH1MAX) rob.th1 = TH1MAX;
    if (rob.th1 < TH1MIN) rob.th1 = TH1MIN;
    if (rob.th2 > TH2MAX) rob.th2 = TH2MAX;
    if (rob.th2 < TH2MIN) rob.th2 = TH2MIN;
}

// Next state
int next_state(int a)
{
int s;

    action2angles(a);
    compute_end_point();
    rob.dz = 0;
    if ((rob.y <= 0) || (rob.oy <= 0)){
        rob.dz = rob.ox - rob.x;
        rob.space += rob.dz;
    }
    s = ang2state(rob.th1, rob.th2);
    return s;
}

// Reward
#define HARDL   -20     // hard level coordinate
#define RHIT    -4      // when hitting angle limit
#define RLOW    -10     // when going too low level
#define RMOVE   -1      // for each move

int get_reward(int s, int snew)
{
int r;  
    if (snew == s) r = RHIT;        // hit the limit angle
    else r = rob.dz;                // proportional to progress
    if (rob.y < YC) r = RLOW;       // going too low CAMBIARE YC -> HARDL
    r += MOVE;                      // for each move
    return r;           
}

// Learning loop 
float learn(int s0)
{
int s, a, r, snew, steps = 0;
float err = 0;
    s = s0;
    while (!end){
        steps++;
        a = ql_egreedy_policy(s);
        snew = next_state(a);
        r = get_reward(s, snew);
        err += ql_updateQ(s, a, r, snew);
        s = snew;
        if (step%100 == 0) ql_reduce_exploration();
        interpreter();
    }
    return err/steps;
}