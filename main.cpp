#include <gtk/gtk.h>
#include <iostream>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#define WIDTH 450
#define HEIGHT 400
#define WW 400
#define HH 50
#define RR 40
#define the_cross_has_been_planted cross=1 // поставлен крестик
#define the_zero_has_been_planted cross=0 // поставлен нолик в ответ

static const int INFINITY = 1000000; // максимальное значение статической оценочной функции
GError *error;
GdkGC *gc;
GdkDrawable *w;
GdkColor bclr;
GtkWidget *mbm,*mkp,*mm1,*mm2,*mmenu1,*mmenu2,*mst,*mfin, *mwin, *mgo;
GdkPixbuf *bm,*kp,*m1,*m2,*menu1,*menu2,*st,*fin, *win,*go;
int game = 0,x_b = 10, y_b = 10, x_m = 10, y_m = 10, step = 1,cross = 0;
double x_mouse, y_mouse;


struct position // Вершины дерева
{
	int board[10][10];
	int player;
};
struct Move // Координаты перемещения
{
	int x;
	int y;
};

int MinMove (position* p, int depth, int alpha, int beta);
int MaxMove (position* p, int depth, int alpha, int beta);

// генерируем все возможные ходы для текущей позиции, 
int generate_moves (position *p, std::list< Move > &move_list) 
{ 
	int n = 0;
	for( int i = 0; i < 10; i++ ) 
		for ( int j = 0; j < 10; j++ ){
		if( p->board[i][j] == 0 ) 
			{
				Move temp;
				temp.x = i;
				temp.y = j;
				move_list.push_back(temp);
				n++;
			}
		}
return n;
}
// проверяем кто выиграл
int check_win( int (*a)[ 10 ] ) 
	{
	int game = -1;
	for ( int i = 0; i < 6; i++ )
		for ( int j = 0; j < 10; j++ ) {
		if (( a[i][j] == 1 && a[i][j] == a[i + 1][j] && a[i + 1][j] == a[i + 2][j] && a[i + 2][j] == a[i + 3][j] && a[i + 3][j] == a[i + 4][j]))
			game = 1;
		if ( a[i][j] == 2 && a[i + 1][j] == 2 && a[i + 2][j] == 2 && a[i + 3][j] == 2 && a[i + 4][j] == 2)
			game = 2;
			}

	for ( int i = 0; i < 10; i++ )
		for ( int j = 0; j < 6; j++ ) {
		if ( a[i][j] == 1 && a[i][j + 1] == 1 && a[i][j + 2] == 1 && a[i][j + 3] == 1 && a[i][j + 4] == 1 )
			game = 1;
		if ( a[i][j] == 2 && a[i][j + 1] == 2 && a[i][j + 2] == 2 && a[i][j + 3] == 2 && a[i][j + 4] == 2)
			game = 2;
			}

	for ( int i = 0; i < 6; i++ )
		for ( int j = 0; j < 6; j++ ) {
			if (( a[j][i] == 1 && a[j + 1][i + 1] == 1 && a[j + 2][i + 2] == 1 && a[j + 3][i + 3] == 1 && a[j + 4][i + 4] == 1 ) || 
			( a[j][i + 4] == 1 && a[j + 1][i + 3] == 1 && a[j + 2][i + 2] == 1 && a[j + 3][i + 1] == 1 && a[j + 4][i] == 1))
			game = 1;
			if (( a[j][i] == 2 && a[j + 1][i + 1] == 2 && a[j + 2][i + 2] == 2 && a[j + 3][i + 3] == 2 && a[j + 4][i + 4] == 2 ) || 
			( a[j][i + 4] == 2 && a[j + 1][i + 3] == 2 && a[j + 2][i + 2] == 2 && a[j + 3][i + 1] == 2 && a[j + 4][i] == 2))
			game = 2;
			}
		return game;
	} 
