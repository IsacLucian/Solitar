#include <iostream>
#include <fstream>
#include <graphics.h>
#include <math.h>
#include <dirent.h>
#include <time.h>   /// pentru random
#include <windows.h>
#include <mmsystem.h>   /// pentru muzica

using namespace std;

const int total = 8;    // dimensiunea matricei
int fills = 1;    // fill square
int emptys = 2;   // empty square
int width = 1350;
int height = 700;
int dim = 65;     // dimensiunea unui patrat
int R = 16;       // raza cerculi
int Rposibil = 8;
int lastcx = 1160 - dim/2, lastcy = 200 - dim / 2;
char current_folder[256];
bool page = 1, variabila_meniu = 0; /// 0 = meniu romana, 1 = meniu engleza
bool efect=1, muzica=1;

COLORREF ColorShadow = RGB(170, 0, 30);
COLORREF ActiveShadow = RGB(30, 150, 0);
COLORREF Color = RGB(200, 0, 30);     // culoarea cercului
COLORREF Active = RGB(0, 255, 0);     // culoarea Hover/Select cerc
COLORREF Jump = RGB(0, 255, 1);     // culoarea posibilitatii
COLORREF Create = RGB(105, 105, 105); // patrat de vreau sa l creez
COLORREF ClearColor = RGB(10, 10, 10);  // folosit pentru a curata un patrat din ecran
COLORREF Purple = RGB(107,1,180);
COLORREF PurpleShadow = RGB(77,0,75);
COLORREF Gold = RGB(255,223,0);
COLORREF GoldShadow = RGB(207,181,79);
COLORREF Blu = RGB(0, 50, 255);
COLORREF BluShadow = RGB(0,50,80);
COLORREF Red = RGB(200, 0, 30);
COLORREF RedShadow = RGB(150, 0, 30);
COLORREF Azure = RGB(0, 128, 255);
COLORREF Piesa = Color,Umbra = ColorShadow,Hover = Active,UmbraHover = ActiveShadow; ///Umbra inlocuieste ColorShadow, Piesa = Color, Hover = Active, UmbraHover = ActiveShadow

int dx[] = {-1, 1, 0, 0};     /// vectori de directie
int dy[] = {0, 0, 1, -1};

struct Moves
{
    int xi, yi, xf, yf, dir;
};

/**
    ISAC LUCIAN
*/

int GetX_coord(int x);
int GetY_coord(int y);
int GetX_mat(int x);
int GetY_mat(int y);
int NoPossibleMoves(int tabla[][total]);
int CheckWin(int tabla[][total], int endx, int endy);
void Posiblemove(int px[], int py[], int lg);
void ClearPosiblemove(int px[], int py[], int lg);
void Interact(int tabla[][total]);
void Start(char loc[]);
void DrawCircle(int x, int y, int r, COLORREF c);
void Clear(int xi, int yi, int xf, int yf);
void MovePiece(int tabla[][total], int from_x, int from_y, int to_x, int to_y, int dir);
void Undo(int tabla[][total], Moves a[], int &n);
void CreateTable();
void SelectLvl();
void ShuffleMove(int tabla[][total], int maxim, Moves m[], int &n);
void UpdateTime(int ms);
void UpdateMoves(Moves a[], int n);
void Fill(int tabla[][total], int x, int y);
inline bool Inmat(int x, int y);
inline double dist(int xi, int yi, int xf, int yf);

//////////////////////////////////

/**
    ALEX OBREJA
*/

void Meniu();
inline void Meniu_Romana(int i);
inline void Meniu_Engleza(int i);
void Meniu_Highlight(int meniu,int &schimbare);
void Interactiune_Meniu();
void Setari(int &culoarepiesa,int &culoarehover);
void Setari_Romana(int culoarepiesa, int culoarehover);
void Setari_Engleza(int culoarepiesa, int culoarehover);
void PiesaGlobala(int x,int y,int &culoarepiesa);
void HoverGlobal(int x,int y,int &culoarehover); ///culoarea unei piese cand trec peste ea
void Reguli();
void Reguli_Romana();
void Reguli_Engleza();
void Sageata();
inline void Highlight(char cuv[],int x,int y,int font,unsigned color);

/**
    distanta intre 2 puncte
*/
inline double dist(int xi, int yi, int xf, int yf)
{
    return sqrt((xi - xf) * (xi - xf) + (yi - yf) * (yi - yf));
}

/**
    transform din coordonata xOy in coordonata din matrice
*/
int GetX_coord(int x)
{
    int startx = width / 2 - dim * total / 2 - dim;
    int endx = startx + 9 * dim;
    if(x < startx || x > endx) return -1;

    return (x - startx) / dim;
}

int GetY_coord(int y)
{
    int starty = height / 2 - dim * total / 2 - dim;
    int endy = starty + 9 * dim;

    if(y < starty || y > endy) return -1;

    return (y - starty) / dim;
}

/**
    transform din coordonata din matrice in coordonata xOy
*/

int GetX_mat(int x)
{
    int startx = width / 2 - dim * total / 2;

    return startx + dim * x;
}

int GetY_mat(int y)
{
    int starty = height / 2 - dim * total / 2;

    return starty + dim * y;
}

/**
    verific daca pozitia este in matrice
*/

inline bool Inmat(int x, int y)
{
    return x >= 0 && x < total && y >= 0 && y < total;
}

/**
    marchez posibilele pozitii
*/

void Posiblemove(int px[], int py[], int lg)
{
    for(int i = 0; i < lg; i++)
    {
        int midx = GetX_mat(px[i]) + dim / 2;
        int midy = GetY_mat(py[i]) + dim / 2;

        if(i == lg - 1)
        {
            DrawCircle(midx - 2, midy + 2, R, UmbraHover);
            DrawCircle(midx, midy, R, Hover);
        }
        else DrawCircle(midx, midy, Rposibil, Jump);

    }
}

void ClearPosiblemove(int px[], int py[], int lg, int tabla[][total])
{
    for(int i = 0; i < lg; i++)
    {
        if(px[i] == -1 || py[i] == -1)
            continue;

        int midx = GetX_mat(px[i]) + dim / 2;
        int midy = GetY_mat(py[i]) + dim / 2;


        if(i == lg - 1 && tabla[py[i]][px[i]] == fills)
        {
            DrawCircle(midx - 2, midy + 2, R, Umbra);
            DrawCircle(midx, midy, R, Piesa);
        }
        else
        {
            if(tabla[py[i]][px[i]] == fills)
                continue;
            setfillstyle(SOLID_FILL, BLACK);
            floodfill(midx, midy, BLACK);
        }
    }
}

void DrawCircle(int x, int y, int r, COLORREF c)
{
    setcolor(c);
    circle(x, y, r);
    setfillstyle(SOLID_FILL, c);
    floodfill(x, y, c);
}

/**
    Afisez miscarile si numarul de mutari in partea stanga
*/

