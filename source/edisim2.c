/*------------------------------------------------------------
   EDISIM2.c
   "Editor Simulador de Robots Manipuladores version 2.0"
   Proyecto de TESIS de Licenciatura
   Héctor Morales Piloni
   FCC BUAP 1999 - 2000.
   JalaVer@yahoo.com
  ------------------------------------------------------------*/

/*------------------------------------------------------------
	Seccion #includes
  ------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "edisim2.h"  //Aqui se encuentran definidas las piezas
#include "resource.h" //Aqui se encuentran definidos: menu, 
					  //ToolBars, StatusBars

/*------------------------------------------------------------
	Seccion #defines
  ------------------------------------------------------------*/
#define FigSize				300.0			//¿?
#define RADIAN				0.017453292520	//trivial
#define PI					3.1415926536	//trivial
#define	UP					1				//Posicion arriba
#define	RIGHT				2				//Posicion derecha
#define	UNTITLED			"Sin título"	
#define	RESX				800				//Resolucion X
#define	RESY				600				//Resolucion Y
#define	HALF_SCREEN_WIDTH	400				//trivial
#define	HALF_SCREEN_HEIGHT	300				//trivial
#define	ASPECT_RATIO		1.3333			//ResX/ResY
#define AMBIENT_LIGHT		64				//Factor de iluminacion

/*------------------------------------------------------------
	Seccion typedef
  ------------------------------------------------------------*/
typedef double matrix_4x4 [4][4];	//define una matriz 4x4

/*------------------------------------------------------------
	Variables Globales
  ------------------------------------------------------------*/
int i;							//auxiliar
int j;							//auxiliar
int	k;							//auxiliar
int	ManoAbierta=0;				//Bandera para la pinza
int	mano=0;						//auxiliar
int	pinza=0;					//auxiliar
int	Order_Robot[NumPlaTot];		//Contiene los planos del robot
int Order_Table[22];			//contiene los planos de la mesa
int Order_Cube[6];				//contiene los planos del cubo
int	OrdenDatos[50][2];			//contiene los vertices de las piezas
int ordenr[50][2];				//contiene el numero de pieza que se va agregando 
int	indice=1;					//numero de piezas utilizadas
int	borrar=0;					//bandera para borrar piezas
int	mover=0;					//bandera para mover piezas
int	numero=0;					//??
int	vpx;						//auxiliar vector posicion x
int	vpy;						//auxiliar vector posicion y
int	vpz;						//auxiliar vector posicion z
int	cc=0;						//auxiliar
int	dd=0;						//auxiliar
int	x;							//auxiliar
int	y;							//auxiliar
int	l=0;						//auxiliar
int	t=0;						//auxiliar
int	nbarra=0;					//numero de barras deslizantes	
int	barraab[50];				//contiene el estado de cada una de las barras
int	pp=0;						//auxiliar
int	iMsgType;					//Tipo de mensaje
int	iPart;						//Numero de pieza
int	ObjMenuCount=ID_MENU_OBJ+0;	//Cuenta el numero de piezas
int	ObjSelected;				//Pieza seleccionada
int cx,cy;						//Ancho y alto del area de trabajo
int PosCamX;					//Posicion de la camara en eje X
int PosCamY;					//Posicion de la camara en eje Y
int RotAng[50];					//Aqui se tienen los angulos de rotacion
								//de los brazos

double	cos_table[360+1]; //Contiene los valores del coseno de 0-360
double	sin_table[360+1]; //Contiene los valores del seno de 0-360

double OjoAObjeto;		//Distancia del ojo al objeto
double OjoAPantalla;	//Distancia del ojo a la pantalla
		
double distance;		//distancia en z del objeto
double zoom;			//distancia en z de la camara
double visual=0.0;		//Numero de unidades que se mueve la camara 
						//sobre el eje z para el zoom...
double Distance[NumPlaTot+6+22]; //aux para el ordenamiento de planos

//Aqui se almacenan los planos que pertenecen a la sombra
//tal como en Robot[i][j] se almacenan los que pertenecen 
//al robot
double SRx[NumPlaTot][4]; //Sombra del robot
double SRy[NumPlaTot][4]; 
double STx[22][4];		  //Sombra de la mesa
double STy[22][4]; 
double SCx[6][4];		  //Sombra del cubo
double SCy[6][4]; 

double srx,sry;	//sombra 3D en x y robot
double stx,sty;	//sombra 3D en x y mesa
double scx,scy; //sombra 3D en x y cubo

double ex,ey,ez; //espacio entre la mano y el cubo

matrix_4x4	V,Vaux;	//matriz de rotacion y matriz auxiliar

float intensity;	//intensidad de iluminacion

//vector original de luz: {50.0,-50.0,70.0}
struct Pto3D light={0.502518,-0.502518,0.703626}; //vector normalizado
struct Pto3D VectPos1[50];	//contiene los vectores de posicion iniciales
struct Pto3D Eje1[50];		//contiene los ejes de rotacion iniciales
struct Pto3D VectPosF[50];	//contiene los vectores de posicion finales
struct Pto3D EjeF[50];		//contene los ejes de rotacion finales
struct Pto3D FinalPos;		//Posicion del elemento terminal del robot

//Contiene los planos del robot, mesa y cubo
//todos revuletos para despues ser ordenados, para saber cual es cual
//tiene un identificador de tipo y el numero de planos de ese tipo
struct Obj Objects[NumPlaTot+6+22][4];

//Aqui se guardan los planos ordenados del robot, cubo y mesa
//tiene una variable que indica el tipo de objeto y cada uno 
//de los objetos esta ordenado por separado, se puede repetir el 
//numero de plano, pero no sucedera con el tipo de objeto.
struct Order Order[NumPlaTot+6+22];
 
char *szMsg;	//mensajes de los cuadros de dialogo

HINSTANCE	hInst;		//instancia a la ventana principal
HWND		hwndTB1;	//ToolBar1
HWND		hwndTB2;	//ToolBar2
HWND		hwndTB3;	//ToolBar3
HWND		hwndST;		//StatusBar
HWND        hwnd;		//Ventana principal
HDC			hdcBuffer;	//hdc del doble buffer
HBITMAP		hbmBuffer;	//bitmap que contiene el doble buffer

static OPENFILENAME ofn;	//estructura para el manejo de archivos
static char szAppName[] = "EDISIM 2.0";

//Para saber si se pinta alambrado o con shading
BOOL WireFrame = FALSE;
//Para saber si se pinta el cubo y la mesa
BOOL IsTableEnabled = FALSE;
//Para saber si fue tomado el cubo
BOOL IsCubeTaken = FALSE;


/*------------------------------------------------------------
	Prototipos de funciones
  ------------------------------------------------------------*/
void OpenCloseHand(int, int, int, HDC);
void RobotAssembly(int, int, int, int, int);
void borraorden(void);
void PositionVector(int, int, int, int);
void CamaraAA(int, HDC);
void PlacePart(int, int, HDC);
void CopiaDatos(int);
void creaorden(int);
void deslizar(void);
void DoCaption(HWND,char*);
void Inicializa(void);
void OkMessage(HWND, char*, char*);
void OrdenaPlanos(int,int,struct Order[],struct Obj[][4]);
void DrawPlane(int, int, HDC);
void DrawPlaneWire(int, int, HDC);
void DrawRobot(HDC);
void PonePiso(void);
void PopFileInitialize(HWND);
void qs(int,int,struct Order[]);
void RestauraVpE(void);
void RotateRobot(int, int, HDC);
void swap(int,int,struct Order[]);
void table();
void Mat_Identity_4x4(matrix_4x4);
void Persp_Point(double, double, double, long*, long*);
void RotateCamX(int);
void RotateCamY(int);
void RotateCamZ(int);
void CreaMatRot();
void RotaPtos(double*,double*,double*);
void Shading(int,struct Pto3D[][4]);
void DrawRobotSolid(int);
void DrawTableSolid(int);
void DrawCubeSolid(int);
void Demo(HDC);

int  embonar(int,int,int);

short AskAboutSave(HWND, char*);

BOOL Visible(int,struct Pto3D[][4]);
BOOL PopFileOpenDlg(HWND, PSTR, PSTR);
BOOL PopFileRead(HWND, PSTR);
BOOL PopFileSaveDlg(HWND, PSTR, PSTR);
BOOL PopFileWrite(HWND, PSTR);
BOOL TakeCube();
BOOL LetCube();
BOOL Collision(int);

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

/*------------------------------------------------------------
	WinMain()
  ------------------------------------------------------------*/
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{

    MSG         msg;
    WNDCLASSEX  wndclass;

    wndclass.cbSize        = sizeof (wndclass);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wndclass.lpszMenuName  = "MENU1";
    wndclass.lpszClassName = szAppName;
    wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);

    RegisterClassEx (&wndclass);
	
	hInst = hInstance;

    hwnd  = CreateWindow (szAppName,  // Nombre de clase de la ventana
	        "EDISIM 2.0",             // Titulo de la ventana
            WS_OVERLAPPEDWINDOW,      // Estilo de ventana
            CW_USEDEFAULT,            // posicion inicial x
			CW_USEDEFAULT,            // posicion inicial y
            CW_USEDEFAULT,            // tamaño inicial x
            CW_USEDEFAULT,            // tamaño inicial y
            NULL,                     // Manejador del padre
            NULL,                     // Manejador del menu
            hInstance,                // Instancia de programa
		    NULL);		              // Parametros de creacion
	
    ShowWindow (hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow (hwnd);

	InitCommonControls();

    while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}