// находит максимум из двух величин
int max(int a, int b)
{
	if (a > b) return a;
	else return b;
}
// находит максимум из двух величин
int min(int a, int b)
{
	if (a < b) return a;
	else return b;
}
// делаем ход в m крестиком или ноликом
void domove(Move *m, position *p)
{
if (p->player == 1)
	p->board[m->x][m->y] = 1;
if (p->player == 2)
	p->board[m->x][m->y] = 2;
}


// отменяем ход который делали в m
void undomove(Move *m, position *p)
{
	p->board[m->x][m->y] = 0;
}
// оцениваем текущую позицию
int evaluate_position(int (*a)[ 10 ])
	{
	int evaluation = 0;
	int eval_x = 0;
	int eval_o = 0;
	int game = check_win(a);
		if (game == 1) evaluation = -INFINITY;
		if (game == 2) evaluation = INFINITY;
		//если никто не выиграл
		if (game == -1) 
		{
		//свободные ходы по горизонтали
		for (int t = 0; t < 10; t++) {
			for (int i = 0; i < 6; i++) {
			if (( a[i][t] == 1 || a[i][t] == 0 ) && ( a[i + 1][t] == 1 || a[i + 1][t] == 0 ) && 
				( a[i + 2][t] == 1 || a[i + 2][t] == 0 ) && ( a[i + 3][t] == 1 || a[i + 3][t] == 0 ) && 
				( a[i + 4][t] == 1 || a[i + 4][t] == 0 )) eval_x++;

			if (( a[i][t] == 2 || a[i][t] == 0 ) && ( a[i + 1][t] == 2 || a[i + 1][t] == 0 ) && 
				( a[i + 2][t] == 2 || a[i + 2][t] == 0 ) && ( a[i + 3][t] == 2 || a[i + 3][t] == 0 ) && 
				( a[i + 4][t] == 2 || a[i + 4][t] == 0 )) eval_o++;
			if (( a[i][t] == 2 || a[i][t] == 0 ) && ( a[i + 1][t] == 2 ) && ( a[i + 2][t] == 2 ) && ( a[i + 3][t] == 2 ) && ( a[i + 4][t] == 2 || a[i + 4][t] == 0 )) eval_o += 1000;
			if (( a[i][t] == 1 || a[i][t] == 0 ) && ( a[i + 1][t] == 1 ) && ( a[i + 2][t] == 1 ) && ( a[i + 3][t] == 1 ) && ( a[i + 4][t] == 1 || a[i + 4][t] == 0 )) eval_x += 1000;	
				}
			}
		////свободные ходы по вертикали
		for (int t = 0; t < 10; t++) {
			for (int j = 0; j < 6; j++) {
			if (( a[t][j] == 1 || a[t][j] == 0 ) && ( a[t][j + 1] == 1 || a[t][j + 1] == 0 ) && 
				( a[t][j + 2] == 1 || a[t][j + 2] == 0 ) && ( a[t][j + 3] == 1 || a[t][j + 3] == 0 ) && 
				( a[t][j + 4] == 1 || a[t][j + 4] == 0 )) eval_x++;
			if (( a[t][j] == 2 || a[t][j] == 0 ) && ( a[t][j + 1] == 2 || a[t][j + 1] == 0 ) && 
				( a[t][j + 2] == 2 || a[t][j + 2] == 0 ) && ( a[t][j + 3] == 2 || a[t][j + 3] == 0 ) && 
				( a[t][j + 4] == 2 || a[t][j + 4] == 0 )) eval_o++;
			if (( a[t][j] == 2 || a[t][j] == 0 ) && ( a[t][j + 1] == 2 ) && ( a[t][j + 2] == 2 ) && ( a[t][j + 3] == 2 ) && ( a[t][j + 4] == 2 || a[t][j + 4] == 0 )) eval_o += 1000;
			if (( a[t][j] == 1 || a[t][j] == 0 ) && ( a[t][j + 1] == 1 ) && ( a[t][j + 2] == 1 ) && ( a[t][j + 3] == 1 ) && ( a[t][j + 4] == 1 || a[t][j + 4] == 0 )) eval_x += 1000;	
				}
			}
		////свободные ходы по диагонали
		for (int t = 0; t < 6; t++) {
			for (int j = 0; j < 6; j++) {
			if (( a[j][t] == 1 || a[j][t] == 0 ) && ( a[j + 1][t + 1] == 1 || a[j + 1][t + 1] == 0 ) && 
				( a[j + 2][t + 2] == 1 || a[j + 2][t + 2] == 0 ) && ( a[j + 3][t + 3] == 1 || a[j + 3][t + 3] == 0 ) && 
				( a[j + 4][t + 4] == 1 || a[j + 4][t + 4] == 0 )) eval_x++;

			if (( a[j][t + 4] == 1 || a[j][t + 4] == 0 ) && ( a[j + 1][t + 3] == 1 || a[j + 1][t + 3] == 0 ) && 
				( a[j + 2][t + 2] == 1 || a[j + 2][t + 2] == 0 ) && ( a[j + 3][t + 1] == 1 || a[j + 3][t + 1] == 0 ) && 
				( a[j + 4][t] == 1 || a[j + 4][t] == 0 )) eval_x++;

			if (( a[j][t] == 2 || a[j][t] == 0 ) && ( a[j + 1][t + 1] == 2 || a[j + 1][t + 1] == 0 ) && 
				( a[j + 2][t + 2] == 2 || a[j + 2][t + 2] == 0 ) && ( a[j + 3][t + 3] == 2 || a[j + 3][t + 3] == 0 ) && 
				( a[j + 4][t + 4] == 2 || a[j + 4][t + 4] == 0 )) eval_o++;

			if (( a[j][t + 4] == 2 || a[j][t + 4] == 0 ) && ( a[j + 1][t + 3] == 2 || a[j + 1][t + 3] == 0 ) && 
				( a[j + 2][t + 2] == 2 || a[j + 2][t + 2] == 0 ) && ( a[j + 3][t + 1] == 2 || a[j + 3][t + 1] == 0 ) && 
				( a[j + 4][t] == 2 || a[j + 4][t] == 0 )) eval_o++;
			
			if (( a[j][t] == 2 || a[j][t] == 0 ) && ( a[j + 1][t + 1] == 2 ) && ( a[j + 2][t + 2] == 2 ) && ( a[j + 3][t + 3] == 2 ) && 
				( a[j + 4][t + 4] == 2 || a[j + 4][t + 4] == 0 )) eval_o += 1000;

			if (( a[j][t + 4] == 2 || a[j][t + 4] == 0 ) && ( a[j + 1][t + 3] == 2 ) && ( a[j + 2][t + 2] == 2 ) && ( a[j + 3][t + 1] == 2 ) && 
				( a[j + 4][t] == 2 || a[j + 4][t] == 0 )) eval_o += 1000;
				
			if (( a[j][t] == 1 || a[j][t] == 0 ) && ( a[j + 1][t + 1] == 1 ) && ( a[j + 2][t + 2] == 1 ) && ( a[j + 3][t + 3] == 1 ) && 
				( a[j + 4][t + 4] == 1 || a[j + 4][t + 4] == 0 )) eval_x += 1000;

			if (( a[j][t + 4] == 1 || a[j][t + 4] == 0 ) && ( a[j + 1][t + 3] == 1 ) && ( a[j + 2][t + 2] == 1 ) && ( a[j + 3][t + 1] == 1 ) && 
				( a[j + 4][t] == 1 || a[j + 4][t] == 0 )) eval_x += 1000;

				}
			}
		evaluation = eval_o - eval_x;
		}
		return evaluation;
	}  