void UpdateMoves(Moves a[], int n)
{
    int startx = width / 2 + dim * total;
    int starty = height / 2 - dim * total / 2;
    char num[3], from_to[20];
    if(n > 9)
    {
        num[0] = n / 10 + '0';
        num[1] = n % 10 + '0';
        num[2] = 0;
    }
    else
    {
        num[0] = n + '0';
        num[1] = 0;
    }

    Clear(startx - 4 * dim + 10, 0, width, height);

    setcolor(WHITE);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 10);
    startx -= dim;
    outtextxy(startx, starty, num);


    int x = startx - 2 * dim;
    int y = starty + dim / 2;

    for(int i = 0; i < n; i++)
    {
        if(i % 25 == 0 && i != 0)
        {
            y = starty + dim / 2;
            x = x + 2 * dim;
        }

        from_to[0] = a[i].xi + '0' + 1;
        from_to[1] = ',';
        from_to[2] = a[i].yi + '0' + 1;
        from_to[3] = 0;
        strcat(from_to, " => ");
        from_to[7] = a[i].xf + '0' + 1;
        from_to[8] = ',';
        from_to[9] = a[i].yf + '0' + 1;
        from_to[10] = 0;

        setcolor(WHITE);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);

        outtextxy(x, y, from_to);
        y += dim / 3;
    }
}

/**
    Sterg un patrat din imagine
*/

void Clear(int xi, int yi, int xf, int yf)
{
    setcolor(ClearColor);
    rectangle(xi, yi, xf, yf);
    setfillstyle(SOLID_FILL, BLACK);
    floodfill(xi + 1, yi + 1, ClearColor);
    setcolor(BLACK);
    rectangle(xi, yi, xf, yf);
}

/**
    verific daca am gastigat/pierdut
*/

int CheckWin(int tabla[][total], int endx, int endy)
{
    /**
        return 0   mai sunt mutari
        return 1   am pierdut
        return 2   am castigat
    */

    int nrpiese = 0;
    for(int i = 0; i < total; i++)
    {
        for(int j = 0; j < total; j++)
        {
            if(tabla[i][j] != fills)
                continue;
            nrpiese++;
            int x, y;

            for(int k = 0; k < 4; k++)
            {
                x = i + dx[k];
                y = j + dy[k];

                if(Inmat(x, y) && tabla[x][y] == fills && Inmat(x + dx[k], y + dy[k]) && tabla[x + dx[k]][y + dy[k]] == emptys)
                    return 0;
            }
        }
    }

    if(nrpiese != 1) return 1;
    if(tabla[endy][endx] != fills) return 1;

    return 2;
}

/**
    Mut o piesa
*/
void MovePiece(int tabla[][total], int from_x, int from_y, int to_x, int to_y, int dir)
{
    tabla[from_y][from_x] = emptys;
    tabla[from_y + dy[dir]][from_x + dx[dir]] = emptys;
    tabla[to_y][to_x] = fills;

    int midx = GetX_mat(from_x) + dim / 2;
    int midy = GetY_mat(from_y) + dim / 2;

    setfillstyle(SOLID_FILL, BLACK);
    floodfill(midx, midy, BLACK);

    midx = GetX_mat(from_x + dx[dir]) + dim / 2;
    midy = GetY_mat(from_y + dy[dir]) + dim / 2;

    setfillstyle(SOLID_FILL, BLACK);
    floodfill(midx, midy, BLACK);

    midx = GetX_mat(to_x) + dim / 2;
    midy = GetY_mat(to_y) + dim / 2;

    DrawCircle(midx - 2, midy + 2, R, Umbra);
    DrawCircle(midx, midy, R, Piesa);
}

void Undo(int tabla[][total], Moves a[], int &n)
{
    if(n == 0)
        return;

    n--;
    tabla[a[n].xf][a[n].yf] = emptys;
    tabla[a[n].xi][a[n].yi] = fills;
    tabla[a[n].xi + dy[a[n].dir]][a[n].yi + dx[a[n].dir]] = fills;


    int midx = GetX_mat(a[n].yf) + dim / 2;
    int midy = GetY_mat(a[n].xf) + dim / 2;

    setfillstyle(SOLID_FILL, BLACK);
    floodfill(midx, midy, BLACK);
    midx = GetX_mat(a[n].yi) + dim / 2;
    midy = GetY_mat(a[n].xi) + dim / 2;

    DrawCircle(midx - 2, midy + 2, R, Umbra);
    DrawCircle(midx, midy, R, Piesa);

    midx = GetX_mat(a[n].yi + dx[a[n].dir]) + dim / 2;
    midy = GetY_mat(a[n].xi + dy[a[n].dir]) + dim / 2;

    DrawCircle(midx - 2, midy + 2, R, Umbra);
    DrawCircle(midx, midy, R, Piesa);
}

int NoPossibleMoves(int tabla[][total])
{
    int startx = width / 6;
    int starty = height / 2;

    Clear(startx, starty, startx + 2 * dim, starty + dim);

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    setcolor(Hover);

    if(variabila_meniu) outtextxy(startx, starty, "No. possible moves: ");
    else outtextxy(startx, starty, "Nr. mutari posibile: ");

    int cnt = 0;

    for(int i = 0; i < total; i++)
        for(int j = 0; j < total; j++)
        {
            if(tabla[i][j] != fills)
                continue;

            for(int k = 0; k < 4; k++)
            {
                int x = i + dx[k];
                int y = j + dy[k];

                if(Inmat(x, y) && tabla[x][y] == fills)
                {
                    x += dx[k];
                    y += dy[k];
                    if(Inmat(x, y) && tabla[x][y] == emptys)
                        cnt++;
                }
            }
        }

    char number[3];
    number[0] = cnt / 10 + '0';
    number[1] = cnt % 10 + '0';
    number[2] = 0;
    outtextxy(startx + 3 * dim / 2, starty, number);

    return cnt;
}

void UpdateTime(int ms)
{
    Clear(width / 6 - 2 * dim, height / 4 - dim / 2, width / 6 + 2 * dim, height / 4 + dim / 2);

    int s, m;

    s = ms / 1000;
    m = s / 60;
    s = s % 60;

    char S[3], M[10];

    M[0] = m / 10 + '0';
    M[1] = m % 10 + '0';
    M[2] = 0;

    S[0] = s / 10 + '0';
    S[1] = s % 10 + '0';
    S[2] = 0;

    strcat(M, " : ");
    strcat(M, S);

    setcolor(WHITE);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 7);
    outtextxy(width / 6 + 20, height / 5, M);
}