/*------------------------------------------------------------
	WndProc() 
  ------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Para saber si se necesita guardar el archivo
	static BOOL		bNeedSave = FALSE;
	//Para saber si se selecciono un objeto del menu "objetos"
	static BOOL		ObjectWasSelected = FALSE;
	//Para almacenar el nombre del archivo
	static char		szFileName[_MAX_PATH];
	//Para almacenar el titulo de la ventana (ie EDISIM 2-FILENAME)
	static char		szTitleName[_MAX_FNAME + _MAX_EXT];
	//Contiene el area de dibujo
	static RECT		rcArea;
	//Para saber la posicion del mouse y su posicion anterior
	static POINT	mousePos,testPos;
	//Para su uso con los DialogBoxes
	static HINSTANCE hInstance;
	//Arreglo que contiene la coordenada final x para la
	//division del status bar
	static int SbWd[2];  

	HDC				hdc;
    PAINTSTRUCT		ps;
	HMENU			hmenu;
	RECT			r;

	int				a,b,c,i;   //Auxiliares

	char			szPos[40]; //cadena que represeta las coord. 
							   //x,y,z del elemento terminal

	//Esta estructura es necesaria para los mensajes del StatusBar
	//Contiene un ID de menu y una cadena
	typedef struct tagPOPUPSTRING
	{
		HMENU hMenu;
		UINT  uiString;
	}POPUPSTRING;
	//Un arreglo con 6 menus
	//aunque al final solo se uso uno
	//dificil explicar por que...
	static POPUPSTRING popstr[6];

	//Estas variables son necesarias para los ToolTips
	//de los botones de las ToolBars
	//LPNMHDR, LPSTR y LPTOOLTIPTEXT estan definidos en commctrl.h
	LPNMHDR pnmh;
	LPSTR   pReply;
	LPTOOLTIPTEXT lpttt;

	//Estructura de botones para el toolbar1
	//Contiene abrir, guardar, arriba, derecha, borrar, ayuda
	TBBUTTON tbb1[]=
	{
		0,ID_ARCHIVO_NUEVO,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		1,ID_ARCHIVO_ABRIR,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		2,ID_ARCHIVO_GUARDAR,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,
		3,ID_EDITOR_BORRARPIEZA,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		4,ID_EDITOR_COLOCARPIEZA_DERECHA,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		5,ID_EDITOR_COLOCARPIEZA_ARRIBA,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		6,ID_EDITOR_ALAMBRADO,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,0,
		7,ID_EDITOR_COLOCAMESA,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,0,
		0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,
		8,ID_AYUDA_CONTENIDO,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
	};

	//Estructura de botones para el toolbar2
	//Contiene las piezas
	TBBUTTON tbb2[]=
	{
		0,1,TBSTATE_ENABLED|
			TBSTATE_CHECKED,TBSTYLE_CHECKGROUP,0,0,0,0,
		1,2,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		2,3,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		3,4,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		4,5,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		5,6,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		6,7,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		7,8,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		8,9,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0,
		9,10,TBSTATE_ENABLED,TBSTYLE_CHECKGROUP,0,0,0,0
	};

	//Estructura de botones para el toolbar3	
	//Contiene botones para la camara
	TBBUTTON tbb3[]=
	{
		0,ID_VISTA_CAMARAX,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		1,ID_VISTA_CAMARAY,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		2,ID_VISTA_CAMARAZ,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		3,ID_VISTA_CAMARAXNEG,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		4,ID_VISTA_CAMARAYNEG,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		5,ID_VISTA_CAMARAZNEG,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		6,ID_VISTA_ACERCAR,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		7,ID_VISTA_ALEJAR,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		8,ID_CAMARA_DERECHA,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		9,ID_CAMARA_IZQUIERDA,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		10,ID_CAMARA_ARRIBA,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		11,ID_CAMARA_ABAJO,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		12,ID_SIMULADOR_MOVERPIEZAMAS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		13,ID_SIMULADOR_MOVERPIEZAMENOS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,
		14,ID_CAMARA_ORIGEN,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0
	};

    switch (iMsg)
    {
		case WM_CREATE:

			hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

			//Obtiene el DC de la ventana principal
			hdc = GetDC(hwnd);

			//Se crea el DC para el doble buffer
			hdcBuffer = CreateCompatibleDC(hdc);
			ReleaseDC(hwnd,hdc);

			//Crea ToolBar 1
			hwndTB1 = CreateToolbarEx(hwnd,
								   CCS_NORESIZE|
								   TBSTYLE_TOOLTIPS|
								   WS_VISIBLE|
								   WS_CHILD,							
								   1,
								   8,hInst,
								   BITMAP1,
								   tbb1,
								   11,0,0,0,0,
								   sizeof(TBBUTTON));

			//Crea ToolBar 2
			hwndTB2 = CreateToolbarEx(hwnd,
								   CCS_NORESIZE|
								   TBSTYLE_TOOLTIPS|
								   TBSTYLE_WRAPABLE|
								   WS_CLIPSIBLINGS|
								   WS_VISIBLE|
								   WS_CHILD,							
								   2,
								   10,hInst,
								   BITMAP3,
								   tbb2,
								   10,0,0,24,24,
								   sizeof(TBBUTTON));

			//Crea ToolBar 3
			hwndTB3 = CreateToolbarEx(hwnd,
								   CCS_NORESIZE|
								   TBSTYLE_TOOLTIPS|
								   TBSTYLE_WRAPABLE|
								   WS_VISIBLE|WS_CHILD,
								   3,
								   15,hInst,
								   BITMAP2,
								   tbb3,
								   15,0,0,24,24,
								   sizeof(TBBUTTON));

			//Crea StatusBar
			hwndST = CreateStatusWindow(WS_CHILD|
									    WS_VISIBLE|										
										CCS_BOTTOM|
										SBARS_SIZEGRIP,
										"Listo",
										hwnd,
										3);

			//Inicializa parametros para el manejo de archivos
			PopFileInitialize(hwnd);
			
			//Pone nombre a la ventana (ie "Sin Titulo")
			DoCaption(hwnd,UNTITLED);

			//Por Default solo se puede colocar la 1a pieza arriba
			//asi que de entrada se deshabilita la opcion derecha
			hmenu = GetMenu(hwnd);
			SendMessage(hwndTB1,TB_ENABLEBUTTON,
						ID_EDITOR_COLOCARPIEZA_DERECHA,FALSE);
			EnableMenuItem(hmenu,ID_EDITOR_COLOCARPIEZA_DERECHA,
							MF_GRAYED);
			//Tambien la opcion de borrar pieza, al principio no hay
			//piezas que borrar
			SendMessage(hwndTB1,TB_ENABLEBUTTON,
						ID_EDITOR_BORRARPIEZA,FALSE);
			EnableMenuItem(hmenu,ID_EDITOR_BORRARPIEZA,MF_GRAYED);
			//Tambien la opcion de mover piezas, ya que no hay que
			//mover al principio
			SendMessage(hwndTB3,TB_ENABLEBUTTON,
						ID_SIMULADOR_MOVERPIEZAMAS,FALSE);
			EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMAS,MF_GRAYED);
			SendMessage(hwndTB3,TB_ENABLEBUTTON,
						ID_SIMULADOR_MOVERPIEZAMENOS,FALSE);
			EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMENOS,MF_GRAYED);
			//No hay pinzas ni barras deslizantes asi que tambien
			//se deshabilita su uso al principio
			EnableMenuItem(hmenu,ID_SIMULADOR_ABRIRPINZA,MF_GRAYED);
			EnableMenuItem(hmenu,ID_SIMULADOR_CERRARPINZA,MF_GRAYED);
			EnableMenuItem(hmenu,ID_SIMULADOR_DESLIZAR,MF_GRAYED);

			DeleteObject(hmenu);

			//Inicializa variables necesarias
			Inicializa();
			return 0;
		
		case WM_INITMENU:
			//Si el menu accesado es el de Objetos,
			//marca el objeto seleccionado
			hmenu = (HMENU) wParam;

			//Si fue seleccionado un objeto se marca
			//si no, se deja el menu como esta
			if(ObjectWasSelected)
			{
				//ID_MENU_OBJ es el ID del menu objetos, 
				//ID_MENU_OBJ + n es el n-esimo elemento del menu
				for(i=ID_MENU_OBJ+1; i<=(ObjMenuCount+ID_MENU_OBJ); i++)
					CheckMenuItem(hmenu,i,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_MENU_OBJ+ObjSelected,MF_CHECKED);
				ObjectWasSelected = FALSE;
			}

			if(WireFrame)
				CheckMenuItem(hmenu,ID_EDITOR_ALAMBRADO,MF_CHECKED);
			else
				CheckMenuItem(hmenu,ID_EDITOR_ALAMBRADO,MF_UNCHECKED);

			if(IsTableEnabled)
				CheckMenuItem(hmenu,ID_EDITOR_COLOCAMESA,MF_CHECKED);
			else
				CheckMenuItem(hmenu,ID_EDITOR_COLOCAMESA,MF_UNCHECKED);

			//Pregunta que boton esta presionado para marcar su
			//respectivo menu item
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,1,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,2,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,3,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,4,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,5,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,6,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,7,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,8,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,9,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_CHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_UNCHECKED);
			}
			else
			if(SendMessage(hwndTB2,TB_ISBUTTONCHECKED,10,0))
			{
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA1,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA2,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA3,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA4,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA5,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA6,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA7,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA8,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA9,MF_UNCHECKED);
				CheckMenuItem(hmenu,ID_EDITOR_PIEZA10,MF_CHECKED);
			}

			return 0;

		case WM_MENUSELECT:
			//			0 0
			//_____ooo0__J__0ooo____
			//Esta funcion esta perrisima!!!
			
			//Se obtiene el manejador del menu
			hmenu = (HMENU) lParam;
			//Luego del submenu
			hmenu = GetSubMenu(hmenu,1);

			//Se inicializa el arreglo que supuestamente 
			//usa la funcion MenuHelp para encontrar
			//las cadenas... ie STRINGTABLE
			//se inicializa el primero solamente
    		popstr[0].hMenu    = hmenu;
			popstr[0].uiString = ID_ARCHIVO_NUEVO;

			MenuHelp(WM_MENUSELECT,wParam,lParam,hmenu,hInst,
					 hwndST,(UINT*)popstr);
			return 0;

		case WM_NOTIFY:

			pnmh = (LPNMHDR) lParam;
			
			//Si se requiere desplegar el tooltip...
			if(pnmh->code == TTN_NEEDTEXT)
			{
				//Esta cosa contiene el ID del boton que necesita
				//el tooltip
				lpttt = (LPTOOLTIPTEXT) lParam;
				switch(lpttt->hdr.idFrom)
				{
					//Tooltips para los botones de las piezas
					case 1: 
						pReply = "Cubo";
						break;
					case 2:
						pReply = "Base";
						break;
					case 3:
						pReply = "Cilindro";
						break;
					case 4:
						pReply = "Extensión";
						break;
					case 5:
						pReply = "Pinza";
						break;
					case 6:
						pReply = "Segmento largo";
						break;
					case 7:
						pReply = "Cubo con soporte";
						break;
					case 8:
						pReply = "Segmento corto";
						break;
					case 9:
						pReply = "Complemento p/pinza";
						break;
					case 10:
						pReply = "Barra deslizante";
						break;
					//Tooltips para los botones de archivo (TB1)
					case ID_ARCHIVO_NUEVO:
						pReply = "Crea un nuevo archivo";
						break;
					case ID_ARCHIVO_ABRIR:
						pReply = "Abre un archivo existente";
						break;
					case ID_ARCHIVO_GUARDAR:
						pReply = "Guarda en un archivo";
						break;
					case ID_EDITOR_BORRARPIEZA:
						pReply = "Borra la ultima pieza";
						break;
					case ID_EDITOR_COLOCARPIEZA_ARRIBA:
						pReply = "Coloca la pieza arriba";
						break;
					case ID_EDITOR_COLOCARPIEZA_DERECHA:
						pReply = "Coloca la pieza a la derecha";
						break;
					case ID_EDITOR_ALAMBRADO:
						pReply = "Sólido/Alambrado";
						break;
					case ID_EDITOR_COLOCAMESA:
						pReply = "Coloca la mesa";
						break;
					case ID_AYUDA_CONTENIDO:
						pReply = "Ayuda del sistema";
						break;
					//Tooltips para los botones de camara
					case ID_VISTA_CAMARAX:
						pReply = "Rota la cámara sobre el eje X";
						break;
					case ID_VISTA_CAMARAY:
						pReply = "Rota la cámara sobre el eje Y";
						break;
					case ID_VISTA_CAMARAZ:
						pReply = "Rota la cámara sobre el eje Z";
						break;
					case ID_VISTA_CAMARAXNEG:
						pReply = "Rota la cámara sobre el eje -X";
						break;
					case ID_VISTA_CAMARAYNEG:
						pReply = "Rota la cámara sobre el eje -Y";
						break;
					case ID_VISTA_CAMARAZNEG:
						pReply = "Rota la cámara sobre el eje -Z";
						break;
					case ID_VISTA_ACERCAR:
						pReply = "Acerca la cámara";
						break;
					case ID_VISTA_ALEJAR:
						pReply = "Aleja la cámara";
						break;
					case ID_SIMULADOR_MOVERPIEZAMAS:
						pReply = "Rota la pieza seleccionada +";
						break;
					case ID_SIMULADOR_MOVERPIEZAMENOS:
						pReply = "Rota la pieza seleccionada -";
						break;
					case ID_CAMARA_ORIGEN:
						pReply = "Regresa el robot a su posición original";
						break;
					case ID_CAMARA_DERECHA:
						pReply = "Mueve la cámara hacia la derecha";
						break;
					case ID_CAMARA_ABAJO:
						pReply = "Mueve la cámara hacia abajo";
						break;
					case ID_CAMARA_IZQUIERDA:
						pReply = "Mueve la cámara hacia la izquierda";
						break;
					case ID_CAMARA_ARRIBA:
						pReply = "Mueve la cámara hacia arriba";
						break;
				}
				lstrcpy(lpttt->szText,pReply);
			}
			return 0;

		case WM_CHAR:
			//Ajusta la camara con el teclado
			switch(wParam)
			{
				case 'x':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAX,0);
					break;
				case 'X':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAXNEG,0);
					break;
				case 'y':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAY,0);
					break;
				case 'Y':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAYNEG,0);
					break;
				case 'z':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAZ,0);
					break;
				case 'Z':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAZNEG,0);
					break;
				case 'a':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_ACERCAR,0);
					break;
				case 'A':
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_ALEJAR,0);
					break;
				case '+':
					SendMessage(hwnd,WM_COMMAND,ID_SIMULADOR_MOVERPIEZAMAS,0);
					break;
				case '-':
					SendMessage(hwnd,WM_COMMAND,ID_SIMULADOR_MOVERPIEZAMENOS,0);
					break;
			}
			return 0;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_UP:
					SendMessage(hwnd,WM_COMMAND,ID_CAMARA_ARRIBA,0);
					break;
				case VK_DOWN:
					SendMessage(hwnd,WM_COMMAND,ID_CAMARA_ABAJO,0);
					break;
				case VK_LEFT:
					SendMessage(hwnd,WM_COMMAND,ID_CAMARA_IZQUIERDA,0);
					break;
				case VK_RIGHT:
					SendMessage(hwnd,WM_COMMAND,ID_CAMARA_DERECHA,0);
					break;
			}
			return 0;

		case WM_LBUTTONDOWN:
			//Se obtienen las coordenadas del mouse cuando se 
			//presiona el boton izquierdo, se guardan en testPos
			//para mas adelante comparar con la nueva posicion 
			//y asi saber si se esta moviendo a la derecha o a la
			//izquierda, hacia arriba o hacia abajo
			testPos.x = mousePos.x = LOWORD(lParam);
			testPos.y = mousePos.y = HIWORD(lParam);
			return 0;

		case WM_RBUTTONDOWN:
			//Se obtiene la coordenada y del mouse cuando se 
			//presiona el boton derecho, se guarda en testPos
			//para mas adelante comparar con la nueva posicion 
			//y asi saber si se esta moviendo hacia arriba o hacia
			//abajo
			testPos.y = mousePos.y = HIWORD(lParam);
			return 0;

    	case WM_MOUSEMOVE:
			//Si se presiona el boton izquierdo mueve la camara
			if(wParam & MK_LBUTTON)
			{
				mousePos.x = LOWORD(lParam);
				mousePos.y = HIWORD(lParam);
				if (mousePos.x > testPos.x)
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAYNEG,0);
				else
				if (mousePos.x < testPos.x)
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAY,0);
				else
				if (mousePos.y > testPos.y)
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAX,0);
				else
				if (mousePos.y < testPos.y)
        			SendMessage(hwnd,WM_COMMAND,ID_VISTA_CAMARAXNEG,0);
				testPos.x = mousePos.x;
				testPos.y = mousePos.y;
			}
			//Si se presiona el boton derecho ajusta el zoom
			if(wParam & MK_RBUTTON)
			{
				mousePos.y = HIWORD(lParam);
				if (mousePos.y > testPos.y)
					SendMessage(hwnd,WM_COMMAND,ID_VISTA_ACERCAR,0);
				else
				if (mousePos.y < testPos.y)
        			SendMessage(hwnd,WM_COMMAND,ID_VISTA_ALEJAR,0);
				testPos.y = mousePos.y;
			}
			return 0;

		case WM_COMMAND:
			iMsgType = LOWORD(wParam);

			if (iMsgType > ID_MENU_OBJ && 
				iMsgType <=(ID_MENU_OBJ+ObjMenuCount))
			{
				ObjSelected = iMsgType % ID_MENU_OBJ;
				ObjectWasSelected = TRUE;
			}

			switch(iMsgType)
			{
				case ID_AYUDA_ACERCADE:
					DialogBox(hInstance,"ID_DIALOG_ACERCADE",hwnd,AboutDlgProc);
					return 0;

				case ID_AYUDA_CONTENIDO:
					ShellExecute(hwnd,"open","edisim2.hlp",NULL,NULL,SW_SHOWNORMAL);
					return 0;

				case ID_AYUDA_DEMO:
					InvalidateRect(hwnd,&rcArea,FALSE);
					return 0;

				case ID_AYUDA_HOMEPAGE:
					ShellExecute(hwnd,"open","iexplore.exe","http://www.geocities.com/edisim2",
							  	 NULL,SW_SHOWNORMAL);
					return 0;

				case MESSAGE:
					InvalidateRect(hwnd,&rcArea,TRUE);
					return 0;

				case ID_ARCHIVO_NUEVO:
					if(bNeedSave && IDCANCEL ==
					   AskAboutSave(hwnd,szTitleName))
							return 0;

					SetWindowText(hwnd,"\0");
					szTitleName[0] = '\0';
					szFileName[0] = '\0';
					DoCaption(hwnd,szTitleName);
					bNeedSave = FALSE;
					while(indice!=1)
						borraorden();
					InvalidateRect(hwnd,&rcArea,TRUE);
					hmenu = GetMenu(hwnd);
					SendMessage(hwndTB1,TB_ENABLEBUTTON,
								ID_EDITOR_COLOCARPIEZA_DERECHA,FALSE);
					EnableMenuItem(hmenu,ID_EDITOR_COLOCARPIEZA_DERECHA,
									MF_GRAYED);
					SendMessage(hwndTB1,TB_ENABLEBUTTON,
								ID_EDITOR_BORRARPIEZA,FALSE);
					EnableMenuItem(hmenu,ID_EDITOR_BORRARPIEZA,MF_GRAYED);
					SendMessage(hwndTB3,TB_ENABLEBUTTON,
								ID_SIMULADOR_MOVERPIEZAMAS,FALSE);
					EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMAS,MF_GRAYED);
					SendMessage(hwndTB3,TB_ENABLEBUTTON,
								ID_SIMULADOR_MOVERPIEZAMENOS,FALSE);
					EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMENOS,MF_GRAYED);
					EnableMenuItem(hmenu,ID_SIMULADOR_ABRIRPINZA,MF_GRAYED);
					EnableMenuItem(hmenu,ID_SIMULADOR_CERRARPINZA,MF_GRAYED);
					EnableMenuItem(hmenu,ID_SIMULADOR_DESLIZAR,MF_GRAYED);
					DeleteObject(hmenu);
					Inicializa();
					return 1;

				case ID_ARCHIVO_ABRIR:
					if(bNeedSave && IDCANCEL ==
					   AskAboutSave(hwnd,szTitleName))
					{
						InvalidateRect(hwnd,&rcArea,TRUE);
						return 0;
					}

                    if (PopFileOpenDlg (hwnd, szFileName, szTitleName))
                    {
						while(indice!=1)
							borraorden();

						if (!PopFileRead (hwnd, szFileName))
                        {
							OkMessage (hwnd, "Error al abrir el archivo %s!",
                                              szTitleName);
                            szFileName[0]  = '\0';
                            szTitleName[0] = '\0';
                         }
                    }

                    DoCaption (hwnd, szTitleName);
                    bNeedSave = FALSE;
					InvalidateRect(hwnd,&rcArea,TRUE);
					hmenu = GetMenu(hwnd);
					SendMessage(hwndTB1,TB_ENABLEBUTTON,
								ID_EDITOR_COLOCARPIEZA_DERECHA,TRUE);
					EnableMenuItem(hmenu,ID_EDITOR_COLOCARPIEZA_DERECHA,
									MF_ENABLED);
					SendMessage(hwndTB1,TB_ENABLEBUTTON,
								ID_EDITOR_BORRARPIEZA,TRUE);
					EnableMenuItem(hmenu,ID_EDITOR_BORRARPIEZA,MF_ENABLED);
					SendMessage(hwndTB3,TB_ENABLEBUTTON,
								ID_SIMULADOR_MOVERPIEZAMAS,TRUE);
					EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMAS,MF_ENABLED);
					SendMessage(hwndTB3,TB_ENABLEBUTTON,
								ID_SIMULADOR_MOVERPIEZAMENOS,TRUE);
					EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMENOS,MF_ENABLED);
					EnableMenuItem(hmenu,ID_SIMULADOR_ABRIRPINZA,MF_ENABLED);
					EnableMenuItem(hmenu,ID_SIMULADOR_CERRARPINZA,MF_ENABLED);
					EnableMenuItem(hmenu,ID_SIMULADOR_DESLIZAR,MF_ENABLED);
					DeleteObject(hmenu);
					Inicializa();
                    return 0;

                case ID_ARCHIVO_GUARDAR:
					if (szFileName[0])
                    {
						if (PopFileWrite (hwnd, szFileName))
                        {
							bNeedSave = FALSE;
                            return 1;
                        }
                        else
							OkMessage (hwnd, "Error al guardar el archivo %s",
                                              szTitleName);
							InvalidateRect(hwnd,&rcArea,TRUE);
                            return 0;
                    }
					else
					{
						if(SendMessage (hwnd, WM_COMMAND, ID_ARCHIVO_GUARDARCOMO, 0L))
							return 1;
						else
							return 0;
					}
					
                case ID_ARCHIVO_GUARDARCOMO:
					if (PopFileSaveDlg (hwnd, szFileName, szTitleName))
                    {
						DoCaption (hwnd, szTitleName);
                        if (PopFileWrite (hwnd, szFileName))
                        {
							bNeedSave = FALSE;
							InvalidateRect(hwnd,&rcArea,TRUE);
                            return 1;
                        }
                        else
						{
							OkMessage (hwnd, "Error al guardar el archivo %s",
                                              szTitleName);
							InvalidateRect(hwnd,&rcArea,TRUE);
						}
                    }
					InvalidateRect(hwnd,&rcArea,TRUE);
                    return 0;

				case ID_ARCHIVO_SALIR:
					SendMessage(hwnd,WM_CLOSE,0,0);
					return 0;

				case ID_EDITOR_PIEZA1:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								1,MAKELONG(TRUE,0));
					iPart=1;
					return 0;

				case ID_EDITOR_PIEZA2:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								2,MAKELONG(TRUE,0));
					iPart=2;
					return 0;

				case ID_EDITOR_PIEZA3:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								3,MAKELONG(TRUE,0));
					iPart=3;
					return 0;

				case ID_EDITOR_PIEZA4:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								4,MAKELONG(TRUE,0));
					iPart=4;
					return 0;

				case ID_EDITOR_PIEZA5:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								5,MAKELONG(TRUE,0));
					iPart=5;
					return 0;

				case ID_EDITOR_PIEZA6:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								6,MAKELONG(TRUE,0));
					iPart=6;
					return 0;

				case ID_EDITOR_PIEZA7:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								7,MAKELONG(TRUE,0));
					iPart=7;
					return 0;

				case ID_EDITOR_PIEZA8:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								8,MAKELONG(TRUE,0));
					iPart=8;
					return 0;

				case ID_EDITOR_PIEZA9:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								9,MAKELONG(TRUE,0));
					iPart=9;
					return 0;

				case ID_EDITOR_PIEZA10:
					SendMessage(hwndTB2,TB_CHECKBUTTON,
								10,MAKELONG(TRUE,0));
					iPart=10;
					return 0;

				case ID_EDITOR_ALAMBRADO:

					//Checa si ya esta en modo Alambrado
					if(WireFrame)
					{
						WireFrame=FALSE;
						SendMessage(hwndTB1,TB_CHECKBUTTON,
									ID_EDITOR_ALAMBRADO,MAKELONG(FALSE,0));
					}
					else
					{
						WireFrame=TRUE;
						SendMessage(hwndTB1,TB_CHECKBUTTON,
									ID_EDITOR_ALAMBRADO,MAKELONG(TRUE,0));
					}

					InvalidateRect(hwnd,&rcArea,FALSE);
					return 0;

				case ID_EDITOR_COLOCAMESA:

					//Checa si ya esta puesta la mesa
					if(IsTableEnabled)
					{
						IsTableEnabled=FALSE;
						SendMessage(hwndTB1,TB_CHECKBUTTON,
									ID_EDITOR_COLOCAMESA,MAKELONG(FALSE,0));
					}
					else
					{
						IsTableEnabled=TRUE;
						SendMessage(hwndTB1,TB_CHECKBUTTON,
									ID_EDITOR_COLOCAMESA,MAKELONG(TRUE,0));
					}

					InvalidateRect(hwnd,&rcArea,FALSE);
					return 0;

				case ID_CAMARA_ORIGEN:
					visual = 0.0;
					distance = FigSize / 100.0;
					PosCamX=0;
					PosCamY=0;
					Mat_Identity_4x4(V);
					Mat_Identity_4x4(Vaux);
					RotateCamX(90);
					InvalidateRect(hwnd,&rcArea,FALSE);
					return 0;

				case ID_EDITOR_COLOCARPIEZA_ARRIBA:
				case ID_EDITOR_COLOCARPIEZA_DERECHA:
				case ID_EDITOR_BORRARPIEZA:
					bNeedSave = TRUE;

				case ID_SIMULADOR_ABRIRPINZA:
				case ID_SIMULADOR_CERRARPINZA:
				case ID_SIMULADOR_MOVERPIEZAMAS:
				case ID_SIMULADOR_MOVERPIEZAMENOS:
				case ID_SIMULADOR_DESLIZAR:
				case ID_VISTA_CAMARAX:				
				case ID_VISTA_CAMARAY:
				case ID_VISTA_CAMARAZ:
				case ID_VISTA_CAMARAXNEG:
				case ID_VISTA_CAMARAYNEG:
				case ID_VISTA_CAMARAZNEG:
				case ID_VISTA_ACERCAR:
				case ID_VISTA_ALEJAR:
				case ID_CAMARA_DERECHA:
				case ID_CAMARA_IZQUIERDA:
				case ID_CAMARA_ARRIBA:
				case ID_CAMARA_ABAJO:
					InvalidateRect(hwnd,&rcArea,FALSE);
					return 0;

				case ID_PIEZA_1:
					iPart=1;
					return 0;

				case ID_PIEZA_2:
					iPart=2;
					return 0;

				case ID_PIEZA_3:
					iPart=3;
					return 0;

				case ID_PIEZA_4:
					iPart=4;
					return 0;

				case ID_PIEZA_5:
					iPart=5;
					return 0;

				case ID_PIEZA_6:
					iPart=6;
					return 0;

				case ID_PIEZA_7:
					iPart=7;
					return 0;

				case ID_PIEZA_8:
					iPart=8;
					return 0;

				case ID_PIEZA_9:
					iPart=9;
					return 0;

				case ID_PIEZA_10:
					iPart=10;
					return 0;
			}
			return 0;

        case WM_PAINT: 
			hdc = BeginPaint (hwnd, &ps);

			r.top = 0;
			r.left= 0;
			r.right = cx;
			r.bottom= cy;
			//Se borra el doble buffer
            FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));

			switch(iMsgType)
			{
				case ID_AYUDA_DEMO:
					if(SendMessage(hwnd,WM_COMMAND,ID_ARCHIVO_NUEVO,0))
					{
						Demo(hdc);
						SendMessage(hwnd,WM_COMMAND,ID_ARCHIVO_NUEVO,0);
					}
					return 0;

				case MESSAGE:
					OkMessage (hwnd, szMsg,szTitleName);
					DrawRobot(hdc);
					return 0;

				case ID_EDITOR_COLOCARPIEZA_ARRIBA:

					hmenu = GetMenu(hwnd);

					if(iPart==5)
					{
						EnableMenuItem(hmenu,ID_SIMULADOR_ABRIRPINZA,MF_ENABLED);
						EnableMenuItem(hmenu,ID_SIMULADOR_CERRARPINZA,MF_ENABLED);
					}
					if(iPart==10)
						EnableMenuItem(hmenu,ID_SIMULADOR_DESLIZAR,MF_ENABLED);

					if(!SendMessage(hwndTB1,TB_ISBUTTONENABLED,
								ID_EDITOR_COLOCARPIEZA_DERECHA,0))
					{
						SendMessage(hwndTB1,TB_ENABLEBUTTON,
									ID_EDITOR_COLOCARPIEZA_DERECHA,TRUE);
						EnableMenuItem(hmenu,ID_EDITOR_COLOCARPIEZA_DERECHA,
										MF_ENABLED);
						SendMessage(hwndTB1,TB_ENABLEBUTTON,
									ID_EDITOR_BORRARPIEZA,TRUE);
						EnableMenuItem(hmenu,ID_EDITOR_BORRARPIEZA,MF_ENABLED);
						SendMessage(hwndTB3,TB_ENABLEBUTTON,
									ID_SIMULADOR_MOVERPIEZAMAS,TRUE);
						EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMAS,MF_ENABLED);
						SendMessage(hwndTB3,TB_ENABLEBUTTON,
									ID_SIMULADOR_MOVERPIEZAMENOS,TRUE);
						EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMENOS,MF_ENABLED);
					}

					DeleteObject(hmenu);
					//Inicializa los angulos de rotacion para las piezas
					for(i=0; i<50; i++)
						RotAng[i]=0;
					RestauraVpE();
					PlacePart(iPart,UP,hdc);
					return 0;

				case ID_EDITOR_COLOCARPIEZA_DERECHA:

					hmenu = GetMenu(hwnd);

					if(iPart==5)
					{
						EnableMenuItem(hmenu,ID_SIMULADOR_ABRIRPINZA,MF_ENABLED);
						EnableMenuItem(hmenu,ID_SIMULADOR_CERRARPINZA,MF_ENABLED);
					}
					if(iPart==10)
						EnableMenuItem(hmenu,ID_SIMULADOR_DESLIZAR,MF_ENABLED);

					if(!SendMessage(hwndTB1,TB_ISBUTTONENABLED,
								ID_EDITOR_COLOCARPIEZA_DERECHA,0))
					{
						SendMessage(hwndTB1,TB_ENABLEBUTTON,
									ID_EDITOR_COLOCARPIEZA_DERECHA,TRUE);
						EnableMenuItem(hmenu,ID_EDITOR_COLOCARPIEZA_DERECHA,
										MF_ENABLED);
					}

					DeleteObject(hmenu);

					//Inicializa los angulos de rotacion para las piezas
					for(i=0; i<50; i++)
						RotAng[i]=0;
					RestauraVpE();
					PlacePart(iPart,RIGHT,hdc);
					return 0;

				case ID_EDITOR_BORRARPIEZA:
					borrar=1;
					borraorden();
					if(indice == 1)
					{
						FinalPos.x = 0.0;
						FinalPos.y = 0.0;
						FinalPos.z = 0.0;
					}
					DrawRobot(hdc);
					if(SendMessage(hwndTB1,TB_ISBUTTONENABLED,
								ID_EDITOR_COLOCARPIEZA_DERECHA,0)&&
						indice == 1)
					{
						hmenu = GetMenu(hwnd);
						SendMessage(hwndTB1,TB_ENABLEBUTTON,
									ID_EDITOR_COLOCARPIEZA_DERECHA,FALSE);
						EnableMenuItem(hmenu,ID_EDITOR_COLOCARPIEZA_DERECHA,
										MF_GRAYED);
						SendMessage(hwndTB1,TB_ENABLEBUTTON,
									ID_EDITOR_BORRARPIEZA,FALSE);
						EnableMenuItem(hmenu,ID_EDITOR_BORRARPIEZA,MF_GRAYED);
						SendMessage(hwndTB3,TB_ENABLEBUTTON,
									ID_SIMULADOR_MOVERPIEZAMAS,FALSE);
						EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMAS,MF_GRAYED);
						SendMessage(hwndTB3,TB_ENABLEBUTTON,
									ID_SIMULADOR_MOVERPIEZAMENOS,FALSE);
						EnableMenuItem(hmenu,ID_SIMULADOR_MOVERPIEZAMENOS,MF_GRAYED);
						DeleteObject(hmenu);
					}
					return 0;

				case ID_SIMULADOR_ABRIRPINZA:
					OpenCloseHand(1,1,0,hdc);
					return 0;

				case ID_SIMULADOR_CERRARPINZA:
					OpenCloseHand(0,1,0,hdc);
					return 0;

				case ID_SIMULADOR_MOVERPIEZAMAS:
					mover=1;
					RotateRobot(ObjSelected-1,1,hdc);
					return 0;

				case ID_SIMULADOR_MOVERPIEZAMENOS:
					mover=1;
					RotateRobot(ObjSelected-1,2,hdc);
					return 0;

				case ID_SIMULADOR_DESLIZAR:
					OpenCloseHand(1,0,ObjSelected-1,hdc);
					return 0;

				case ID_CAMARA_DERECHA:
					PosCamX-=10;
					DrawRobot(hdc);
					return 0;

				case ID_CAMARA_IZQUIERDA:
					PosCamX+=10;
					DrawRobot(hdc);
					return 0;

				case ID_CAMARA_ABAJO:
					PosCamY-=10;
					DrawRobot(hdc);
					return 0;

				case ID_CAMARA_ARRIBA:
					PosCamY+=10;
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_CAMARAX:
					RotateCamX(5);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_CAMARAY:
					RotateCamY(5);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_CAMARAZ:
					RotateCamZ(5);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_CAMARAXNEG:
					RotateCamX(355);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_CAMARAYNEG:
					RotateCamY(355);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_CAMARAZNEG:
					RotateCamZ(355);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_ACERCAR:
					CamaraAA(2,hdc);
					DrawRobot(hdc);
					return 0;

				case ID_VISTA_ALEJAR:
					CamaraAA(1,hdc);
					DrawRobot(hdc);
					return 0;

				default:
					DrawRobot(hdc);
					return 0;
			}

			EndPaint (hwnd, &ps);
            return 0;

		case WM_SIZE:
			//Se obtiene el largo y ancho de la ventana principal
			cx = LOWORD(lParam);
			cy = HIWORD(lParam);

			//Si existe el bitmap del doble buffer, borrarlo
			if(hbmBuffer)
				DeleteObject(hbmBuffer);
			
			//Se obtiene el DC de la ventana
			hdc = GetDC(hwnd);

			//Se crea un bitmap compatible con el DC donde se pinta
			hbmBuffer = CreateCompatibleBitmap(hdc,cx,cy);

			//Se selecciona este bitmap en el doble buffer
			SelectObject(hdcBuffer,hbmBuffer);
			ReleaseDC(hwnd,hdc);

			//Se obtienen las coordenadas del ToolBar1
			GetWindowRect(hwndTB1,&r);

			//Se obtiene el ancho del ToolBar1
			y=r.bottom-r.top;

			//Se cambia de tamaño y posicion el ToolBar1
			MoveWindow(hwndTB1,0,0,cx,y,TRUE);

			a=cx-y;
			b=y;
			c=cy-y;

			MoveWindow(hwndTB3,a,y,b,cy-y,TRUE);

			GetWindowRect(hwndST,&r);
			y=r.bottom-r.top;
			MoveWindow(hwndST,0,cy-y,cx,y,TRUE);
			SetRect(&rcArea,b*2+5,b,cx-b,cy-y);

			c=c-y;
			MoveWindow(hwndTB2,0,b,b*2+5,c,TRUE);
			
			//Ajusta el tamaño de las ventanas del Status Bar
			//la primera sera de 4/5 partes
			SbWd[0] = cx - cx/5;
			SbWd[1] = -1;

			//Divide el status bar en 2, una con mensajes y 
			//otra con las coordenadas del elemento terminal
			SendMessage(hwndST,SB_SETPARTS,2,(LPARAM)SbWd);
			sprintf(szPos,"x=%.1f, y=%.1f, z=%.1f",
					FinalPos.x,FinalPos.z,FinalPos.y);
			SendMessage(hwndST,SB_SETTEXT,0,(LPARAM)"Listo");
			SendMessage(hwndST,SB_SETTEXT,1,(LPARAM)szPos);
	
			return 0;

		case WM_ACTIVATE:
			if(LOWORD(wParam)!=WA_INACTIVE)
			{
				sprintf(szPos,"x=%.1f, y=%.1f, z=%.1f",
						FinalPos.x,FinalPos.z,FinalPos.y);
				SendMessage(hwndST,SB_SETTEXT,0,(LPARAM)"Listo");
				SendMessage(hwndST,SB_SETTEXT,1,(LPARAM)szPos);
			}
			return 0;

		case WM_CLOSE:
			if(bNeedSave && IDCANCEL ==
				AskAboutSave(hwnd,szTitleName))
			{
				InvalidateRect(hwnd,&rcArea,TRUE);
				return 0;
			}
			else
				DestroyWindow(hwnd);

		case WM_DESTROY:
			//Si existe el bitmap del doble buffer, borrarlo
			if(hbmBuffer)
				DeleteObject(hbmBuffer);

			//Se borra el DC del doble buffer
			DeleteDC(hdcBuffer);

			PostQuitMessage (0);
            return 0;
    }

	return DefWindowProc (hwnd, iMsg, wParam, lParam);
}

/*------------------------------------------------------------
	AboutDlgProc: Manejo del DialogBox Acerca de
  ------------------------------------------------------------*/
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam,
												 LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD (wParam))
			{
				case IDOK:
					EndDialog(hDlg,0);
					return TRUE;
			}
			break;
	}
	return FALSE;
}