// собственно альфа-бета процедура, возвращающая лучший ход нолика
Move AlphaBeta (position* p, int depth, int alpha, int beta)
{
		std::list < Move > Move_list;	
		Move step;
		int bestvalue = -INFINITY;
		int value = -INFINITY;

		p->player = 2; // ходит нолик
		generate_moves(p,Move_list); // генерируем все возможные ответы нолика
		while ( !Move_list.empty() ) // пока список возможных ответов нолика не пуст
			{						 // строим дерево игры
			p->player = 2;
			domove(&Move_list.front(),p); // ходим
			value = MinMove(p, depth-1, alpha, beta); // оцениваем

			if (value >= bestvalue) { // ищем лучший ход из возможных
				bestvalue = value;
				alpha = bestvalue; // определяем значение альфа
				step = Move_list.front(); // если текущий ход лучше предыдущего, сохраняем ход
				}

			undomove(&Move_list.front(),p); // отменяем сделанный ход
			Move_list.pop_front(); // освобождаем список ходов от проверенного
			}
		return step;
	}
int MinMove (position* p, int depth, int alpha, int beta)
	{
	int game = check_win(p->board); // проверяем не выйгрышна ли для кого-нибудь позиция
	if (game == 1) return -INFINITY;
	else if (game == 2) return INFINITY;
	
	if (depth == 0)  // если достигли заданной глубины, оцениваем терминальные вершины
		return evaluate_position(p->board);

	int bestvalue = INFINITY;
	std::list < Move > Move_list;	

	p->player = 1; // играет крестик
	generate_moves(p, Move_list); // генерирум все возможные ходы крестика

	while ( !Move_list.empty() ) // пока список не пуст, ищем лучший ход - минимальную оценку позиции
		{
			p->player = 1; // крестик
			domove(&Move_list.front(),p); // делаем ход
			int value = MaxMove(p, depth-1, alpha, beta); // оцениваем
			
			if ( value <= bestvalue ) { // ищем минимальную
				beta = value; // определяем значение бета
				bestvalue = value;
			}
			undomove(&Move_list.front(),p); // отменяем сделанный ход
			Move_list.pop_front(); // убираем его из списка возможных ходов

			if ( beta < alpha ) { // если лучший ход меньше чем родитель - альфа
				// то прерываем поиск
			//std::cout << "beta pruning" << std::endl;
			break;
				return bestvalue;
				}
		}
	return bestvalue;
	}