void ShuffleMove(int tabla[][total], int maxim, Moves m[], int &n)
{
    int r;
    srand(time(NULL));
    r = rand() % maxim;

    for(int i = 0; i < total; i++)
        for(int j = 0; j < total; j++)
            if(tabla[i][j] == fills)
            {
                for(int k = 0; k < 4; k++)
                {
                    int x = i + dy[k];
                    int y = j + dx[k];

                    if(Inmat(x, y) && tabla[x][y] == fills)
                    {
                        x += dy[k];
                        y += dx[k];

                        if(Inmat(x, y) && tabla[x][y] == emptys)
                        {
                            if(r == 0)
                            {
                                m[n++] = {i, j, x, y, k};
                                MovePiece(tabla, j, i, y, x, k);
                                return;
                            }
                            r--;
                        }
                    }
                }
            }
}

/**
    actualizez mutarile
*/

void Interact(int tabla[][total], int endx, int endy, bool &b, bool &r)
{
    Moves m[50];
    int px[5], py[5];   /// pozitiile valabile
    int lg = 0, nr_moves = 0, status, nr;
    bool enable = true;
    int undox = width / 6 - dim / 2 + 15;
    int undoy = height / 2 + dim * total / 2 - dim;

    NoPossibleMoves(tabla);

    UpdateMoves(m, nr_moves);

    UpdateTime(0);

    int mx, my, ct = 0;
    int lastx, lasty;
    int activex, activey;
    int clicks = 0, l = -1;
    activex = activey = lastx = lasty = -1;

    HDC dc = GetDC(NULL);
    while(1)
    {
        clearmouseclick(WM_LBUTTONDOWN);
        delay(50);
        ct += 50;

        if(ct % 1000 == 0 && enable)
            UpdateTime(ct);


        mx = mousex();
        my = mousey();

        int x = GetX_coord(mx) - 1;       /// !!! x e coloana si y e linia !!!
        int y = GetY_coord(my) - 1;

        int newx = GetX_mat(x) + dim / 2;
        int newy = GetY_mat(y) + dim / 2;

        if(GetPixel(dc, newx, newy) == Piesa)
        {
            DrawCircle(newx - 2, newy + 2, R, UmbraHover);
            DrawCircle(newx, newy, R, Hover);

            if(lastx > 0 && lasty > 0 && (activex != lastx || activey != lasty))
            {
                DrawCircle(lastx - 2, lasty + 2, R, Umbra);
                DrawCircle(lastx, lasty, R, Piesa);
            }

            lastx = newx;
            lasty = newy;
        }
        else
        {
            if(lastx > 0 && lasty > 0 && (lastx != newx || lasty != newy) && (activex != lastx || activey != lasty))
            {
                DrawCircle(lastx - 2, lasty + 2, R, Umbra);
                DrawCircle(lastx, lasty, R, Piesa);
                lastx = lasty = -1;
            }
        }

        if(ismouseclick(WM_LBUTTONDOWN))
        {

            if(mx < 100)
            {
                b = true;
                cleardevice();
                return;
            }


            if(undox - dim <= mx && mx < undox && undoy <= my && my <= undoy + dim)
            {
                r = true;
                cleardevice();
                return;
            }

            if(!enable)
                continue;


            if(GetPixel(dc, newx, newy) == Hover)   /// vad daca am dat click pe o piesa rosie
            {
                activex = newx;
                activey = newy;
                ClearPosiblemove(px, py, lg, tabla);
                lg = 0;
                for(int k = 0; k < 4; k++)
                {
                    int i = x + dx[k];
                    int j = y + dy[k];
                    if(!(Inmat(i, j) && Inmat(i + dx[k], j + dy[k])))
                       continue;

                    if(tabla[j + dy[k]][i + dx[k]] == emptys && tabla[j][i] == fills)
                    {
                        px[lg] = i + dx[k];
                        py[lg] = j + dy[k];
                        lg++;
                    }
                }

                px[lg] = x;
                py[lg] = y;
                lg++;
                Posiblemove(px, py, lg);    /// pun punctele verzi pe care ma pot duce
            }
            else {

                if(GetPixel(dc, newx, newy) == Jump)  /// verific daca am dat click pe o pozitie valida si mut piesa
                {
                    if(efect && !muzica)
                        PlaySound("piesa.wav",NULL,SND_ASYNC);

                    for(int k = 0; k < 4; k++)
                        if(px[lg - 1] + 2 * dx[k] == x && py[lg - 1] + 2 * dy[k] == y)
                        {
                            m[nr_moves] = {py[lg - 1], px[lg - 1], y, x, k};
                            nr_moves++;

                            for(int j = 0; j < lg - 1; j++)
                                if(px[j] == x && py[j] == y) px[j] = py[j] = -1;

                            MovePiece(tabla, px[lg - 1], py[lg - 1], x, y, k);
                            break;
                        }
                    UpdateMoves(m, nr_moves);
                }
                ClearPosiblemove(px, py, lg, tabla);

                if(undox <= mx && mx <= undox + dim && undoy <= my && my <= undoy + dim)
                {
                    Undo(tabla, m, nr_moves);
                    UpdateMoves(m, nr_moves);
                }

                nr = NoPossibleMoves(tabla);
                if(undox + dim <= mx && mx <= undox + 2 * dim && undoy <= my && my <= undoy + dim)
                {
                    ShuffleMove(tabla, nr, m, nr_moves);
                    UpdateMoves(m, nr_moves);
                    if(efect && !muzica)
                        PlaySound("piesa.wav",NULL,SND_ASYNC);
                }

            }

            status = CheckWin(tabla, endx, endy);

            if(status)
            {
                settextjustify(CENTER_TEXT, CENTER_TEXT);
                settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 7);
                int startx = width / 2;
                int starty = height / 2;
                if(status == 1)
                {
                    setcolor(YELLOW);
                    if(variabila_meniu) outtextxy(startx, starty, "You lost!");
                    else outtextxy(startx, starty, "Ai pierdut!");
                }
                else
                {
                    setcolor(BLUE);
                    if(variabila_meniu)outtextxy(startx, starty, "You won!");
                    else outtextxy(startx, starty, "Ai castigat!");
                }

                setcolor(WHITE);
                enable = false;
            }
        }
    }
}

/**
    generez tabla de joc
*/