/*------------------------------------------------------------
	DoCaption: Pone el titulo en la ventana principal
  ------------------------------------------------------------*/
void DoCaption (HWND hwnd, char *szTitleName)
{
	char szCaption[64 + _MAX_FNAME + _MAX_EXT] ;
    wsprintf (szCaption, "%s - %s", szAppName,
               szTitleName[0] ? szTitleName : UNTITLED) ;
    SetWindowText (hwnd, szCaption) ;
}

/*------------------------------------------------------------
	OkMessage: Manda un mensaje *szMessage
  ------------------------------------------------------------*/
void OkMessage (HWND hwnd, char *szMessage, char *szTitleName)
{
    char szBuffer[64 + _MAX_FNAME + _MAX_EXT] ;
    wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;
    MessageBox (hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
}

/*------------------------------------------------------------
	AskAboutSave: pregunta si desea salvar el archivo
  ------------------------------------------------------------*/
short AskAboutSave (HWND hwnd, char *szTitleName)
{
    char szBuffer[64 + _MAX_FNAME + _MAX_EXT] ;
    int  iReturn ;
    wsprintf (szBuffer, "¿Guardar cambios en %s?",
              szTitleName[0] ? szTitleName : UNTITLED) ;
    iReturn = MessageBox (hwnd, szBuffer, szAppName,
                          MB_YESNOCANCEL | MB_ICONQUESTION) ;
    if (iReturn == IDYES)
         if (!SendMessage (hwnd, WM_COMMAND, ID_ARCHIVO_GUARDAR, 0L))
              iReturn = IDCANCEL ;
    return iReturn ;
}

/*------------------------------------------------------------
	Inicializa: Inicializa variables
  ------------------------------------------------------------*/
void Inicializa()
{
	char szPos[40];

	PosCamX  = 0;
	PosCamY  = 0;
	zoom     = FigSize * 15.0;
	distance = FigSize / 100.0;

	//Crea las tablas de senos y cosenos
	table();

	//Inicializa la matriz de rotacion V
	Mat_Identity_4x4(V);
	Mat_Identity_4x4(Vaux);

	//Inicializa la matriz de rotacion
	Mat.Q.x=1.0;
	Mat.Q.y=0.0;
	Mat.Q.z=0.0;
	Mat.Q.ang=2.0;
	CreaMatRot();

	//Rota la camara 90 grados
	RotateCamX(90);

	iPart=1;

    //Inicializa el arreglo que contiene las barras deslizantes
	for(i=0;i<50;i++) 
		barraab[i]=0;

	for(i=0; i<50; i++)
		RotAng[i]=0;

	VectPosCubo.x = 200.0;
	VectPosCubo.y = 0.0;
	VectPosCubo.z = 0.0;
	
	VectAlturaCubo.x = 0.0;
	VectAlturaCubo.y = 0.0;
	VectAlturaCubo.z = 14.0;
	
	VectPerpenCubo.x = 0.0;
	VectPerpenCubo.y = 7.0;
	VectPerpenCubo.z = 0.0;

	for(i=0; i<6; i++)
		for(j=0; j<4; j++)
			CubeAux[i][j] = Cube[i][j];

	FinalPos.x=0.0;
	FinalPos.y=0.0;
	FinalPos.z=0.0;
	sprintf(szPos,"x=%.1f, y=%.1f, z=%.1f",
			FinalPos.x,FinalPos.z,FinalPos.y);
	SendMessage(hwndST,SB_SETTEXT,1,(LPARAM)szPos);

	ManoAbierta=0;
	IsCubeTaken=FALSE;
}

/*------------------------------------------------------------
	Table: Crea las tablas de Senos y Cosenos de los angulos
	de 0 a 360 para no tener que calcular sus valores cada vez
	que se requieran, solo se buscan en la tabla
  ------------------------------------------------------------*/
void table()
{
   int	 angle;
   double radius;

   for (angle=0; angle<=360; angle++)
   {
       radius = (double) ((PI*(double)angle)/(double)180);
       cos_table[angle] = (double)cos(radius);
       sin_table[angle] = (double)sin(radius);
   }
}

/*------------------------------------------------------------
	Mat_Identity_4x4: Crea una matriz identidad 4x4
	En lugar de asignar 0's y que el procesador de punto 
	flotante convierta estos a reales temporales y despues 
	los asigne a elementos del arreglo  <-- LENTO
	Se llena el arreglo con 0's binarios y la funcion memset
	que es muy rapida  =)
  ------------------------------------------------------------*/
void Mat_Identity_4x4(matrix_4x4 a)
{
   memset((void *)a,0,sizeof(double)*16);
   a[0][0] = a[1][1] = a[2][2] = a[3][3] = 1;
}

/*------------------------------------------------------------
	RotateCamX: Rota la camara sobre el eje X
  ------------------------------------------------------------*/
void RotateCamX( int ang )
{
	Vaux[2][1] = cos_table[ang]*V[2][1] - sin_table[ang]*V[3][1] ;
	Vaux[2][2] = cos_table[ang]*V[2][2] - sin_table[ang]*V[3][2] ;
	Vaux[2][3] = cos_table[ang]*V[2][3] - sin_table[ang]*V[3][3] ;
	Vaux[3][1] = sin_table[ang]*V[2][1] + cos_table[ang]*V[3][1] ;
	Vaux[3][2] = sin_table[ang]*V[2][2] + cos_table[ang]*V[3][2] ;
	Vaux[3][3] = sin_table[ang]*V[2][3] + cos_table[ang]*V[3][3] ;
	for( i=1; i<4; i++)
		for( j=1; j<4; j++) 
			V[i][j] = Vaux[i][j];
}

/*------------------------------------------------------------
	RotateCamY: Rota la camara sobre el eje Y
  ------------------------------------------------------------*/
void RotateCamY( int ang )
{
	Vaux[1][1] = cos_table[ang]*V[1][1] + sin_table[ang]*V[3][1] ;
	Vaux[1][2] = cos_table[ang]*V[1][2] + sin_table[ang]*V[3][2] ;
	Vaux[1][3] = cos_table[ang]*V[1][3] + sin_table[ang]*V[3][3] ;
	Vaux[3][1] = -sin_table[ang]*V[1][1] + cos_table[ang]*V[3][1] ;
	Vaux[3][2] = -sin_table[ang]*V[1][2] + cos_table[ang]*V[3][2] ;
	Vaux[3][3] = -sin_table[ang]*V[1][3] + cos_table[ang]*V[3][3] ;
	for( i=1; i<4; i++)
		for( j=1; j<4; j++) 
			V[i][j] = Vaux[i][j];
}

/*------------------------------------------------------------
	RotateCamZ: Rota la camara sobre el eje Z
  ------------------------------------------------------------*/
void RotateCamZ( int ang )
{
	Vaux[1][1] = cos_table[ang]*V[1][1] - sin_table[ang]*V[2][1] ;    /* [Rz] * [V] */
	Vaux[1][2] = cos_table[ang]*V[1][2] - sin_table[ang]*V[2][2] ;
	Vaux[1][3] = cos_table[ang]*V[1][3] - sin_table[ang]*V[2][3] ;
	Vaux[2][1] = sin_table[ang]*V[1][1] + cos_table[ang]*V[2][1] ;
	Vaux[2][2] = sin_table[ang]*V[1][2] + cos_table[ang]*V[2][2] ;
	Vaux[2][3] = sin_table[ang]*V[1][3] + cos_table[ang]*V[2][3] ;
	for( i=1; i<4; i++)
		for( j=1; j<4; j++) 
			V[i][j] = Vaux[i][j];
}

/*------------------------------------------------------------
	CreaMatRot: Crea la matriz de rotacion para los puntos
  ------------------------------------------------------------*/
void CreaMatRot( )
{
	double  k,u,v,w,b;
	int		ang;

	ang=(int)(Mat.Q.ang/2.0);

	k=sin_table[ang];
	b=cos_table[ang];

    u=Mat.Q.x*k; v=Mat.Q.y*k; w=Mat.Q.z*k;

    Mat.R[1][1]= b*b + u*u - v*v - w*w;
    Mat.R[1][2]= 2*( u*v - b*w );
    Mat.R[1][3]= 2*( b*v + u*w );
    Mat.R[2][1]= 2*( b*w + u*v );
    Mat.R[2][2]= b*b + v*v - u*u - w*w;
    Mat.R[2][3]= 2*( v*w - b*u );
    Mat.R[3][1]= 2*( u*w - b*v );
    Mat.R[3][2]= 2*( b*u + v*w );
    Mat.R[3][3]= b*b + w*w - u*u - v*v;
}

/*------------------------------------------------------------
	RotaPtos: Rota los puntos de un plano
  ------------------------------------------------------------*/
void RotaPtos( double *x, double *y, double *z )
{
	double x1, y1, z1;

    x1= Mat.R[1][1]**x + Mat.R[1][2]**y + Mat.R[1][3]**z;
    y1= Mat.R[2][1]**x + Mat.R[2][2]**y + Mat.R[2][3]**z;
    z1= Mat.R[3][1]**x + Mat.R[3][2]**y + Mat.R[3][3]**z;
    *x=x1;
	*y=y1;
	*z=z1;
}

/*------------------------------------------------------------
	PerspPoint: Proyeccion perspectiva de un punto (x,y,z)
  ------------------------------------------------------------*/
void Persp_Point(double x,double y,double z,long *xP,long *yP)
{
	//Con esto se logra el efecto de traslacion de la camara
	//en x,y,z
	x += PosCamX;
	y += PosCamY;
	z += zoom;

	*xP  = (long)(HALF_SCREEN_WIDTH + (x * RESX * distance)/z);
	*yP  = (long)(HALF_SCREEN_HEIGHT + (ASPECT_RATIO * y * RESY * distance)/z);
}

/*------------------------------------------------------------
	DrawRobot: Pinta el robot
  ------------------------------------------------------------*/
void RotateRobot(int param,int op,HDC hdc)
{	
	double x, y, z;
	double ax,ay,az;
	int Ini2;
	int MaxAngle=360,
		MinAngle=-360;
	BOOL c = FALSE;

	if(indice>1)
	{
		ax=ay=az=0.0;
		if(param>0 )
			if((param>0 && param < cc) && ordenr[param][0]!=3  && ordenr[param][0]!=13)
			{ 
				Ini2=ordenr[param][1];
				switch ( op ) 
				{
					case 2: 
						x=-Eje1[param].x;
						y=-Eje1[param].y;
						z=-Eje1[param].z;
						RotAng[param]+=(int)Mat.Q.ang;
						break;

					case 1:   
						x=Eje1[param].x;
						y=Eje1[param].y;
						z=Eje1[param].z;
						RotAng[param]-=(int)Mat.Q.ang;
						break;
				}

				if(RotAng[param]>360)
					RotAng[param]-=360;
				else if(RotAng[param]<-360)
					RotAng[param]+=360;

				if(ordenr[param][0]==6 ||
				   ordenr[param][0]==8 ||
				   ordenr[param][0]==9)
				{
				   if(ordenr[param-1][0]==3)
				   {
					   MaxAngle=210;
					   MinAngle=-30;
				   }
				   else
				   {
					   MaxAngle=90;
					   MinAngle=-90;
				   }
				}

				if(ordenr[param][0]==16 ||
				   ordenr[param][0]==18 ||
				   ordenr[param][0]==19)
				{
				   if(ordenr[param-1][0]==13)
				   {
					   MaxAngle=140;
					   MinAngle=-130;
				   }
				   else
				   {
					   MaxAngle=90;
					   MinAngle=-90;
				   }
				}

				//Se rotan primero los datos y vectores de pos.
				//para saber si hay colisiones
				Mat.Q.x = x;
				Mat.Q.y = y;
				Mat.Q.z = z;
				CreaMatRot();

				for( i=OrdenDatos[param][0]; i < l; i++ )
				{
					for( j=0; j< 4; j++ )
					{
						RotaPtos(&Datos[i][j].x, 
								 &Datos[i][j].y, 
								 &Datos[i][j].z );
					}
				}

				for( i=Ini2+1; i <= cc; i++ )
				{
					RotaPtos(&VectPos1[i].x,
							 &VectPos1[i].y,
							 &VectPos1[i].z);
				}

				c = Collision(param);

				if((RotAng[param]<=MaxAngle && RotAng[param]>=MinAngle) &&
					!c)
				{
					for( i=Ini2+1; i <= cc; i++ )
						RotaPtos(&Eje1[i].x,
								 &Eje1[i].y,
								 &Eje1[i].z );

					for(j=0; j<indice-1; j++)
					{
						ax += VectPos1[ordenr[j][1]].x/2;
						ay += VectPos1[ordenr[j][1]].y/2;
						az += VectPos1[ordenr[j][1]].z/2;
					}

					if(IsCubeTaken)
					{
						RotaPtos(&ex,&ey,&ez);
						RotaPtos(&VectAlturaCubo.x,
								 &VectAlturaCubo.y,
								 &VectAlturaCubo.z);
						RotaPtos(&VectPerpenCubo.x,
								 &VectPerpenCubo.y,
								 &VectPerpenCubo.z);
						VectPosCubo.x = ax+ex;
						VectPosCubo.y = ay+ey;
						VectPosCubo.z = az+ez;
						for(i=0; i<6; i++)
						{
							for(j=0; j<4; j++)
							{
								RotaPtos(&CubeAux[i][j].x,
										 &CubeAux[i][j].y,
										 &CubeAux[i][j].z);
							}
						}
					}

					//espacio entre el cubo y la mano
					ex = VectPosCubo.x - ax;
					ey = VectPosCubo.y - ay;
					ez = VectPosCubo.z - az;

					//Se manda el mensaje al Status Bar
					SendMessage(hwndST,SB_SETTEXT,0,(LPARAM)"Listo");

					DrawRobot(hdc);  
				}
				else
				{
					MessageBeep(MB_OK);
					if(op==1)
						RotAng[param]+=(int)Mat.Q.ang;
					if(op==2)
						RotAng[param]-=(int)Mat.Q.ang;

					//Regresamos los Vect Pos a su posicion original
					Mat.Q.x = -x;
					Mat.Q.y = -y;
					Mat.Q.z = -z;
					CreaMatRot();
					for( i=OrdenDatos[param][0]; i < l; i++ )
					{
						for( j=0; j< 4; j++ )
						{
							RotaPtos(&Datos[i][j].x, 
									 &Datos[i][j].y, 
									 &Datos[i][j].z );
						}
					}

					for( i=Ini2+1; i <= cc; i++ )
					{
						RotaPtos(&VectPos1[i].x,
								 &VectPos1[i].y,
								 &VectPos1[i].z);
					}

					//Se manda el mensaje al Status Bar
					SendMessage(hwndST,SB_SETTEXT,0,
								(LPARAM)"Las piezas chocan");

				}//else
			}
	}

	else 
	{
		szMsg="La pieza no se puede rotar";
		SendMessage (hwnd, WM_COMMAND, MESSAGE, 0L);
	}
}

/*------------------------------------------------------------
	RobotAssembly: Arma el robot pieza por pieza
  ------------------------------------------------------------*/
void RobotAssembly( int ini, int fin, int iVect, int cual, int pos)
{
	int i, j,c,d;
	struct Pto3D Delta;

	Delta.x= 0.0;
	Delta.y= 0.0;
	Delta.z= 0.0;

	for( i=0; i<=iVect; i++)
	{
		Delta.x = Delta.x + VectPos1[i].x ;
		Delta.y = Delta.y + VectPos1[i].y ;
		Delta.z = Delta.z + VectPos1[i].z ;
	}

	FinalPos.x = (VectPos1[iVect+1].x + Delta.x)/2;
	FinalPos.y = (VectPos1[iVect+1].y + Delta.y)/2;
	FinalPos.z = (VectPos1[iVect+1].z + Delta.z)/2;

	c=0;

	if(cual==1)
	{
		d=NumPlaRob;
		for( i=ini; i<fin; i++ )
		{
			for( j=0; j< 4; j++ )
			{
				Robot[c+d][j].x = (DatosO[i][j].x + Delta.x)/2;
				Robot[c+d][j].y = (DatosO[i][j].y + Delta.y)/2;
				Robot[c+d][j].z = (DatosO[i][j].z + Delta.z)/2;
			} 
			c++;
		}
	}

	else
	{  
		d=0;
		for( i=ini; i<fin; i++ )
		{
			for( j=0; j< 4; j++ )
			{
				Robot[c+pos+d][j].x = (Datos[i][j].x + Delta.x)/2;
				Robot[c+pos+d][j].y = (Datos[i][j].y + Delta.y)/2;
				Robot[c+pos+d][j].z = (Datos[i][j].z + Delta.z)/2;
			} 
			c++;
		}
   }
}

/*------------------------------------------------------------
	OrdenaPlanos: Oredena los planos usando el algoritmo
	del pintor del promedio de z 
	para saber cual se pinta primero
  ------------------------------------------------------------*/
void OrdenaPlanos(int a,int b,struct Order O[],struct Obj Object[][4])
{
	double zAux;

	for(i=a;i<b;i++)
	{
		zAux = (Object[i][0].z + Object[i][2].z) / 2.0 ;
		Distance[i] = zAux;
		O[i].p = Object[i][0].plane;
		O[i].type = Object[i][0].type;
	}

	qs(a,b-1,O);
}

/*------------------------------------------------------------
	qs: Realiza un ordenamiento por el metodo Quick Sort
  ------------------------------------------------------------*/
void qs(int left,int right,struct Order O[])
{
	int i, last;
	
	if (left >= right)
		return;
	swap(left,(left+right)/2,O);
	last= left;

	for (i= left+1; i<=right; i++)
		if(Distance[i] > Distance[left])
			swap(++last,i,O);
	 swap(left,last,O);
	 qs(left,last-1,O);
	 qs(last+1,right,O);
 }

/*------------------------------------------------------------
	swap: util en el algoritmo Quick Sort
  ------------------------------------------------------------*/
void swap(int i,int j,struct Order O[])
{
	double dAux;
	int    iAux,tAux;

	dAux = Distance[i];
	Distance[i] = Distance[j];
	Distance[j] = dAux;

	iAux = O[i].p;
	tAux = O[i].type;
	O[i].p = O[j].p;
	O[i].type = O[j].type;
	O[j].p = iAux;
	O[j].type = tAux;
}

/*------------------------------------------------------------
	DrawPlane: Pinta los planos del robot
	tambien pinta el piso, la mesa, el cubo y las sombras
  ------------------------------------------------------------*/
void DrawPlane(int a, int b, HDC hdc)
{
	int i,j;
	char szPos[40];
	struct Pto2D Pantalla[4],Shadow[4];
	POINT sombra[4];
	POINT lispiso[4];
	HBRUSH hOrgBrush;
	HPEN hOrgPen;

	//calcula la distancia del objeto al ojo
	distance=distance+visual;

	if(distance<=0.0) 
		distance=0.5; 
	else if(distance>10.0)
		distance=10.0;

	//Pinta el piso
	//Verifica si el piso es visible
	if (Visible(l,Robot)) 
	{
		for( j=0; j < 4; j++ )
		{
			Persp_Point(Robot[l][j].x,Robot[l][j].y,Robot[l][j].z,
						&Pantalla[j].x, &Pantalla[j].y);
		}
		for(j=0; j<4; j++) 
		{
			lispiso[j].x=Pantalla[j].x;
			lispiso[j].y=Pantalla[j].y;
		}  

		//selecciona un color azul
		hOrgBrush = SelectObject(hdcBuffer, CreateSolidBrush(RGB(80,145,243)));
		hOrgPen = SelectObject(hdcBuffer, GetStockObject(BLACK_PEN));
		Polygon(hdcBuffer,lispiso,4);

		//terminamos de usar Brush y pen, asi que se borran
		DeleteObject(hOrgBrush);
		DeleteObject(hOrgPen);
	}
		

	//Pinta las sombras
	//Verifica si el piso es visible, si no lo es no hay donde
	//proyectarlas!
	if(Visible(l,Robot))
	{
		for(i=a; i<l; i++)
		{
			for( j=0; j < 4; j++ )
			{
				//La proyeccion de las sombras en el piso 
				//ya se calculo en DrawRobot()
				//Se calcula la proyeccion perspectiva y listo!
				Persp_Point(SRx[i][j],SRy[i][j],0,&Shadow[j].x,&Shadow[j].y);
			}
			for(j=0; j<4; j++) 
			{
				sombra[j].x=Shadow[j].x;
				sombra[j].y=Shadow[j].y;
			}

			//Se utiliza casi la mitad del color del piso, 
			//asi se vera azul obscuro pero no negro
			hOrgBrush = SelectObject(hdcBuffer,CreateSolidBrush(RGB(60,120,220)));
			hOrgPen = SelectObject(hdcBuffer,GetStockObject(NULL_PEN) );
			Polygon(hdcBuffer, sombra, 4);

			//terminamos con la brush y la pen asi que se borran
			DeleteObject(hOrgBrush);
			DeleteObject(hOrgPen);
		}		

		if(IsTableEnabled)
		{
			//Sombra de la mesa
			for(i=0; i<22; i++)
			{
				for( j=0; j < 4; j++ )
				{
					Persp_Point(STx[i][j],STy[i][j],0,&Shadow[j].x,&Shadow[j].y);
				}
				for(j=0; j<4; j++) 
				{
					sombra[j].x=Shadow[j].x;
					sombra[j].y=Shadow[j].y;
				}

				hOrgBrush = SelectObject(hdcBuffer,CreateSolidBrush(RGB(60,120,220)));
				hOrgPen = SelectObject(hdcBuffer,GetStockObject(NULL_PEN) );
				Polygon(hdcBuffer, sombra, 4);

				DeleteObject(hOrgBrush);
				DeleteObject(hOrgPen);
			}		

			//Sombra del cubo
			for(i=0; i<6; i++)
			{
				for( j=0; j < 4; j++ )
				{
					Persp_Point(SCx[i][j],SCy[i][j],0,&Shadow[j].x,&Shadow[j].y);
				}
				for(j=0; j<4; j++) 
				{
					sombra[j].x=Shadow[j].x;
					sombra[j].y=Shadow[j].y;
				}

				hOrgBrush = SelectObject(hdcBuffer,CreateSolidBrush(RGB(60,120,220)));
				hOrgPen = SelectObject(hdcBuffer,GetStockObject(NULL_PEN) );
				Polygon(hdcBuffer, sombra, 4);

				DeleteObject(hOrgBrush);
				DeleteObject(hOrgPen);
			}		
		}//if IstableEnabled
	}//if Visible(l)

	for(i=a; i<b; i++)
	{
		switch(Order[i].type)
		{
		case 0:
			DrawRobotSolid(Order[i].p);
			break;
		case 1:
			DrawTableSolid(Order[i].p);
			break;
		case 2:
			DrawCubeSolid(Order[i].p);
			break;
		}
	} 

    //pone el mensaje del segundo segmento del SB
	sprintf(szPos,"x=%.1f, y=%.1f, z=%.1f",
			FinalPos.x,FinalPos.z,FinalPos.y);
	SendMessage(hwndST,SB_SETTEXT,1,(LPARAM)szPos);

	//Se copia bit a bit el contenido del Doble buffer a la memoria
	//de video
	BitBlt(hdc,0,0,cx,cy,hdcBuffer,0,0,SRCCOPY);
}

/*------------------------------------------------------------
	DrawPlaneWire: Pinta los planos del robot
	en modo alambrado o wireframe
  ------------------------------------------------------------*/
void DrawPlaneWire(int a, int b, HDC hdc)
{
	int i, j;
	char szPos[40];
	struct Pto2D Pantalla[4];
	POINT list[4];
	POINT lispiso[4];
	HPEN hOrgPen;

	//calcula la distancia del objeto al ojo
	distance=distance+visual;

	if(distance<=0.0) 
		distance=0.5; 
	else if(distance>10.0)
		distance=10.0;

	//Pinta el piso
	//Verifica si el piso es visible
	if (Visible(l,Robot)) 
	{
		for( j=0; j < 4; j++ )
		{
			Persp_Point(Robot[l][j].x,Robot[l][j].y,Robot[l][j].z,
						&Pantalla[j].x, &Pantalla[j].y);
		}
		for(j=0; j<4; j++) 
		{
			lispiso[j].x=Pantalla[j].x;
			lispiso[j].y=Pantalla[j].y;
		}  

		hOrgPen = SelectObject(hdcBuffer,CreatePen(PS_SOLID,0,RGB(80,145,243)));

		MoveToEx(hdcBuffer,lispiso[0].x,lispiso[0].y,NULL);
		LineTo(hdcBuffer,lispiso[1].x,lispiso[1].y);
		MoveToEx(hdcBuffer,lispiso[1].x,lispiso[1].y,NULL);
		LineTo(hdcBuffer,lispiso[2].x,lispiso[2].y);
		MoveToEx(hdcBuffer,lispiso[2].x,lispiso[2].y,NULL);
		LineTo(hdcBuffer,lispiso[3].x,lispiso[3].y);
		MoveToEx(hdcBuffer,lispiso[3].x,lispiso[3].y,NULL);
		LineTo(hdcBuffer,lispiso[0].x,lispiso[0].y);
		
		DeleteObject(hOrgPen);
	}
		

	for(i=a; i<b; i++)
	{
		for( j=0; j < 4; j++ )
		{
			Persp_Point( Robot[i][j].x, Robot[i][j].y, Robot[i][j].z,
						&Pantalla[j].x, &Pantalla[j].y);
		}
		for(j=0; j<4; j++) 
		{
			list[j].x=Pantalla[j].x;
			list[j].y=Pantalla[j].y;
		}

		hOrgPen = SelectObject(hdcBuffer,CreatePen(PS_SOLID,0,RGB(200,200,200)));	
		MoveToEx(hdcBuffer,list[0].x,list[0].y,NULL);
		LineTo(hdcBuffer,list[1].x,list[1].y);
		MoveToEx(hdcBuffer,list[1].x,list[1].y,NULL);
		LineTo(hdcBuffer,list[2].x,list[2].y);
		MoveToEx(hdcBuffer,list[2].x,list[2].y,NULL);
		LineTo(hdcBuffer,list[3].x,list[3].y);
		MoveToEx(hdcBuffer,list[3].x,list[3].y,NULL);
		LineTo(hdcBuffer,list[0].x,list[0].y);

		DeleteObject(hOrgPen);
	} 

	if(IsTableEnabled)
	{
		//pinta la mesa
		for( i=0; i<22; i++ )
		{
			for( j=0; j < 4; j++ )
			{
				Persp_Point(table3d[i][j].x,table3d[i][j].y,table3d[i][j].z,
							&Pantalla[j].x, &Pantalla[j].y);
			}
			for(j=0; j<4; j++) 
			{
				list[j].x=Pantalla[j].x;
				list[j].y=Pantalla[j].y;
			}	

			hOrgPen = SelectObject(hdcBuffer,CreatePen(PS_SOLID,0,RGB(0,250,0)));	
			MoveToEx(hdcBuffer,list[0].x,list[0].y,NULL);
			LineTo(hdcBuffer,list[1].x,list[1].y);
			MoveToEx(hdcBuffer,list[1].x,list[1].y,NULL);
			LineTo(hdcBuffer,list[2].x,list[2].y);
			MoveToEx(hdcBuffer,list[2].x,list[2].y,NULL);
			LineTo(hdcBuffer,list[3].x,list[3].y);
			MoveToEx(hdcBuffer,list[3].x,list[3].y,NULL);
			LineTo(hdcBuffer,list[0].x,list[0].y);
			DeleteObject(hOrgPen);
		}

		//pinta el cubo
		for( i=0; i<6; i++ )
		{
			for( j=0; j < 4; j++ )
			{	
				Persp_Point(cube3d[i][j].x,cube3d[i][j].y,cube3d[i][j].z,
							&Pantalla[j].x, &Pantalla[j].y);
			}
			for(j=0; j<4; j++) 
			{
				list[j].x=Pantalla[j].x;
				list[j].y=Pantalla[j].y;
			}

			hOrgPen = SelectObject(hdcBuffer,CreatePen(PS_SOLID,0,RGB(250,0,0)));	
			MoveToEx(hdcBuffer,list[0].x,list[0].y,NULL);
			LineTo(hdcBuffer,list[1].x,list[1].y);
			MoveToEx(hdcBuffer,list[1].x,list[1].y,NULL);
			LineTo(hdcBuffer,list[2].x,list[2].y);
			MoveToEx(hdcBuffer,list[2].x,list[2].y,NULL);
			LineTo(hdcBuffer,list[3].x,list[3].y);
			MoveToEx(hdcBuffer,list[3].x,list[3].y,NULL);
			LineTo(hdcBuffer,list[0].x,list[0].y);
			DeleteObject(hOrgPen);
		}
	}

	//pone el mensaje del segundo segmento del SB
	sprintf(szPos,"x=%.1f, y=%.1f, z=%.1f",
			FinalPos.x,FinalPos.z,FinalPos.y);
	SendMessage(hwndST,SB_SETTEXT,1,(LPARAM)szPos);

	//Se copia bit a bit el contenido del Doble buffer a la memoria
	//de video
	BitBlt(hdc,0,0,cx,cy,hdcBuffer,0,0,SRCCOPY);
}

/*------------------------------------------------------------
	Visible: determina si un plano es visible usando el 
	metodo de la normal
  ------------------------------------------------------------*/
BOOL Visible(int Plano,struct Pto3D Object[][4])
{
	struct Pto3D V1, V2, Normal ;

	V1.x = Object[Plano][1].x - Object[Plano][0].x ;
	V1.y = Object[Plano][1].y - Object[Plano][0].y ;
	V1.z = Object[Plano][1].z - Object[Plano][0].z ;
	V2.x = Object[Plano][2].x - Object[Plano][0].x ;
	V2.y = Object[Plano][2].y - Object[Plano][0].y ;
	V2.z = Object[Plano][2].z - Object[Plano][0].z ;
	Normal.x = V1.y*V2.z - V1.z*V2.y ;
	Normal.y = V1.z*V2.x - V1.x*V2.z ;
	Normal.z = V1.x*V2.y - V1.y*V2.x ;
	if (Normal.z <= 0.0)
		return(TRUE);
	else
		return(FALSE);
}

/*------------------------------------------------------------
	CamaraAA: Acerca y Aleja la camara sobre el eje Z
  ------------------------------------------------------------*/
void CamaraAA(int op,HDC hdc)
{ 
	if(op==2)
		//Acercar
		visual=0.5;
	else if(op==1) 
		//Alejar
		visual=-0.5;
	DrawRobot(hdc);  
	visual=0.0;
}

/*------------------------------------------------------------
	RestauraVpE: Restaura el vector de posicion de las piezas
	en caso de que estas se hayan movido para colocar una 
	nueva pieza
  ------------------------------------------------------------*/
void RestauraVpE()
{
	int i=0;
	if(mover==1 || borrar==1)
	{ 
		dd=0;
		for(i=0; i<indice-1; i++)
		{
			CopiaDatos(ordenr[i][0]);
			VectPos1[i].x=VectPosF[i].x;
			VectPos1[i].y=VectPosF[i].y;
			VectPos1[i].z=VectPosF[i].z;
			Eje1[i].x=EjeF[i].x;
			Eje1[i].y=EjeF[i].y;
			Eje1[i].z=EjeF[i].z;    
		}
		VectPos1[i].x=VectPosF[i].x;
		VectPos1[i].y=VectPosF[i].y;
		VectPos1[i].z=VectPosF[i].z; 
	}
	if(borrar==1)
		borrar=0;
	mover=0;
}

/*------------------------------------------------------------
	creaorden: crea el orden de las piezas
  ------------------------------------------------------------*/
void creaorden(int n)
{  
	char *szPart;
	int i;
	HMENU hmenu;

	hmenu=GetSubMenu(GetMenu(hwnd),4);
	i=ObjMenuCount-300;
	switch(n)
	{
		case 1:
		case 11:
			szPart="Cubo";
			break;
		case 2:
		case 12:
			szPart="Base";
			break;
		case 3:
		case 13:
			szPart="Cilindro";
			break;
		case 4:
		case 14:
			szPart="Extensión";
			break;
		case 5:
		case 15:
			szPart="Mano";
			break;
		case 6:
		case 16:
			szPart="Segmento 1";
			break;
		case 7:
		case 17:
			szPart="Soporte";
			break;
		case 8:
		case 18:
			szPart="Segmento 2";
			break;
		case 9:
		case 19:
			szPart="Comp. Mano";
			break;
		case 10:
		case 20:
			szPart="Barra";
			break;
		default:
			break;
	}


	if( ((ColP[n-1].y - ColP[n-1].x) + dd) <= NumPlaTot )
	{
		if (indice-1==0) 
		{
			ordenr[0][0]=n;
			ordenr[0][1]=cc++;
			indice++;      
		}
		else 
		{
			ordenr[indice-1][0]=n;
			ordenr[indice-1][1]=cc++;
			indice++;   
		}

		CopiaDatos(n);
		ObjMenuCount++;
		AppendMenu(hmenu,MF_STRING,ObjMenuCount,szPart);
	}
	else 
	{
		szMsg="No se puede almacenar la pieza";
		SendMessage (hwnd, WM_COMMAND, MESSAGE, 0L);
	}
}

/*------------------------------------------------------------
	CopiaDatos: copia los datos del robot
	a una estructura de datos en memoria
  ------------------------------------------------------------*/
void CopiaDatos(int n)
{
	int c=0,ini,fin;

	ini=ColP[n-1].x;
	fin=ColP[n-1].y;

	if(n==5 || n==15)
	{
		mano=dd;
		pinza=n;
	}

	for( i=ini; i<fin; i++ )
	{
		for( j=0; j< 4; j++ )
		{
			Datos[c+dd][j].x = DatosO[i][j].x;
			Datos[c+dd][j].y = DatosO[i][j].y;
			Datos[c+dd][j].z = DatosO[i][j].z;
		}  
		c++;
	}

	OrdenDatos[indice-2][0]=dd;
	OrdenDatos[indice-2][1]=dd+(fin-ini);
	dd+=c;
}

/*------------------------------------------------------------
	borraorden: sirve para eliminar piezas del robot
  ------------------------------------------------------------*/
void borraorden()
{
	int i=0, ii=0;
	HMENU hmenu,hmenu2;

	hmenu2=GetMenu(hwnd);
	hmenu=GetSubMenu(hmenu2,4);
	
	if (indice<=1)
	{ 
		//szMsg="No hay piezas que borrar";
		//SendMessage (hwnd, WM_COMMAND, MESSAGE, 0L);
		//amensaje();
		//outtextxy(50,RESY-16,"No hay piezas que borrar. De ENTER para Continuar...");
		//recibetecla(); 
	}
	else
	{
		if(ordenr[indice-2][0]==10 || ordenr[indice-2][0]==20)
		{ 
			EnableMenuItem(hmenu2,ID_SIMULADOR_DESLIZAR,MF_GRAYED);
			nbarra=0;
			for(i=0; i<indice; i++)
				if(ordenr[i][0]==10 || ordenr[i][0]==20) 
					nbarra=i; 
				barraab[nbarra]=0; 
		}
		if(ordenr[indice-2][0]==5 || ordenr[indice-2][0]==15)
		{
			EnableMenuItem(hmenu2,ID_SIMULADOR_ABRIRPINZA,MF_GRAYED);
			EnableMenuItem(hmenu2,ID_SIMULADOR_CERRARPINZA,MF_GRAYED);
		}
		ordenr[indice-2][0]=0;
		ordenr[indice-2][1]=0;
		VectPos1[indice-1].x=0.0;  
		VectPosF[indice-1].x=0.0;
		VectPos1[indice-1].y=0.0;  
		VectPosF[indice-1].y=0.0;
		VectPos1[indice-1].z=0.0;  
		VectPosF[indice-1].z=0.0;
		Eje1[indice-2].x=0.0;      
		EjeF[indice-2].x=0.0;
		Eje1[indice-2].y=0.0;      
		EjeF[indice-2].y=0.0;
		Eje1[indice-2].z=0.0;      
		EjeF[indice-2].z=0.0;
		indice-=1; 
		cc--;
		while(i!=VecArr)
		{
			if(VectPosA[i].w==ordenr[indice-2][0])
				PositionVector(ordenr[indice-2][0],indice-2,1,0); 
			i++; 
			ii=1;
		}
		if(ii!=1)
		{
			i=0;
			while(i!=VecDer)
			{  /*VecArr*/
				if(VectPosD[i].w==ordenr[indice-2][0]) /* VectPosA*/
					PositionVector(ordenr[indice-2][0],indice-2,2,0); 
				i++;
			}
		}
		if(indice==1)
		{
			dd=0;
			l=0;
			Mat_Identity_4x4(V);
			Mat_Identity_4x4(Vaux);
			RotateCamX(90);
			for(i=0; i<50; i++)
				barraab[i]=0; 
		}
		else 
		{
			dd= OrdenDatos[indice-2][1];
			l= OrdenDatos[indice-2][1];
			OrdenDatos[indice-1][0]=0;
			OrdenDatos[indice-1][1]=0; 
		}
		mano=0;
		DeleteMenu(hmenu,ObjMenuCount,MF_STRING);
		ObjMenuCount--;
	}
}

/*------------------------------------------------------------
	DrawRobot: trivial
  ------------------------------------------------------------*/
void DrawRobot(HDC hdc)
{
	double x1,y1,z1;
	int x2,y2,i,pos=0;
	int a;

	if(indice-1!=0)
		for(i=0;i<indice-1;i++) 
		{
			x2=OrdenDatos[i][0];
			y2=OrdenDatos[i][1];
			if(ordenr[i][1]==0) 
				pos=0;
			RobotAssembly( x2, y2, ordenr[i][1], 0, pos );
			pos=pos+(y2-x2);
			l=pos;                
		}
	else  
		l=0;

	PonePiso();
	if(indice==1) 
		pp=0;
	else
	{
		if(distance<=3) 
			pp=1; 
		else 
			pp=0; 
	}

	for( i=0; i<l+pp; i++)
	{
		for( j=0; j< 4; j++ )
		{
			//Fake Shadows:
			//Se proyectan los planos del robot contra el piso
			//generalmente Y=0, pero en este caso el piso no 
			//es Y=0 sino Z=0 asi que solo se cambia Y por Z
			//Se divide entre Z, asi que mientras mas alto
			//este el objeto, mas pequeña la sombra...
			srx = Robot[i][j].x - (Robot[i][j].z/light.z)*light.x;
			sry = Robot[i][j].y - (Robot[i][j].z/light.z)*light.y;
			//Se multiplican los vertices de la sombra por la
			//matriz de vision para calcular su posicion
			//respecto a la camara de vision
			SRx[i][j] = V[1][1]*srx + V[1][2]*sry;
			SRy[i][j] = V[2][1]*srx + V[2][2]*sry;

			//Se multiplican los vertices del robot por la matriz
			//de vision tambien
			x1= V[1][1]*Robot[i][j].x + V[1][2]*Robot[i][j].y + V[1][3]*Robot[i][j].z;
			y1= V[2][1]*Robot[i][j].x + V[2][2]*Robot[i][j].y + V[2][3]*Robot[i][j].z;
			z1= V[3][1]*Robot[i][j].x + V[3][2]*Robot[i][j].y + V[3][3]*Robot[i][j].z;
			Robot[i][j].x = x1;
			Robot[i][j].y = y1;
			Robot[i][j].z = z1;
		}
	}

	for(i=0; i<l; i++)
	{
		for(j=0; j<4; j++)
		{
			Objects[i][j].z = Robot[i][j].z;
		}
		Objects[i][0].type = 0;
		Objects[i][0].plane = i;
	}

	a=l;

	if(IsTableEnabled)
	{
		for(i=0; i<22; i++)
		{
			for(j=0; j<4; j++)
			{
				stx = (Table[i][j].x + VectPosMesa.x) - 
					  ((Table[i][j].z+VectPosMesa.z)/light.z)*light.x;
				sty = (Table[i][j].y + VectPosMesa.y) - 
					  ((Table[i][j].z+VectPosMesa.z)/light.z)*light.y;
				STx[i][j] = V[1][1]*stx + V[1][2]*sty;
				STy[i][j] = V[2][1]*stx + V[2][2]*sty;

				table3d[i][j].x= V[1][1]*(Table[i][j].x + VectPosMesa.x) +
								 V[1][2]*(Table[i][j].y + VectPosMesa.y) + 
								 V[1][3]*(Table[i][j].z + VectPosMesa.z);
				table3d[i][j].y= V[2][1]*(Table[i][j].x + VectPosMesa.x) +
								 V[2][2]*(Table[i][j].y + VectPosMesa.y) +
								 V[2][3]*(Table[i][j].z + VectPosMesa.z);
				table3d[i][j].z= V[3][1]*(Table[i][j].x + VectPosMesa.x) +
								 V[3][2]*(Table[i][j].y + VectPosMesa.y) +
								 V[3][3]*(Table[i][j].z + VectPosMesa.z);
				Objects[a][j].z=table3d[i][j].z;
			}
				Objects[a][0].type = 1;
				Objects[a][0].plane = i;
				a++;
		}

		for(i=0; i<6; i++)
		{
			for(j=0; j<4; j++)
			{
				scx = (CubeAux[i][j].x + VectPosCubo.x) - 
					  ((CubeAux[i][j].z+VectPosCubo.z)/light.z)*light.x;
				scy	= (CubeAux[i][j].y + VectPosCubo.y) - 
					  (CubeAux[i][j].z/light.z)*light.y;
				SCx[i][j] = V[1][1]*scx + V[1][2]*scy;
				SCy[i][j] = V[2][1]*scx + V[2][2]*scy;

				cube3d[i][j].x= V[1][1]*(CubeAux[i][j].x + VectPosCubo.x) +
							    V[1][2]*(CubeAux[i][j].y + VectPosCubo.y) +
								V[1][3]*(CubeAux[i][j].z + VectPosCubo.z);
				cube3d[i][j].y= V[2][1]*(CubeAux[i][j].x + VectPosCubo.x) +
								V[2][2]*(CubeAux[i][j].y + VectPosCubo.y) +
								V[2][3]*(CubeAux[i][j].z + VectPosCubo.z);
				cube3d[i][j].z= V[3][1]*(CubeAux[i][j].x + VectPosCubo.x) +
								V[3][2]*(CubeAux[i][j].y + VectPosCubo.y) +
								V[3][3]*(CubeAux[i][j].z + VectPosCubo.z);
				Objects[a][j].z=cube3d[i][j].z;
			}
				Objects[a][0].type = 2;
				Objects[a][0].plane = i;
				a++;
		}
	}

	if(WireFrame)
		DrawPlaneWire(0,l,hdc);
	else
	{
		OrdenaPlanos(0,a,Order,Objects);
		DrawPlane(0,a,hdc);
	}
}

/*------------------------------------------------------------
	PonePiso: trivial
  ------------------------------------------------------------*/
void PonePiso()
{
	int i=0,auxx,auxz,xz=0;

	while(i<nfigx)
	{
		if(ordenr[0][0]==pisox[i].x)
		{  
			auxx=pisox[i].y;  
			xz=1;
			for( j=0; j< 4; j++ ) 
			{
				Robot[l][j].x = auxx ;
				Robot[l][j].y = Piso1[j].y ;
				Robot[l][j].z = Piso1[j].z ;        
			} 
		}
		i++; 
	}

	if(xz==0) 
	{
		i=0;
		while(i<nfigz)
		{
			if(ordenr[0][0]==pisoz[i].x)
			{  
				auxz=pisoz[i].y;
				for( j=0; j< 4; j++ ) 
				{
					Robot[l][j].x = Piso[j].x ;
					Robot[l][j].y = Piso[j].y ;
					Robot[l][j].z = auxz ;        
				}  
			}
			i++; 
		}  
	}
}

/*------------------------------------------------------------
	PositionVector: Calcula el vector de posicion de una figura
  ------------------------------------------------------------*/
void PositionVector(int nfig, int vcor, int lugar, int h)
{
	int i=0, entro=0;

	if(lugar==1) 
		while(i!=VecArr)
		{
			if(VectPosA[i].w==nfig) 
			{
				VectPos1[vcor+1].x=VectPosA[i].x;  
				VectPosF[vcor+1].x=VectPosA[i].x;
				VectPos1[vcor+1].y=VectPosA[i].y;  
				VectPosF[vcor+1].y=VectPosA[i].y;
				VectPos1[vcor+1].z=VectPosA[i].z;  
				VectPosF[vcor+1].z=VectPosA[i].z;
				Eje1[vcor].x=Eje[i].x;	     
				EjeF[vcor].x=Eje[i].x;
				Eje1[vcor].y=Eje[i].y;             
				EjeF[vcor].y=Eje[i].y;
				Eje1[vcor].z=Eje[i].z;	     
				EjeF[vcor].z=Eje[i].z;
			}   
			i++;
		}

	if(lugar==2 && h==4) 
	{
		i=0; 
		while(i!=VecArr)
		{
			if(VectPosA[i].w==nfig) 
			{
				VectPos1[vcor+1].x=VectPosA[i].x;  
				VectPosF[vcor+1].x=VectPosA[i].x;
				VectPos1[vcor+1].y=VectPosA[i].y;  
				VectPosF[vcor+1].y=VectPosA[i].y;
				VectPos1[vcor+1].z=VectPosA[i].z;  
				VectPosF[vcor+1].z=VectPosA[i].z;
				Eje1[vcor].x=Eje[i].x;	     
				EjeF[vcor].x=Eje[i].x;
				Eje1[vcor].y=Eje[i].y;             
				EjeF[vcor].y=Eje[i].y;
				Eje1[vcor].z=Eje[i].z;	     
				EjeF[vcor].z=Eje[i].z;
				entro=1;  
			}   
			i++;
		} 
		i=0;
		if (entro==0)  
			while(i!=VecDer)
			{
				if(VectPosD[i].w==nfig) 
				{
					VectPos1[vcor+1].x=VectPosD[i].x;  
					VectPosF[vcor+1].x=VectPosD[i].x;
					VectPos1[vcor+1].y=VectPosD[i].y;  
					VectPosF[vcor+1].y=VectPosD[i].y;
					VectPos1[vcor+1].z=VectPosD[i].z;  
					VectPosF[vcor+1].z=VectPosD[i].z;
					Eje1[vcor].x=EjeD[i].x;   
					EjeF[vcor].x=EjeD[i].x;
					Eje1[vcor].y=EjeD[i].y;   
					EjeF[vcor].y=EjeD[i].y;
					Eje1[vcor].z=EjeD[i].z;   
					EjeF[vcor].z=EjeD[i].z;
				}    
				i++;
			}
	}
	else if(lugar==2) 
		while(i!=VecDer)
		{
			if(VectPosD[i].w==nfig) 
			{
				VectPos1[vcor+1].x=VectPosD[i].x;  
				VectPosF[vcor+1].x=VectPosD[i].x;
				VectPos1[vcor+1].y=VectPosD[i].y;  
				VectPosF[vcor+1].y=VectPosD[i].y;
				VectPos1[vcor+1].z=VectPosD[i].z;  
				VectPosF[vcor+1].z=VectPosD[i].z;
				if(h!=3)
				{
					Eje1[vcor].x=EjeD[i].x;   
					EjeF[vcor].x=EjeD[i].x;
					Eje1[vcor].y=EjeD[i].y;   
					EjeF[vcor].y=EjeD[i].y;
					Eje1[vcor].z=EjeD[i].z;   
					EjeF[vcor].z=EjeD[i].z;
				}    
			}
			i++;
		}
}

/*------------------------------------------------------------
	OpenCloseHand: Abre y Cierra la pinza
  ------------------------------------------------------------*/
void OpenCloseHand( int oper, int mob , int numb, HDC hdc)
{
	int Dx=0, c=0, npaux;
	if(mob==1) 
	{ 
		//Para las pinzas
		//Abrir pinza
		if((oper==1)&&(ManoAbierta==0)) 
		{
			if(IsCubeTaken)
			{
				if(LetCube())
				{
					Dx=6;
					IsCubeTaken=FALSE;
					ManoAbierta=1;
				}
				else
				{
					Dx=0;
					ManoAbierta=0;
				}
			}
			else
			{
				Dx=30;
				ManoAbierta=1;
			}
		}
		//Cerrar pinza
		else 
		if((oper==0)&&(ManoAbierta==1))
		{
			//Si se puede tomar el cubo...
			if(TakeCube())
			{
				Dx=-6;
				IsCubeTaken = TRUE;
				ManoAbierta=0;
			}
			else
			{
				Dx=-30;
				ManoAbierta=0;
			}
		}
		else
		    Dx=0;

		for( i=ColP[pinza].x; i < (ColP[pinza].x+6); i++ )
		{
			for( j=0; j< 4; j++ )
			{
				Datos[6+mano+c][j].x = Datos[6+mano+c][j].x;
				Datos[6+mano+c][j].y = Datos[6+mano+c][j].y + Dx;
				Datos[6+mano+c][j].z = Datos[6+mano+c][j].z;
			}
			c++;
		}
		c=0;

		for( i=ColP[pinza].x+6; i < (ColP[pinza].x+12); i++ )
		{
			for( j=0; j< 4; j++ )
			{
				Datos[12+mano+c][j].x = Datos[12+mano+c][j].x;
				Datos[12+mano+c][j].y = Datos[12+mano+c][j].y - Dx;
				Datos[12+mano+c][j].z = Datos[12+mano+c][j].z;
			} 
			c++;
		}
	}
	else 
	{ 
		//Para las barras
		if(ordenr[numb][0]==10 || ordenr[numb][0]==20)
		{
			npaux=OrdenDatos[numb][0];
			if (barraab[numb]==0)
			{
				Dx=100;
			}
			else if (barraab[numb]==1) 
			{
				Dx=-100;
			}

			for( i=npaux+6; i < l; i++ )
			{
				for( j=0; j< 4; j++ )
				{
					Datos[i][j].x = Datos[i][j].x + Dx;
					Datos[i][j].y = Datos[i][j].y;
					Datos[i][j].z = Datos[i][j].z;
				}
			}
			if(barraab[numb]==0)
				barraab[numb]=1;
			else
				barraab[numb]=0;
		}

	}
	DrawRobot(hdc);
}

/*------------------------------------------------------------
	Embonar: Determina si las piezas embonan
  ------------------------------------------------------------*/
int embonar(int a, int b, int POS)
{
	if(indice!=1)
	{
		//Si se coloca arriba checa propiedades top,bottom
		if(POS == UP)
			if ( (OP[a-1].top == FLAT && OP[b-1].bottom == FLAT) ||
				(OP[a-1].top == CONVEX && OP[b-1].bottom == CONCAVE) ||
				(OP[a-1].top == CONCAVE && OP[b-1].bottom == CONVEX) )
				return 1;
			else 
				return 0;
		
		//Si se coloca a la derecha checa propiedades left,right
		else if (POS == RIGHT)
			if ( (OP[a-1].right == FLAT && OP[b-1].left == FLAT) ||
				(OP[a-1].right == CONVEX && OP[b-1].left == CONCAVE) ||
				(OP[a-1].right == CONCAVE && OP[b-1].left == CONVEX) )
				return 1;
			else 
				return 0;
	}
	else
		return 1;
}

/*------------------------------------------------------------
	PlacePart: Coloca una pieza
  ------------------------------------------------------------*/
void PlacePart(int fig,int opp, HDC hdc)
{ 
	int n;

	if(indice==1)
	{
       if(fig!=1 && fig!=4 && fig!=10) 
	   {
			if(opp==UP)
			{ 
				creaorden(fig);
				PositionVector(fig, indice-2, 1,0);  
			}
			else if(opp==RIGHT)
			{ 
				creaorden(fig+NumPiezas); 
				PositionVector(fig+NumPiezas, indice-2, 2,0);
			}  
	   }
       else if(fig==4) 
	   { 
			creaorden(fig);
			PositionVector(fig, indice-2,2,0);   
	   }
       else 
	   { 
			creaorden(fig);
			PositionVector(fig, indice-2,1,0);  
	   }
       DrawRobot(hdc);  
	}
    else 
	{ 
		if(embonar(ordenr[indice-2][0], fig,opp)!=1)
			if(embonar(ordenr[indice-2][0], fig+NumPiezas,opp)==1) 
			{
				fig+=10;
			}
			else 
				n=4;

		if(n!=4)
			if(ordenr[indice-2][0]==2 || ordenr[indice-2][0]==5 ||ordenr[indice-2][0]==7)
				n=1;
			else if(ordenr[indice-2][0]==1 || ordenr[indice-2][0]==10 || ordenr[indice-2][0]==11 || ordenr[indice-2][0]==20)
				n=2;
			else 
				n=3;
		switch(n)
		{
			case 1: 
				creaorden(fig);
				PositionVector(fig, indice-2, 1,0);
				DrawRobot(hdc);
				break;
			case 2: 
				if(fig>10)
					fig-=10;
				if(opp==UP)
				{ 
					creaorden(fig);
					PositionVector(fig, indice-2, 1,0);  
				}
				else if(opp==RIGHT)
				{ 
					creaorden(fig+NumPiezas); 
					PositionVector(fig+NumPiezas, indice-2, 2,4);
					PositionVector(ordenr[indice-3][0], indice-3, 2,3);  
				}
				DrawRobot(hdc);
				break;
			case 3: 
				creaorden(fig);
				PositionVector(fig, indice-2, 2,4);
				DrawRobot(hdc);
				break;
			case 4: 
				szMsg="Las piezas no embonan";
				SendMessage (hwnd, WM_COMMAND, MESSAGE, 0L);
				break;
		}
	}//if TABLE
}

/*------------------------------------------------------------
	deslizar: Mueve la barra deslizante
  ------------------------------------------------------------*/
void deslizar()
{ 
	nbarra=0;
	for(i=0; i<indice; i++)
		if(ordenr[i][0]==10 || ordenr[i][0]==20) 
			nbarra+=1;
	if(nbarra!=0)
	{
		//ponebotonesRot(0,1);
		//barrasmasmenos(); 
	}
	else 
	{
		//bdoble(5);}
		//borrapantalla(RESX-5,60,RESX-97,RESY-100,3);
		//figmym();
		//bdoble(7);
	}
}

/*------------------------------------------------------------
	PopFileInitialize: inicializa parametros necesarios
	para el manejo de archivos bajo win32
  ------------------------------------------------------------*/
void PopFileInitialize (HWND hwnd)
{
	static char szFilter[] = "EDISIM (*.ROB)\0*.rob\0\0";

	ofn.lStructSize       = sizeof (OPENFILENAME);
    ofn.hwndOwner         = hwnd;
    ofn.hInstance         = NULL;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = NULL;        
    ofn.nMaxFile          = _MAX_PATH;
    ofn.lpstrFileTitle    = NULL;
    ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = NULL;
    ofn.Flags             = 0;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "txt";
    ofn.lCustData         = 0L;
    ofn.lpfnHook          = NULL;
    ofn.lpTemplateName    = NULL;
}

/*------------------------------------------------------------
	PopFileOpenDlg: Muestra el dialogo de "AbrirArchivo"
  ------------------------------------------------------------*/
BOOL PopFileOpenDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
	ofn.hwndOwner         = hwnd;
    ofn.lpstrFile         = pstrFileName;
    ofn.lpstrFileTitle    = pstrTitleName;
    ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

    return GetOpenFileName (&ofn);
}