int MaxMove (position* p, int depth, int alpha, int beta)
	{
	int game = check_win(p->board); // проверяем не выигрышна ли комбинация

	if (game == 1) return -INFINITY;
	else if (game == 2) return INFINITY;

	if(depth == 0) 
		return evaluate_position(p->board);

	int bestvalue = -INFINITY;
	std::list < Move > Move_list;	

	p->player = 2;
	generate_moves(p, Move_list); // генерируем все возможные ходы нолика

	while( !Move_list.empty() ) // пока список не пуст
		{
			p->player = 2; // ходит нолик
			domove(&Move_list.front(),p); // делаем ход
			int value = MinMove(p, depth-1, alpha, beta); // оцениваем позицию

			if ( value >= bestvalue ) { // ищем наилучший ход
				bestvalue = value;
				alpha = bestvalue;
				}	

			undomove(&Move_list.front(), p); // отменяем сделанный ход
			Move_list.pop_front(); // удаляем ход из списка

			if ( alpha > beta ) { // если оценка больше чем наилучшая оценка родителя
			//std::cout << "alpha pruning" << std::endl;
			break;
			return bestvalue; // прерываем перебор
				}
			}
	return bestvalue;
}
/*Мартица начального состояния*/
int mtr[10][10] = {	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0 };

/*Начальная матрица*/
int stdd[10][10] = {	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
					0,	0,	0,	0,	0,	0,	0,	0,	0,	0 };