void Start(char loc[])
{
    ifstream fin(loc);
    int endx, endy;
    int tabla[total][total];

    fin >> endx >> endy;
    for(int i = 0; i < total; i++)
        for(int j = 0; j < total; j++)
            fin >> tabla[i][j];

    fin.close();

    setcolor(WHITE);
    Sageata();

    char img[256];
    strcpy(img, current_folder);
    strcat(img, "/res/undo.jpg");

    int undox = width / 6 - dim / 2 + 15;
    int undoy = height / 2 + dim * total / 2 - dim;
    readimagefile(img, undox, undoy, undox + dim, undoy + dim);

    strcpy(img, current_folder);
    strcat(img, "/res/reset.jpg");
    readimagefile(img, undox - dim + 10, undoy + 10, undox - 10, undoy + dim - 10);

    strcpy(img, current_folder);
    strcat(img, "/res/random.jpg");
    readimagefile(img, undox + dim, undoy, undox + 2 * dim, undoy + dim);

    int startx = width / 2 - dim * total / 2 - dim;
    int starty = height / 2 - dim * total / 2;
    int aux = startx, midx, midy;
    char number[3];

    settextjustify(0, 2);
    settextstyle(8, HORIZ_DIR, 1);

    for(int i = 0; i < total; i++)
    {
        for(int j = 0; j < total; j++)
        {
            startx += dim;
            if(!tabla[i][j])
                continue;

            number[0] = i + '0' + 1;
            number[1] = j + '0' + 1;
            number[2] = 0;

            setcolor(WHITE);
            rectangle(startx, starty, startx + dim, starty + dim);

            midx = startx + dim / 2;
            midy = starty + dim / 2;

            outtextxy(midx + dim / 6, midy + dim / 6, number);

            if(tabla[i][j] == fills)
            {
                DrawCircle(midx - 2, midy + 2, R, Umbra);
                DrawCircle(midx, midy, R, Piesa);
            }

        }
        starty += dim;
        startx = aux;
    }

    setcolor(Hover);
    number[1] = endx + '0' + 1;
    number[0] = endy + '0' + 1;
    startx = GetX_mat(endx);
    starty = GetY_mat(endy);
    rectangle(startx, starty, startx + dim, starty + dim);
    midx = startx + dim / 2;
    midy = starty + dim / 2;

    outtextxy(midx + dim / 6, midy + dim / 6, number);

    bool b, r;
    b = r = false;
    Interact(tabla, endx, endy, b, r);

    if(b == true)
    {
        setvisualpage(0);
        setactivepage(0);
        cleardevice();
        SelectLvl();
    }

    if(r == true)
    {
        setvisualpage(0);
        setactivepage(0);
        cleardevice();
        Start(loc);
    }
}

/**
    verific daca tabla e valida
*/

void Fill(int tabla[][total], int x, int y)
{
    tabla[x][y] = 0;

    for(int k = 0; k < 4; k++)
    {
        int i = x + dx[k];
        int j = y + dy[k];

        if(Inmat(i, j) && tabla[i][j])
            Fill(tabla, i, j);
    }
}

/**
    Tabla customizabila
*/