/*------------------------------------------------------------
	PopFileSaveDlg: Muestra el dialogo de "Guardar"
  ------------------------------------------------------------*/
BOOL PopFileSaveDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
    ofn.hwndOwner         = hwnd;
    ofn.lpstrFile         = pstrFileName;
    ofn.lpstrFileTitle    = pstrTitleName;
    ofn.Flags             = OFN_OVERWRITEPROMPT;

    return GetSaveFileName (&ofn) ;
}

/*------------------------------------------------------------
	PopFileRead: Lee un archivo del disco y crea las
	estructuras de datos necesarias
  ------------------------------------------------------------*/
BOOL PopFileRead (HWND hwnd, PSTR pstrFileName)
{
	FILE  *file;
	int i,j,d,in=0;
	char c;
	float f,v;

    if (NULL == (file = fopen (pstrFileName, "rt")))
         return FALSE;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
		{
			fscanf(file,"%e\n",&v);
			V[i][j]=v; 
		}
	fscanf(file,"%c\n",&c);
	fscanf(file,"%d\n",&in);
	c=fgetc(file);
	while(c!=EOF && i!=7)
	{
		if(c==':') 
		{
			for(i=0; i<in; i++)
			{
				fscanf(file,"%f\n",&f);
				VectPos1[i].x=f;
				VectPosF[i].x=f;
				fscanf(file,"%f\n",&f);
				VectPos1[i].y=f;
				VectPosF[i].y=f;
				fscanf(file,"%f\n",&f);
				VectPos1[i].z=f;
				VectPosF[i].z=f;  
			}
			for(i=0; i<in; i++)
			{
				fscanf(file,"%f\n",&f);
				Eje1[i].x=f;       
				EjeF[i].x=f;
				fscanf(file,"%f\n",&f);
				Eje1[i].y=f;       
				EjeF[i].y=f;
				fscanf(file,"%f\n",&f);
				Eje1[i].z=f;       
				EjeF[i].z=f;      
			}
			for(i=0; i<in-1; i++) 
			{
				fscanf(file,"%d\n",&d);
				creaorden(d); 
			}  
			i=7;
		}
		else 
		{
			i=7;
			fclose(file);
			return FALSE;
		}
		c=fgetc(file);
	}
	fclose(file);
    return TRUE;
}