gboolean destroy(GtkWidget *widget);
gint delete_event(GtkWidget *widget);
gboolean gdk_draw_picture(GdkPixbuf*,gint,gint,int,int); // вывести изображение на экран
gboolean creat_state(GtkWidget *widget, GdkEventExpose *event, gpointer data); // создать состояние
gboolean on_click(GtkWidget *Window, GdkEventButton *event, gpointer data); // обработка нажатия кнопок на мышке
gboolean creatboomstate(); // отображает состояние на экран

gboolean destroy(GtkWidget *widget)
{
	return FALSE;
}

gint delete_event(GtkWidget *widget)
{
	gtk_main_quit();
	return 1;
}

gboolean mousemove(GtkWidget *w, GdkEvent *e,gpointer data)
{
	double a,b;
	
	gdk_event_get_coords(e,&a,&b);
	x_mouse=a;
	y_mouse=b;
	
	return FALSE;
}

int main(int arge, char **argv)
{
	setlocale(LC_ALL,"Russian");
	error = NULL;
	gtk_init(&arge,&argv);

	mm1 = gtk_image_new_from_file("state//m1.jpg");
	m1 = gtk_image_get_pixbuf(GTK_IMAGE(mm1));

	mwin = gtk_image_new_from_file("state//win.jpg");
	win = gtk_image_get_pixbuf(GTK_IMAGE(mwin));

	mgo = gtk_image_new_from_file("state//go.jpg");
	go = gtk_image_get_pixbuf(GTK_IMAGE(mgo));

	mbm = gtk_image_new_from_file("state//bm.png");
	bm = gtk_image_get_pixbuf(GTK_IMAGE(mbm));

	mkp = gtk_image_new_from_file("state//kp.jpg");
	kp = gtk_image_get_pixbuf(GTK_IMAGE(mkp));

	mm2 = gtk_image_new_from_file("state//m2.png");
	m2 = gtk_image_get_pixbuf(GTK_IMAGE(mm2));

	mmenu1 = gtk_image_new_from_file("state//menu1.png");
	menu1 = gtk_image_get_pixbuf(GTK_IMAGE(mmenu1));

	mmenu2 = gtk_image_new_from_file("state//menu2.png");
	menu2 = gtk_image_get_pixbuf(GTK_IMAGE(mmenu2));

	mst = gtk_image_new_from_file("state//St.jpg");
	st = gtk_image_get_pixbuf(GTK_IMAGE(mst));

	mfin = gtk_image_new_from_file("state//fin.png");
	fin = gtk_image_get_pixbuf(GTK_IMAGE(mfin));

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,WIDTH,HEIGHT);
	gtk_widget_add_events(window,GDK_POINTER_MOTION_MASK);
	gtk_widget_add_events(window,GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events(window,GDK_POINTER_MOTION_MASK);
	g_signal_connect(G_OBJECT(window),"motion-notify-event",G_CALLBACK(mousemove),NULL);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(destroy),NULL);
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(delete_event),NULL);
	g_signal_connect(G_OBJECT(window),"button-press-event",G_CALLBACK(on_click),NULL);
	gdk_color_parse("gray",&bclr);
	gtk_widget_modify_bg(window,GTK_STATE_NORMAL,&bclr);
	gdk_color_parse("black",&bclr);
	g_signal_connect(G_OBJECT(window),"expose_event",G_CALLBACK(creat_state),NULL);
	
	gtk_widget_show_all(window);
	gtk_main();

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++)
			std::cout << stdd[j][i];
		std::cout << "\n";
		}
	std::cout << evaluate_position(mtr);

	return 0;
}

gboolean gdk_draw_picture(GdkPixbuf *picture,gint x,gint y,gint wi,gint hh)
{
	gdk_draw_pixbuf(w,gc,picture,0,0,x,y,wi,hh,GDK_RGB_DITHER_NONE,0,0);
	return TRUE;
}

gboolean creat_state(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	w = widget->window;
	gc = widget->style->fg_gc[GTK_WIDGET_STATE(widget)];

	gdk_draw_picture(m1,0,0,WW,WW);
	gdk_draw_picture(menu2,WW,0,HH,HH);
	gdk_draw_picture(menu1,WW,HH,HH,HH);
	gdk_draw_picture(m2,WW,HEIGHT-42,HH,42);	

	return TRUE;
}