void CreateTable()
{
    int tabla[total][total], lgnume = 0;
    char nume[255], aux[255];
    memset(tabla, 0, sizeof(tabla));
    settextstyle(8, HORIZ_DIR, 1);
    settextjustify(0, 2);

    Sageata();

    int patratx, patraty;       /// Patrat gol
    patratx = 100;
    patraty = height / 2 - dim;

    setcolor(WHITE);
    rectangle(patratx, patraty, patratx + dim, patraty + dim);
    setcolor(Create);
    if(variabila_meniu) strcpy(aux, "Add empty square");
    else strcpy(aux, "Adauga patrat gol");
    outtextxy(patratx + dim + 25, patraty + dim / 2, aux);

    int cercx, cercy;       /// Cerc
    cercx = patratx + dim / 2;
    cercy = patraty + 2 * dim;
    DrawCircle(cercx - 2, cercy + 2, R, Umbra);
    DrawCircle(cercx, cercy, R, Piesa);
    setcolor(Create);

    if(variabila_meniu) strcpy(aux, "Add circle");
    else strcpy(aux, "Adauga cerc");
    outtextxy(cercx + dim / 2 + 25, cercy, aux);

    int patratx_end, patraty_end;   /// Finalul

    patratx_end = patratx;
    patraty_end = cercy + dim;

    setcolor(Hover);
    rectangle(patratx_end, patraty_end, patratx_end + dim, patraty_end + dim);

    setcolor(Create);

    if(variabila_meniu) strcpy(aux, "Add end-point");
    else strcpy(aux, "Adauga end-point");
    outtextxy(patratx_end + dim + 25, patraty_end + dim / 2, aux);

    int deletex, deletey;       /// Buton de delete
    deletex = patratx;
    deletey = patraty - dim;

    setcolor(RED);
    rectangle(deletex, deletey, deletex + 2 * dim, deletey + dim / 2);

    settextjustify(1, 1);

    if(variabila_meniu) strcpy(aux, "Delete");
    else strcpy(aux, "Sterge");
    outtextxy(deletex + dim, deletey + dim / 3, aux);
    setcolor(Create);

    int savex, savey;       /// Buton de delete
    savex = deletex;
    savey = deletey - dim;

    setcolor(BLUE);
    rectangle(savex, savey, savex + 2 * dim, savey + dim / 2);

    if(variabila_meniu) strcpy(aux, "Save");
    else strcpy(aux, "Salveaza");
    outtextxy(savex + dim, savey + dim / 3, aux);
    setcolor(Create);

    int startx = width / 2 - dim * total / 2 - dim;
    int starty = height / 2 - dim * total / 2;
    int v = startx;
    int midx, midy;
    char number[3];

    if(variabila_meniu)
    {
        outtextxy(width / 2, height - dim, "*press esc to exit save mode");
        outtextxy(width / 2, height - dim / 2, "*press enter after typing the name");
    }
    else
    {
        outtextxy(width / 2, height - dim, "*apasa esc pentru a iesi din salvare");
        outtextxy(width / 2, height - dim / 2, "*apasa enter dupa introducerea numelui");
    }


    settextjustify(0, 2);
    for(int i = 0; i < total; i++)
    {
        for(int j = 0; j < total; j++)
        {
            startx += dim;
            number[0] = i + '0' + 1;
            number[1] = j + '0' + 1;
            number[2] = 0;
            rectangle(startx, starty, startx + dim, starty + dim);
            midx = startx + dim / 2;
            midy = starty + dim / 2;

            outtextxy(midx + dim / 6, midy + dim / 6, number);
        }
        starty += dim;
        startx = v;
    }

    int ex = -1, ey = -1;
    int mx, my, p = 0, c = 0, e = 0, d = 0, s = 0;
    while(1)
    {
        clearmouseclick(WM_LBUTTONDOWN);
        delay(10);
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            mx = mousex();
            my = mousey();
            if(mx < 100)
            {
                setcolor(WHITE);
                cleardevice();
                settextjustify(CENTER_TEXT, CENTER_TEXT);

                return;
            }

            /// vad ce am selectat
            setcolor(WHITE);
            if(patratx <= mx && mx <= patratx + dim && patraty <= my && my <= patraty + dim)
            {
                p = 1;      /// setez patrat alb
                c = 0;      /// setez cerc
                e = 0;      /// setez end point
                d = 0;      /// setez delete
                s = 0;      /// setez save
            }
            else if(dist(mx, my, cercx, cercy) - 10 <= R)
            {
                p = 0;
                c = 1;
                e = 0;
                d = 0;
                s = 0;
            }
            else if(patratx_end <= mx && mx <= patratx_end + dim && patraty_end <= my && my <= patraty_end + dim)
            {
                p = 0;
                c = 0;
                e = 1;
                d = 0;
                s = 0;
            }
            else if(deletex <= mx && mx <= deletex + 2 * dim && deletey <= my && my <= deletey + dim / 2)
            {
                p = 0;
                c = 0;
                e = 0;
                d = 1;
                s = 0;
            }
            else if(savex <= mx && mx <= savex + 2 * dim && savey <= my && my <= savey + dim / 2)
            {
                p = 0;
                c = 0;
                e = 0;
                d = 0;
                s = 1;
            }


            int x, y, newx, newy;
            x = GetX_coord(mx) - 1;
            y = GetY_coord(my) - 1;

            if(x > -1 && y > -1 && x < total && y < total)
            {
                settextstyle(8, HORIZ_DIR, 1);
                settextjustify(0, 2);
                if(p == 1)      /// adaug un patrat
                {
                    tabla[y][x] = emptys;
                    newx = GetX_mat(x);
                    newy = GetY_mat(y);
                    number[0] = y + '0' + 1;
                    number[1] = x + '0' + 1;
                    number[2] = 0;
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    setcolor(WHITE);
                    rectangle(newx, newy, newx + dim, newy + dim);
                    outtextxy(midx + dim / 6, midy + dim / 6, number);
                }
                else if(c == 1 && tabla[y][x] == emptys)    /// adaug un cerc
                {
                    tabla[y][x] = fills;
                    newx = GetX_mat(x);
                    newy = GetY_mat(y);
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    DrawCircle(midx - 2, midy + 2, R, Umbra);
                    DrawCircle(midx, midy, R, Piesa);
                }
                else if(e == 1 && (tabla[y][x] == emptys || tabla[y][x] == fills))
                {
                    if(ex != -1 && ey != -1) /// clear patrat sfarsit
                    {
                        newx = GetX_mat(ex);
                        newy = GetY_mat(ey);

                        number[0] = ey + '0' + 1;
                        number[1] = ex + '0' + 1;
                        number[2] = 0;
                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setcolor(WHITE);
                        rectangle(newx, newy, newx + dim, newy + dim);
                        outtextxy(midx + dim / 6, midy + dim / 6, number);
                    }

                    ex = x;         /// adaug un patrat verde
                    ey = y;
                    newx = GetX_mat(ex);
                    newy = GetY_mat(ey);

                    number[0] = ey + '0' + 1;
                    number[1] = ex + '0' + 1;
                    number[2] = 0;
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    setcolor(Hover);
                    rectangle(newx, newy, newx + dim, newy + dim);
                    outtextxy(midx + dim / 6, midy + dim / 6, number);

                }
                else if(d == 1)
                {
                    if(x == ex && y == ey)      /// elimin patratul de final
                    {
                        newx = GetX_mat(ex);
                        newy = GetY_mat(ey);

                        number[0] = ey + '0' + 1;
                        number[1] = ex + '0' + 1;
                        number[2] = 0;
                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setcolor(WHITE);
                        rectangle(newx, newy, newx + dim, newy + dim);
                        outtextxy(midx + dim / 6, midy + dim / 6, number);
                        ex = ey = -1;
                    }
                    else if(tabla[y][x] == fills)       /// elimin un cerc
                    {
                        tabla[y][x] = emptys;
                        newx = GetX_mat(x);
                        newy = GetY_mat(y);

                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setfillstyle(SOLID_FILL, BLACK);
                        floodfill(midx, midy, BLACK);
                    }
                    else if(tabla[y][x] == emptys)      /// elimin un patrat gol
                    {
                        tabla[y][x] = 0;
                        newx = GetX_mat(x);
                        newy = GetY_mat(y);

                        number[0] = y + '0' + 1;
                        number[1] = x + '0' + 1;
                        number[2] = 0;
                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setcolor(Create);
                        rectangle(newx, newy, newx + dim, newy + dim);
                        outtextxy(midx + dim / 6, midy + dim / 6, number);

                        for(int k = 0; k < 4; k++)      /// refact patratele stricate
                        {
                            int i = x + dx[k];
                            int j = y + dy[k];

                            if(Inmat(i, j) && tabla[j][i] != 0)
                            {
                                if(i == ex && j == ey) setcolor(Hover);
                                else setcolor(WHITE);
                                i = GetX_mat(i);
                                j = GetY_mat(j);
                                rectangle(i, j, i + dim, j + dim);
                            }
                        }

                    }
                }

                if(ex != -1 && ey != -1) /// patratul de final trebuie sa fie mereu Activ
                {
                    int i = GetX_mat(ex);
                    int j = GetY_mat(ey);
                    setcolor(Hover);
                    rectangle(i, j, i + dim, j + dim);
                }
            }

            int lasts = s;
            settextstyle(8, HORIZ_DIR, 3);
            settextjustify(1, 1);
            while(s == 1)     /// salvez intr un fisier
            {
                setcolor(WHITE);
                outtextxy(startx + (total + 3) * dim, height / 2 - dim / 2, "NUME:");

                if(kbhit())
                {
                    char ch = getch();
                    if(ch == '\b')
                    {
                        if(lgnume > 0)
                        {
                            lgnume--;
                            nume[lgnume] = 0;

                            Clear(startx + (total + 2) * dim - 10, height / 2 - dim / 2, width, height / 2 + dim);
                            setcolor(Azure);
                            outtextxy(startx + (total + 3) * dim, height / 2, nume);
                        }
                    }
                    else if(ch == 13)
                    {
                        if(lgnume == 0)
                        {
                            Clear(60, 0, width, 60);
                            setcolor(Color);

                            if(variabila_meniu) strcpy(aux, "Add a name!");
                            else strcpy(aux, "Introduceti un nume!");
                            outtextxy(width / 2, 40, aux);
                            continue;
                        }

                        if(ex == -1 || ey == -1)
                        {
                            Clear(60, 0, width, 60);
                            setcolor(Color);
                            if(variabila_meniu) strcpy(aux, "End-point is missing!");
                            else strcpy(aux, "Lipseste end-point!");
                            outtextxy(width / 2, 40, aux);
                            continue;
                        }

                        int nrinsule = 0;
                        int copyt[total][total];

                        for(int i = 0; i < total; i++)
                            for(int j = 0; j < total; j++)
                                copyt[i][j] = tabla[i][j];

                        for(int i = 0; i < total; i++)
                            for(int j = 0; j < total; j++)
                                if(copyt[i][j])
                                {
                                    nrinsule++;
                                    Fill(copyt, i, j);
                                }

                        if(CheckWin(tabla, ex, ey) == 1 || nrinsule != 1)
                        {
                            Clear(60, 0, width, 60);
                            setcolor(Color);
                            if(variabila_meniu) strcpy(aux, "Unwinnable!");
                            else strcpy(aux, "Imposibil de castigat!");
                            outtextxy(width / 2, 40, aux);

                            continue;
                        }

                        char dir[255];
                        strcpy(dir, current_folder);
                        strcat(dir, "/maps/");
                        strcat(dir, nume);
                        strcat(dir, ".txt");

                        ofstream fout(dir);

                        fout << ex << " " << ey << "\n";
                        for(int i = 0; i < total; i++, fout << "\n")
                            for(int j = 0; j < total; j++)
                                fout << tabla[i][j] << " ";

                        fout.close();
                        s = 0;
                    }
                    else if(ch == 27)
                    {
                        s = 2;
                        lgnume = 0;
                        Clear(60, 0, width, 60);
                        Clear(startx + (total + 2) * dim, height / 2 - dim, width, height / 2 + dim);
                    }
                    else
                    {
                        if(lgnume < 11)
                        {
                            if(isalpha(ch) || ('0' <= ch && ch <= '9') || ch == '-' || ch == '_')
                            {
                                nume[lgnume++] = ch;
                                nume[lgnume] = 0;
                                setcolor(Azure);
                                outtextxy(startx + (total + 3) * dim, height / 2, nume);
                            }
                        }
                    }
                }

                delay(10);
            }

            if(lasts == 1 && s == 0)
                break;
        }
    }
    settextjustify(CENTER_TEXT, CENTER_TEXT);
}