/*------------------------------------------------------------
	PopFileWrite: Escribe los datos del robot en disco
  ------------------------------------------------------------*/
BOOL PopFileWrite (HWND hwndEdit, PSTR pstrFileName)
{
	FILE  *file;
	int i,j;

    if (NULL == (file = fopen (pstrFileName, "wt")))
        return FALSE;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			fprintf(file,"%e\n",V[i][j]);

	fprintf(file,">\n%d\n",indice);
	fprintf(file,":");

	for(i=0; i<indice; i++)
	{
		fprintf(file,"%f\n",VectPosF[i].x);
		fprintf(file,"%f\n",VectPosF[i].y);
		fprintf(file,"%f\n",VectPosF[i].z);
	}

	for(i=0; i<indice; i++)
	{
		fprintf(file,"%f\n",EjeF[i].x);
		fprintf(file,"%f\n",EjeF[i].y);
		fprintf(file,"%f\n",EjeF[i].z); 
	}

	for(i=0; i<indice-1; i++)
		fprintf(file,"%d\n",ordenr[i][0]);

    fclose (file);

    return TRUE;
}

/*------------------------------------------------------------
	Shading: obtiene la iluminacion de un plano con respecto 
	a la posicion del vector de iluminacion
  ------------------------------------------------------------*/