// обрабатываем нажатие на мышку и создаем ответный ход компьютера
gboolean on_click(GtkWidget *Window, GdkEventButton *event, gpointer data)
{		
		x_mouse = event->x;
		y_mouse = event->y;

		for(int i=0; i<RR; i++)
		{
			for(int j=0; j<RR; j++)
			{
				if(x_mouse > j*RR && x_mouse < (j+1)*RR && x_mouse < (j+1)*RR && y_mouse > i*RR && y_mouse < (i+1)*RR && x_mouse < HEIGHT)
				{
					x_m = j; y_m = i;
				}
			}
		}
	if(x_mouse > HEIGHT && y_mouse > HH && y_mouse < HH*2)
		{
			int depth = 1;
			for(int i=0; i < 10; i++)
			{
				for(int j=0; j < 10; j++)
				{
					mtr[i][j] = stdd[i][j];
				}
			}
			creatboomstate();
		}
	if(event->button == 1 && x_mouse < HEIGHT)
		{
			x_b = x_m;
			y_b = y_m;
			the_cross_has_been_planted;
				mtr[x_b][y_b] = 1;
				stdd[x_b][y_b] = 1;
				gdk_draw_picture(kp,x_b*RR,y_b*RR,RR,RR);
				// Здесь используется альфа-бета процедура, которая возвращает лучший ход
					{
					position p;
					Move step;
					int depth = 2;
					for (int i = 0; i < 10; i++)
						for (int j = 0; j < 10; j++)
						p.board[j][i] = mtr[j][i];
					
					p.player = 0;
					int alpha = -INFINITY;
					int beta = INFINITY;
					step = AlphaBeta (&p, depth, alpha, beta);

					for (int i = 0; i < 10; i++)
						for (int j = 0; j < 10; j++)
						 mtr[j][i] = p.board[j][i];
					
					mtr[step.x][step.y]=2;
					stdd[step.x][step.y]=2;
					
					gdk_draw_picture(st,step.x*RR,step.y*RR,RR,RR);
					if (check_win(mtr) == 1) gdk_draw_picture( win,0,0,WW,HEIGHT );
					if (check_win(mtr) == 2) gdk_draw_picture( go,0,0,WW,HEIGHT );
					}

				the_zero_has_been_planted;
		}

	if(x_mouse > HEIGHT && y_mouse > 0 && y_mouse < HH)
	{
		for(int i=0; i < 10; i++)
		{
			for(int j=0; j < 10; j++)
			{
				mtr[i][j] = stdd[i][j];
			}
		}
		creatboomstate();
	}

	
	
	if(x_mouse > HEIGHT && y_mouse > 2*HH && y_mouse < HH*3)
	{
		gtk_main_quit();
	}

return TRUE;
}

gboolean creatboomstate()

{
	int i,j;
	gdk_draw_rectangle(w,gc,TRUE,0,0,WW,HEIGHT);
	for(int i=0; i < 10; i++)
		{
			for(int j=0; j < 10; j++)
			{
				mtr[i][j]=stdd[i][j];
			}
		}

	for(j=0; j < 10; j++)
	{
		for(i=0; i < 10; i++)
		{
			if(stdd[i][j] == 1) gdk_draw_picture(kp,i*RR,j*RR,RR,RR);
			if(stdd[i][j] == 2) gdk_draw_picture(st,i*RR,j*RR,RR,RR);
			if(stdd[i][j] == 0) gdk_draw_picture(fin,i*RR,j*RR,RR,RR);
			if(stdd[i][j] == 9) 
			{
				gdk_draw_picture(bm,i*RR,j*RR,RR,RR);
				x_m = i;
				y_m = j;
			}
		}
	}
	return TRUE;
}