void SelectLvl()
{
    Sageata();

    char s[255];
    strcpy(s, current_folder);
    strcat(s, "/maps");

    struct dirent *entry;
    DIR *dir = opendir(s);

    int nrlvl = 0;
    char lvl[50][100];
    while ((entry = readdir(dir)) != NULL)
        strcpy(lvl[nrlvl++], entry->d_name);

    closedir(dir);

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
    setcolor(WHITE);

    if(variabila_meniu) outtextxy(width / 2, 50, "Select Map");
    else outtextxy(width / 2, 50, "Selecteaza Mapa");

    Moves coord[50];
    int lg, but = 0, index = 0;
    char news[255];
    int startx = 100;
    int starty = 100;
    srand(time(0));
    for(int i = 0; i < nrlvl; i++)
    {
        COLORREF random = RGB((rand() % 255), (rand() % 255), (rand() % 255));
        if(random == BLACK)
            random = RGB((rand() % 255), (rand() % 255), (rand() % 255));

        setcolor(random);

        lg = 0;
        for(int j = 0; lvl[i][j]; j++)
        {
            if(lvl[i][j] == '.')
                break;

            news[lg++] = lvl[i][j];
        }
        news[lg] = 0;

        if(lg == 0)
            continue;

        if(index > 5)
        {
            index = 0;
            startx = 100;
            starty += 2 * dim;
        }

        index++;
        coord[but++] = {startx, starty, startx + 2 * dim, starty + dim};
        rectangle(startx, starty, startx + 2 * dim, starty + dim);
        settextstyle(8,HORIZ_DIR,1);
        outtextxy(startx + dim, starty + dim / 2, news);

        settextstyle(10,HORIZ_DIR,2);
        outtextxy(startx + dim, starty - 10, "X");

        startx += 3 * dim;
    }

    int mx, my;
    while(1)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            mx = mousex();
            my = mousey();
            if(mx < 100)
            {
                setcolor(WHITE);
                return;
            }

            for(int i = 0; i < but; i++)
            {
                if(coord[i].xi <= mx && mx <= coord[i].xf && coord[i].yi <= my && my <= coord[i].yf)
                {
                    news[0] = 0;
                    strcpy(news, current_folder);
                    strcat(news, "/maps/");
                    strcat(news, lvl[i + nrlvl - but]);
                    setvisualpage(0);
                    setactivepage(0);
                    cleardevice();
                    clearmouseclick(WM_LBUTTONDOWN);
                    Start(news);
                }

                if(coord[i].xi + dim - 10 <= mx && mx <= coord[i].xi + dim + 10 && coord[i].yi - 30 <= my && my <= coord[i].yi - 1)
                {
                    news[0] = 0;
                    strcpy(news, current_folder);
                    strcat(news, "/maps/");
                    strcat(news, lvl[i + nrlvl - but]);
                    remove(news);
                    setvisualpage(0);
                    setactivepage(0);
                    cleardevice();

                    clearmouseclick(WM_LBUTTONDOWN);
                    SelectLvl();

                    return;
                }
            }
        }

        clearmouseclick(WM_LBUTTONDOWN);
        delay(100);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Meniu()
{
    initwindow(width,height, "", -3, -3);
    GetCurrentDirectoryA(256, current_folder);
    for(int i = 0; current_folder[i]; i++)
        if(current_folder[i] == '\\')
            current_folder[i] = '/';

    Interactiune_Meniu();
}

void Interactiune_Meniu()
{
    int x,y,schimbare=1;
    int culoarepiesa=1,culoarehover=1;


    Reguli_Romana();
    while(1)
    {
        ///double buffering
        setvisualpage(page);
        setactivepage(1-page);

        /// highlight butoane
        Meniu_Highlight(variabila_meniu,schimbare);

        ///click butoane
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);

            x=mousex();
            y=mousey();

            if(x>=615 && x<=735 && y>=200 && y<=240)
            {
                setvisualpage(page);
                setactivepage(page);
                cleardevice();
                SelectLvl();
            }

            else if(x>=535 && x<=815 && y>=260 && y<=300)
            {
                setvisualpage(page);
                setactivepage(page);
                cleardevice();
                CreateTable();
            }

            else if(x>=525 && x<=825 && y>=340 && y<=380)
            {
                setvisualpage(page);
                setactivepage(1-page);
                cleardevice();
                Reguli();
            }

            else if(x>=570 && x<=780 && y>=420 && y<=460)
                Setari(culoarepiesa,culoarehover);


            else if(x>=585 && x<=765 && y>=500 && y<=540)
                exit(1);
        }
    }
}

inline void Meniu_Romana(int i)
{
    settextstyle(8,HORIZ_DIR,6);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    outtextxy(width / 2,100,"[Solitar]");

    settextstyle(8,HORIZ_DIR,4);
    if(i!=1)
        outtextxy(width / 2,225,"Joaca");
    if(i!=2)
        outtextxy(width / 2,300,"Creaza mapa");
    if(i!=3)
        outtextxy(width / 2,375,"Cum se joaca?");
    if(i!=4)
        outtextxy(width / 2,450,"Setari");
    if(i!=5)
        outtextxy(width / 2,525,"Iesire");
}

inline void Meniu_Engleza(int i)
{
    settextstyle(8,HORIZ_DIR,6);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    outtextxy(width / 2,100,"[Solitaire]");

    settextstyle(8,HORIZ_DIR,4);
    if(i!=1)
        outtextxy(width / 2,225,"Play");
    if(i!=2)
        outtextxy(width / 2,300,"Create map");
    if(i!=3)
        outtextxy(width / 2,375,"How to play?");
    if(i!=4)
        outtextxy(width / 2,450,"Settings");
    if(i!=5)
        outtextxy(width / 2,525,"Quit");
}