void Shading(int p,struct Pto3D Object[][4])
{
	struct Pto3D u,v,normal;
	float normal_length,dp;
	double Lx,Ly,Lz;

	//vector u
	u.x = Object[p][1].x - Object[p][0].x;
	u.y = Object[p][1].y - Object[p][0].y;
	u.z = Object[p][1].z - Object[p][0].z;
	
	//vector v
	v.x = Object[p][2].x - Object[p][0].x;
	v.y = Object[p][2].y - Object[p][0].y;
	v.z = Object[p][2].z - Object[p][0].z;

	//calcula la normal al poligono u x v
	normal.x = (u.y*v.z - u.z*v.y);
	normal.y =-(u.x*v.z - u.z*v.x);
	normal.z = (u.x*v.y - u.y*v.x);

	normal_length = (float)sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);

	//Rota la fuente de luz (light.x,light.y,light.z)
	//multiplicando por la matriz de vision V
	Lx= V[1][1]*light.x + V[1][2]*light.y + V[1][3]*light.z;
	Ly= V[2][1]*light.x + V[2][2]*light.y + V[2][3]*light.z;
	Lz= V[3][1]*light.x + V[3][2]*light.y + V[3][3]*light.z;

	//calcula el producto punto entre el vector de iluminacion 
	//y la normal a la superficie ya que 
	//          light_source . normal
	//Cos(t) = -----------------------
	//         |light_source|*|normal|
	//pero como ya esta normalizado el vector de la fuente de luz
	//solo dividimos entre la norma (o tamaño) de la normal
	//entre mas grande sea el angulo entre la normal y la fuente de
	//luz, mas obscuro es el plano (ie se aleja de la luz)
	dp = (float)((normal.x*Lx) + (normal.y*Ly) + (normal.z*Lz));
	
	//si la luz se refleja sobre la superficie
	if(dp>0)
	{
		//se usa una escala de 63 tonos de grises
		//se obtiene un numero del 0 al 63
		//0 = negro; 63 = blanco
		intensity = 63*dp/normal_length;
		
		//si 256/63 = 4  entonces se multiplica por 4
		//el factor de iluminacion para obtener el valor 
		//correspondiente RGB y se le suma el valor de la
		//luz ambiental
		intensity = (intensity*4) + AMBIENT_LIGHT;

		//si se pasa de 230...
		if (intensity > 220)
			intensity = 220;
	}
	//la luz no se refleja en la superficie
	else 
		intensity = AMBIENT_LIGHT;
}

