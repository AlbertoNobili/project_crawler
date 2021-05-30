#include <allegro.h>
#include <math.h>
#include <stdio.h>

// Nota: 1 cm = 4 pixel
#define XWIN	1200
#define YWIN	728
#define XBIT	450
#define YBIT	648
#define BLK		0
#define WHITE	15
#define RED		12
#define GREEN	10
#define DGREEN  2
#define BLUE	9
#define CYAN	11
#define GREY	8
#define CARTH	160     // cart height [px]
#define CARTL	400     // cart length [px]
#define CARTB	680     // cart bottom y coord [px]
#define HARDB   688     // hard bottom y coord [px]
#define CARTT	620     // cart top y coord [px]
#define CARTL   200     // cart left x coord [px]
#define CARTR   600     // cart right x coord [px]
#define TOP     80     
#define JOINTH  540     // joint1 y coord in bitmap [px]
#define RADIUS	5		// joints' radius [px]
#define L1  	200		// link1 length [px]
#define L2  	200		// link2 length [px]


extern int view;
extern int stop;

typedef struct{
    float x, y;     // current end effector position [m, m]
    float ox, oy;   // old end effector position [m, m]
    float z;        // robot position [m]
    float dz;       // position increment [m]
    float th1;      // angle of link 1 [rad]
    float th2;      // angle of link 2 [rad]
    float space;    // space covered [m]
} state;

void display_links(state s)
{
int x1, x2, y1, y2, x3, y3;
BITMAP* buf;

	//usiamo la bitmap come buffer
	buf = create_bitmap(XBIT, YBIT);
	//cancelliamo i link precedenti
	clear_to_color(buf, BLK);
    //draw bottom
	rectfill(buf, 0, CARTB-80, XBIT, HARDB-80, GREEN);
    //draw hard bottom
    rectfill(buf, 0, HARDB-80, XBIT, YBIT, DGREEN);
	//draw links and joints
    x1 = 0;
    y1 = JOINTH;
    x2 = x1 + L1*cos(s.th1);
    y2 = y1 + L1*sin(s.th2);
    x3 = x2 + L2*cos(s.th1+s.th2);
    y3 = y2 + L2*sin(s.th1+s.th2);
    circlefill(buf, x1, y1, RADIUS, CYAN);
    circlefill(buf, x2, y2, RADIUS, CYAN);
    line(buf, x1, y1, x2, y2, GREY);
    line(buf, x2, y2, x3, y3, GREY);
	//ricopiamo la bitmap
	blit(buf, screen, 0, 0, CARTR, TOP, buf->w, buf->h);

	return;
}

void update_info(long epoch, float space)
{
char s[50];
	sprintf(s, "Epoch = %ld  Space = %f", epoch, space);
	textout_centre_ex(screen, font, s, XWIN/2, 10, WHITE, BLK);
}

void init_graphics(state s)
{

	allegro_init();
	set_color_depth(8);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
	clear_to_color(screen, BLK);
	install_keyboard();
	
	srand(time(NULL));

	//draw bottom
	rectfill(screen, 0, CARTB, XWIN, HARDB, GREEN);
    //draw hard bottom
    rectfill(screen, 0, HARDB, XWIN, YWIN, DGREEN);
    //draw robot
    rectfill(screen, CARTL, CARTT, CARTR, CARTB, BLUE);
    circlefill(screen, CARTR, CARTT, RADIUS, CYAN);
    display_links(s);

	return;
}

void terminate_graphics(){
	allegro_exit();
}

char get_scancode_nb ()
{
char c;

	if (keypressed()){
		c = readkey();
		printf("carattere premuto:%c\n", c);
		return c;
	}else 
		return 0;
}

void read_key()
{
char c;
	c = get_scancode_nb();
	if (c == 'v')
		view = (view==1)? 0:1;
	else if (c == 'q')
		stop = 1;
}