void Meniu_Highlight(int meniu,int &schimbare)
{
    int x,y;
    x=mousex();
    y=mousey();

    cleardevice();
    if(x>=615 && x<=735 && y>=200 && y<=240)
    {
        schimbare=1;
        if(!meniu)
        {
            Highlight("Joaca",675,220,5,Active);
            Meniu_Romana(1);
        }
        else
        {
            Highlight("Play",675,220,5,Active);
            Meniu_Engleza(1);
        }
    }

    else if(x>=535 && x<=815 && y>=260 && y<=300)
    {
        schimbare=2;
        if(!meniu)
        {
            Highlight("Creaza mapa",675,290,5,Azure);
            Meniu_Romana(2);
        }
        else
        {
            Highlight("Create map",675,290,5,Azure);
            Meniu_Engleza(2);
        }
    }

    else if(x>=525 && x<=825 && y>=340 && y<=380)
    {
        schimbare=3;
        if(!meniu)
        {
            Highlight("Cum se joaca?",675,370,5,Azure);
            Meniu_Romana(3);
        }
        else
        {
            Highlight("How to play?",675,370,5,Azure);
            Meniu_Engleza(3);
        }
    }

    else if(x>=570 && x<=780 && y>=420 && y<=460)
    {

        schimbare=4;
        if(!meniu)
        {
            Highlight("Setari",675,445,5,Azure);
            Meniu_Romana(4);
        }
        else
        {
            Meniu_Engleza(4);
            Highlight("Settings",675,445,5,Azure);
        }
    }

    else if(x>=585 && x<=765 && y>=500 && y<=540)
    {
        schimbare=5;
        if(!meniu)
        {
            Highlight("Iesire",675,515,5,Color);
            Meniu_Romana(5);
        }
        else
        {
            Highlight("Quit",675,515,5,Color);
            Meniu_Engleza(5);
        }
    }

    else
    {
        schimbare=0;
        if(!meniu)
            Meniu_Romana(0);
        else
            Meniu_Engleza(0);
    }

    page = 1 - page;
    delay(50);
}

void Setari(int &culoarepiesa,int &culoarehover)
{
    int x,y;

    while(1)
    {
        setvisualpage(page);
        setactivepage(1-page);

        cleardevice();
        Sageata();

        if(!variabila_meniu)
            Setari_Romana(culoarepiesa,culoarehover);
        else
            Setari_Engleza(culoarepiesa, culoarehover);

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            x=mousex();
            y=mousey();

            if(x>=230 && x<=555 && y>=185 && y<=410 && variabila_meniu)
            {
                variabila_meniu=0;
                return;
            }

            else if(x>=655 && x<=975 && y>=185 && y<=410 && !variabila_meniu)
            {
                variabila_meniu=1;
                return;
            }

            else if(x>=270 && x<=550 && y>=450 && y<=670)
                efect=1-efect;

            else if(x>=700 && x<=920 && y>=450 && y<=660)
            {
                muzica=1-muzica;
                if(!muzica)
                        PlaySound(NULL,0,0);
                else
                        PlaySound("Music.wav",NULL,SND_LOOP | SND_ASYNC);
            }

            else if(x<=200)
                return;

            PiesaGlobala(x,y,culoarepiesa);
            HoverGlobal(x,y,culoarehover);
        }

        page = 1 - page;
    }
}

void PiesaGlobala(int x, int y, int &culoarepiesa)
{
    if( x>=1058 && x<=1118 && y>=185 && y<=240 )
    {
        culoarepiesa=1;
        Piesa=Color;
        Umbra=ColorShadow;
    }

    else if( x>=1058 && x<= 1118 && y>=285 && y<=340)
    {
        culoarepiesa=2;
        Piesa=Blu;
        Umbra=BluShadow;
    }

    else if(x>=1058 && x<=1118 && y>=385 && y<=440)
    {
        culoarepiesa=3;
        Piesa=Active;
        Umbra=ActiveShadow;
    }

    else if(x>=1058 && x<=1118 && y>=485 && y<=540)
    {
        culoarepiesa=4;
        Piesa=Gold;
        Umbra=GoldShadow;
    }
}

void HoverGlobal(int x,int y,int &culoarehover)
{
    if( x>=1158 && x<=1218 && y>=185 && y<=240 )
    {
        culoarehover=1;
        Hover=Active;
        UmbraHover=ActiveShadow;
        Jump= RGB(0,255,1);
    }

    else if( x>=1158 && x<= 1218 && y>=285 && y<=340)
    {
        culoarehover=2;
        Hover=Gold;
        UmbraHover=GoldShadow;
        Jump= RGB(255,223,1);
    }

    else if(x>=1158 && x<=1218 && y>=385 && y<=440)
    {
        culoarehover=3;
        Hover=Color;
        UmbraHover=ColorShadow;
        Jump= RGB(200,0,31);
    }

    else if(x>=1158 && x<=1218 && y>=485 && y<=540)
    {
        culoarehover=4;
        Hover=Blu;
        UmbraHover=BluShadow;
        Jump= RGB(0,50,81);
    }
}


void Setari_Romana(int culoarepiesa, int culoarehover)
{
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(630,60,"Setari");

    settextstyle(8,HORIZ_DIR,5);
    outtextxy(170,150,"Limba:");
    outtextxy(170,420,"Sunet:");
    outtextxy(1150,150,"Culoare:");

    ///culori piese
    DrawCircle(1088,215,20,ColorShadow);
    DrawCircle(1092,213,20,Color);

    DrawCircle(1088,315,20,BluShadow);
    DrawCircle(1092,313,20,Blu);

    DrawCircle(1088,415,20,ActiveShadow);
    DrawCircle(1092,413,20,Active);

    DrawCircle(1088,515,20,GoldShadow);
    DrawCircle(1092,513,20,Gold);

    ///culori uncte
    DrawCircle(1188,215,10,Active);
    DrawCircle(1188,315,10,Gold);
    DrawCircle(1188,415,10,Color);
    DrawCircle(1188,515,10,Blu);

    setcolor(15);
    ///piesa selectata
    if(culoarepiesa==1)
        rectangle(1058,185,1118,240);
    if(culoarepiesa==2)
        rectangle(1058,285,1118,340);
    if(culoarepiesa==3)
        rectangle(1058,385,1118,440);
    if(culoarepiesa==4)
        rectangle(1058,485,1118,540);

    ///punct selectat
    if(culoarehover==1)
        rectangle(1158,185,1218,240);
    if(culoarehover==2)
        rectangle(1158,285,1218,340);
    if(culoarehover==3)
        rectangle(1158,385,1218,440);
    if(culoarehover==4)
        rectangle(1158,485,1218,540);

    settextstyle(8,HORIZ_DIR,4);
    setcolor(Azure);
    outtextxy(350,370,"Ro");
    setcolor(YELLOW);
    outtextxy(390,370,"ma");
    setcolor(Color);
    outtextxy(430,370,"na");

    char img[256];
    strcpy(img, current_folder);
    strcat(img, "/res/romana.jpg");
    readimagefile(img,240,195,540,345);

    strcpy(img, current_folder);
    strcat(img, "/res/english.jpg");
    readimagefile(img,665,195,965,345);

    if(efect)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,360,460,500,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,360,460,500,600);
    }
    outtextxy(410,640,"Efecte sonore");

    if(muzica)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,750,460,890,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,750,460,890,600);
    }
    outtextxy(800,640,"Muzica");

    setcolor(WHITE);
    outtextxy(820,370,"English");
}