/*------------------------------------------------------------
	DrawRobotSolid: Pinta el robot solido
  ------------------------------------------------------------*/
void DrawRobotSolid(int p)
{
	int j,I;
	struct Pto2D Pantalla[4];
	POINT list[4];
	HBRUSH hOrgBrush;
	HPEN hOrgPen;

	//Si el plano es visible, se pinta
	if (Visible(p,Robot))
	{
		//Calcula la iluminacion del plano
		Shading(p,Robot);

		//Pone la intensidad
		I=(int)(intensity);

		for( j=0; j < 4; j++ )
		{
			Persp_Point( Robot[p][j].x, Robot[p][j].y, Robot[p][j].z,
						&Pantalla[j].x, &Pantalla[j].y);
		}
		for(j=0; j<4; j++) 
		{
			list[j].x=Pantalla[j].x;
			list[j].y=Pantalla[j].y;
		}

		hOrgBrush = SelectObject(hdcBuffer,CreateSolidBrush(RGB(I,I,I)));
		hOrgPen = SelectObject(hdcBuffer,GetStockObject(BLACK_PEN));
		Polygon(hdcBuffer,list,4);

		DeleteObject(hOrgBrush);
		DeleteObject(hOrgPen);
	}//if Visible
}

/*------------------------------------------------------------
	DrawTableSolid: Pinta la mesa solida
  ------------------------------------------------------------*/
void DrawTableSolid(int p)
{
	int j,I;
	struct Pto2D Pantalla[4];
	POINT list[4];
	HBRUSH hOrgBrush;
	HPEN hOrgPen;

	if (Visible(p,table3d))
	{
		//Calcula la iluminacion del plano
		Shading(p,table3d);

		//Pone la intensidad
		I=(int)(intensity);

		for( j=0; j < 4; j++ )
		{
			Persp_Point(table3d[p][j].x,table3d[p][j].y,table3d[p][j].z,
						&Pantalla[j].x, &Pantalla[j].y);
		}
		for(j=0; j<4; j++) 
		{
			list[j].x=Pantalla[j].x;
			list[j].y=Pantalla[j].y;
		}	
		hOrgBrush = SelectObject(hdcBuffer,CreateSolidBrush(RGB(0,I,0)));
		hOrgPen = SelectObject(hdcBuffer, GetStockObject(BLACK_PEN) );
		Polygon(hdcBuffer, list, 4);

		DeleteObject(hOrgBrush);
		DeleteObject(hOrgPen);
	}
}

/*------------------------------------------------------------
	DrawCubeSolid: Pinta el cubo solido
  ------------------------------------------------------------*/
void DrawCubeSolid(int p)
{
	int j,I;
	struct Pto2D Pantalla[4];
	POINT list[4];
	HBRUSH hOrgBrush;
	HPEN hOrgPen;

	if (Visible(p,cube3d))
	{
		//Calcula la iluminacion del plano
		Shading(p,cube3d);

		//Pone la intensidad
		I=(int)(intensity);

		for( j=0; j < 4; j++ )
		{	
			Persp_Point(cube3d[p][j].x,cube3d[p][j].y,cube3d[p][j].z,
						&Pantalla[j].x, &Pantalla[j].y);
		}
		for(j=0; j<4; j++) 
		{
			list[j].x=Pantalla[j].x;
			list[j].y=Pantalla[j].y;
		}
		hOrgBrush = SelectObject(hdcBuffer,CreateSolidBrush(RGB(I,0,0)));
		hOrgPen = SelectObject(hdcBuffer, GetStockObject(BLACK_PEN) );
		Polygon(hdcBuffer, list, 4);

		DeleteObject(hOrgBrush);
		DeleteObject(hOrgPen);
	}
}

/*------------------------------------------------------------
	Translate: mueve los vertices de un objeto a otra posicion
  ------------------------------------------------------------*/
void Translate(double x,double y,double z,struct Pto3D Object[][4],
			   int np)
{
	int i,j;
	for(i=0; i<np; i++)
	{
		for(j=0; j<4; j++)
		{
			Object[i][j].x = Object[i][j].x + x;
			Object[i][j].y = Object[i][j].y + y;
			Object[i][j].z = Object[i][j].z + z;
		}
	}
}

/*------------------------------------------------------------
	TakeCube: Determina si se puede tomar el cubo
  ------------------------------------------------------------*/
BOOL TakeCube()
{
	double ortogonal=0.0, 
		   altura=0.0;

	ortogonal = (VectPerpenCubo.x * ex) +
				(VectPerpenCubo.y * ey) +
				(VectPerpenCubo.z * ez) ;

	altura = FinalPos.z - (VectPosCubo.z + VectAlturaCubo.z);

	if( (altura >=-14.0 && altura <=-7.0) &&
		(ortogonal >= -0.0001 && ortogonal <= 0.0001))
		return TRUE;
	else
		return FALSE;
}

/*------------------------------------------------------------
	LetCube: Determina si se puede soltar el cubo
  ------------------------------------------------------------*/
BOOL LetCube()
{
	if(IsCubeTaken)
	{
		if(VectPosCubo.z>=-1.0 && VectPosCubo.z<=3.0)
		{
			//Corrije la posicion del cubo
			//por si no es exactamente 0.0
			VectPosCubo.z=0.0;
			return TRUE;
		}
		else 
		if( (VectPosCubo.x >= -80 && VectPosCubo.x <=100) &&
			(VectPosCubo.y >= VectPosMesa.y-90 && VectPosCubo.y <= VectPosMesa.y+90) &&
			(VectPosCubo.z >=119 && VectPosCubo.z <= 123) )
		{
			//Corrije la posicion del cubo
			//por si no es exactamente 120.0
			VectPosCubo.z=120.0;
			return TRUE;
		}
		else 
			return FALSE;
	}
	else 
		return FALSE;
}

/*------------------------------------------------------------
	Collision: detecta colisiones entre una pieza y la mesa o 
	el piso
  ------------------------------------------------------------*/
BOOL Collision(int part)
{
	int i,j,k,count;
	double x,y,z;
	double dx,dy,dz;

	dx=dy=dz=0.0;

	//Aqui se obtiene el vector de posicion de la pieza que se 
	//movio
	for(k=0; k<part; k++)
	{
		dx += VectPos1[ordenr[k][1]].x;
		dy += VectPos1[ordenr[k][1]].y;
		dz += VectPos1[ordenr[k][1]].z;
	}

	//Recorre todos los vertices de las pieza que se mueven
	//y compara su posicion con la de la mesa y con el piso
	for(count=part; count<indice-1; count++)
	{
		dx += VectPos1[ordenr[count][1]].x;
		dy += VectPos1[ordenr[count][1]].y;
		dz += VectPos1[ordenr[count][1]].z;

		for(i=OrdenDatos[count][0]; i<OrdenDatos[count][1]; i++)
		{
			for(j=0; j<4; j++)
			{
				x = (Datos[i][j].x + dx)/2.0;
				y = (Datos[i][j].y + dy)/2.0;
				z = (Datos[i][j].z + dz)/2.0;

				//Siempre hace la prueba con el piso
				if(z<0.0)
					return TRUE;

				//si la mesa esta dibujada, hace la prueba con la
				//mesa tambien
				if(IsTableEnabled)
				{
					//Compara con la parte de arriba
					if(x>=VectPosMesa.x-100.0 &&
					   x<=VectPosMesa.x+100.0 &&
					   y>=VectPosMesa.y-100.0 &&
					   y<=VectPosMesa.y+100.0 &&
					   z<=120.0 && z>=100.0)
							return TRUE;
					//compara con las patas de la mesa
					else
					if(x<=VectPosMesa.x+100.0 &&
					   x>=VectPosMesa.x+80.0  &&
					   y<=VectPosMesa.y+100.0 &&
					   y>=VectPosMesa.y+80.0  &&
					   z<=120.0)
							return TRUE;
					else
					if(x<=VectPosMesa.x+100.0 &&
					   x>=VectPosMesa.x+80.0  &&
					   y>=VectPosMesa.y-100.0 &&
					   y<=VectPosMesa.y-80.0  &&
					   z<=120.0)
							return TRUE;
					else
					if(x>=VectPosMesa.x-100.0 &&
					   x<=VectPosMesa.x-80.0  &&
					   y<=VectPosMesa.y+100.0 &&
					   y>=VectPosMesa.y+80.0  &&
					   z<=120.0)
							return TRUE;
					else
					if(x>=VectPosMesa.x-100.0 &&
					   x<=VectPosMesa.x-80.0  &&
					   y>=VectPosMesa.y-100.0 &&
					   y<=VectPosMesa.y-80.0  &&
					   z<=120.0)
							return TRUE;
				}//if IsTableEnabled
			}//for j
		}//for i	
	}//for count

	return FALSE;
}


/*------------------------------------------------------------
	Demo: Realiza una demostracion del programa
  ------------------------------------------------------------*/
void Demo(HDC hdc)
{
	int i;
	RECT r;

	//area de dibujo
	r.top = 0;
	r.left= 0;
	r.right = cx;
	r.bottom= cy;

	/*
	//inicializa angulos de rotacion
	for(i=0; i<50; i++)
		RotAng[i]=0;

	//restaura vector de posicion
	RestauraVpE();
	*/

	iPart=2;
	PlacePart(iPart,UP,hdc);
	////Sleep(50);
	iPart=7;
	PlacePart(iPart,UP,hdc);
	////Sleep(50);
	iPart=3;
	PlacePart(iPart,UP,hdc);
	//Sleep(50);
	iPart=6;
	PlacePart(iPart,UP,hdc);
	//Sleep(50);
	iPart=8;
	PlacePart(iPart,UP,hdc);
	//Sleep(50);
	iPart=9;
	PlacePart(iPart,UP,hdc);
	//Sleep(50);
	iPart=5;
	PlacePart(iPart,RIGHT,hdc);
	//Sleep(50);
	OpenCloseHand(1,1,0,hdc);
	IsTableEnabled=TRUE;
	DrawRobot(hdc);
	//Sleep(50);
	//mover=1;
	for(i=0; i<10; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(3,2,hdc);
		//Sleep(50);
	}
	for(i=0; i<10; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(4,1,hdc);
		//Sleep(50);
	}
	for(i=0; i<10; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(5,1,hdc);
		//Sleep(50);
	}
	WireFrame=TRUE;
    FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
	DrawRobot(hdc);
	//Sleep(500);
	for(i=0; i<4; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateCamX(5);
		DrawRobot(hdc);
		//Sleep(50);
	}
	for(i=0; i<72; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateCamY(5);
		DrawRobot(hdc);
		//Sleep(50);
	}

    FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
	Mat_Identity_4x4(V);
	Mat_Identity_4x4(Vaux);
	RotateCamX(90);
	DrawRobot(hdc);
	//Sleep(500);
	WireFrame=FALSE;
    FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
	DrawRobot(hdc);
	//Sleep(250);
	for(i=0; i<4; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateCamX(5);
		DrawRobot(hdc);
		//Sleep(50);
	}
	for(i=0; i<60; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(1,1,hdc);
		//Sleep(50);
	}
	for(i=0; i<72; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateCamY(5);
		DrawRobot(hdc);
		//Sleep(50);
	}
	for(i=0; i<120; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(1,1,hdc);
		//Sleep(50);
	}
    FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
	Mat_Identity_4x4(V);
	Mat_Identity_4x4(Vaux);
	RotateCamX(90);
	DrawRobot(hdc);
	for(i=0; i<15; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(3,1,hdc);
		//Sleep(50);
	}
    FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
	RotateRobot(4,1,hdc);
	//Sleep(50);
	for(i=0; i<19; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(5,1,hdc);
		//Sleep(50);
	}
	OpenCloseHand(0,1,0,hdc);
	for(i=0; i<22; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(3,2,hdc);
		//Sleep(50);
	}
	for(i=0; i<18; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateCamY(5);
		DrawRobot(hdc);
		//Sleep(50);
	}
	for(i=0; i<45; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(1,2,hdc);
		//Sleep(50);
	}
	for(i=0; i<3; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(3,1,hdc);
		//Sleep(50);
	}
	for(i=0; i<2; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(4,1,hdc);
		//Sleep(50);
	}
	for(i=0; i<16; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(5,1,hdc);
		//Sleep(50);
	}
	OpenCloseHand(1,1,0,hdc);
	for(i=0; i<3; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateCamX(5);
		DrawRobot(hdc);
		//Sleep(50);
	}
	for(i=0; i<5; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(3,2,hdc);
		//Sleep(50);
	}
	for(i=0; i<20; i++)
	{
        FillRect(hdcBuffer,&r,GetStockObject(BLACK_BRUSH));
		RotateRobot(1,1,hdc);
		//Sleep(50);
	}
	OkMessage (hwnd,"Presione para continuar","Demo");
	IsTableEnabled=FALSE;
}