void Setari_Engleza(int culoarepiesa, int culoarehover)
{
    setcolor(WHITE);
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(600,60,"Settings");

    settextstyle(8,HORIZ_DIR,5);
    outtextxy(170,150,"Language:");
    outtextxy(170,420,"Sound:");
    outtextxy(1150,150,"Color:");

    ///culori piese
    DrawCircle(1088,215,20,ColorShadow);
    DrawCircle(1092,213,20,Color);

    DrawCircle(1088,315,20,BluShadow);
    DrawCircle(1092,313,20,Blu);

    DrawCircle(1088,415,20,ActiveShadow);
    DrawCircle(1092,413,20,Active);

    DrawCircle(1088,515,20,GoldShadow);
    DrawCircle(1092,513,20,Gold);

    ///culori puncte
    DrawCircle(1188,215,10,Active);
    DrawCircle(1188,315,10,Gold);
    DrawCircle(1188,415,10,Color);
    DrawCircle(1188,515,10,Blu);

    setcolor(15);
    ///piesa selectata
    if(culoarepiesa==1)
        rectangle(1058,185,1118,240);
    if(culoarepiesa==2)
        rectangle(1058,285,1118,340);
    if(culoarepiesa==3)
        rectangle(1058,385,1118,440);
    if(culoarepiesa==4)
        rectangle(1058,485,1118,540);

    ///punct selectat
    if(culoarehover==1)
        rectangle(1158,185,1218,240);
    if(culoarehover==2)
        rectangle(1158,285,1218,340);
    if(culoarehover==3)
        rectangle(1158,385,1218,440);
    if(culoarehover==4)
        rectangle(1158,485,1218,540);

    settextstyle(8,HORIZ_DIR,4);
    setcolor(Azure);
    outtextxy(760,370,"En");
    setcolor(Color);
    outtextxy(810,370,"gli");
    setcolor(WHITE);
    outtextxy(860,370,"sh");

    char img[256];
    strcpy(img, current_folder);
    strcat(img, "/res/romana.jpg");
    readimagefile(img,240,195,540,345);

    strcpy(img, current_folder);
    strcat(img, "/res/english.jpg");
    readimagefile(img,665,195,965,345);

    if(efect)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,360,460,500,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,360,460,500,600);
    }
    outtextxy(410,640,"Sound effects");

    if(muzica)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,750,460,890,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,750,460,890,600);
    }
    outtextxy(800,640,"Music");

    setcolor(WHITE);
    outtextxy(390,370,"Romana");
}


void Reguli()
{
    int x;

    cleardevice();
    if(!variabila_meniu)
        Reguli_Romana();
    else
        Reguli_Engleza();
    setvisualpage(1-page);
    while(1)
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            x=mousex();

            if(x<=250)
                return;
        }
}

void Reguli_Romana()
{
    Sageata();
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(width/2,60,"Cum se joaca?");

    settextstyle(8,HORIZ_DIR,4);

    ///cuvinte albe
    setcolor(WHITE);
    outtextxy(700,150,"1)Tabla de joc are un spatiu liber, restul fiind ocupate");
    outtextxy(200,190,"de piese.");
    outtextxy(720,250,"2)Doua piese consecutive pot fi mutate pe un spatiu liber,");
    outtextxy(620,300,"alaturat, marcat de un punct, devenind una singura.");
    outtextxy(650,370,"3)Jocul este castigat atunci cand pe tabla ramane o ");
    outtextxy(700,410,"singura piesa si este pierdut daca pe tabla raman cel putin");
    outtextxy(490,450,"doua piese, care nu mai pot fi mutate.");

    ///cuvinte culoare hover
    setcolor(Hover);
    outtextxy(700,150,"spatiu liber");
    outtextxy(1160,250,"spatiu liber");
    outtextxy(620,300,"punct");
    outtextxy(1140,370,"o");

    ///cuvinte culoare piesa
    setcolor(Piesa);
    outtextxy(220,190,"piese");
    outtextxy(280,250,"Doua piese");
    outtextxy(1000,300,"una singura");
    outtextxy(210,450,"doua piese");
    outtextxy(240,410,"singura piesa");


    setcolor(Active);
    outtextxy(470,370,"castigat");
    setcolor(Color);
    outtextxy(620,410,"pierdut");

    setcolor(WHITE);
}

void Reguli_Engleza()
{
    Sageata();
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(width/2,60,"How to Play?");

    settextstyle(8,HORIZ_DIR,4);

    ///cuvinte albe
    setcolor(WHITE);
    outtextxy(710,150,"1)The game board has one blank square and the other ones");
    outtextxy(340,190,"are occupied by pieces.");
    outtextxy(650,250,"2)Two side by side pieces can be moved on an empty");
    outtextxy(630,300,"neighboring square, marked by a point, becoming one.");
    outtextxy(680,370,"3)The game is won when there is only one piece left on");
    outtextxy(670,410,"the board and is lost if two or more pieces remain, that");
    outtextxy(280,450,"can not be moved.");

    ///cuvinte culoare hover
    setcolor(Hover);
    outtextxy(770,150,"blank square");
    outtextxy(1100,250,"empty");
    outtextxy(290,300,"neighboring square");
    outtextxy(800,300,"point");

    ///cuvinte culoare piesa
    setcolor(Piesa);
    outtextxy(490,190,"pieces");
    outtextxy(220,250,"Two");
    outtextxy(590,250,"pieces");
    outtextxy(790,410,"two or more pieces");
    outtextxy(920,370,"only one piece");


    setcolor(Active);
    outtextxy(450,370,"won");
    setcolor(Color);
    outtextxy(490,410,"lost");

    setcolor(WHITE);
}

void Sageata()
{
    setcolor(WHITE);
    setlinestyle(0,0,3);
    line(25,350,50,325);
    line(25,350,50,375);
    setlinestyle(0,0,1);
}

inline void Highlight(char cuv[],int x,int y,int marime,unsigned culoare)
{
    setcolor(culoare);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(8,HORIZ_DIR,marime);
    outtextxy(x,y,cuv);
    setcolor(15);
    settextstyle(8,HORIZ_DIR,4);

}

int main()
{
    PlaySound(TEXT("Music.wav"),NULL,SND_LOOP | SND_ASYNC);
    Meniu();
    return 0;
}

