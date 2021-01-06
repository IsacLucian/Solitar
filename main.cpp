#include <iostream>
#include <fstream>
#include <graphics.h>
#include <math.h>
#include <filesystem>
#include <dirent.h>

namespace fs = std::filesystem;
using namespace std;

const int total = 8;    // dimensiunea matricei
int fills = 1;    // fill square
int emptys = 2;   // empty square
int width = 1350;
int height = 700;
int dim = 65;     // dimensiunea unui patrat
int R = 16;       // raza cerculi
int Rposibil = 8;
char current_folder[256];
bool page = 1, variabila_meniu = 0; /// 0 = meniu romana, 1 = meniu engleza
COLORREF ColorShadow = RGB(170, 0, 30);
COLORREF ActiveShadow = RGB(30, 150, 0);
COLORREF Color = RGB(200, 0, 30);     // culoarea cercului
COLORREF Active = RGB(0, 255, 0);     // culoarea posibilitatii
COLORREF Create = RGB(105, 105, 105); // patrat de vreau sa l creez
COLORREF ClearColor = RGB(10, 10, 10);  // folosit pentru a curata un patrat din ecran
COLORREF Azure = RGB(0, 128, 255);
int dx[] = {-1, 1, 0, 0};     /// vectori de directie
int dy[] = {0, 0, 1, -1};

struct Moves
{
    int xi, yi, xf, yf, dir;
};

int GetX_coord(int x);
int GetY_coord(int y);
int GetX_mat(int x);
int GetY_mat(int y);
inline bool Inmat(int x, int y);
void Posiblemove(int px[], int py[], int lg);
void ClearPosiblemove(int px[], int py[], int lg);
void Interact(int tabla[][total]);
void Start(char loc[]);
inline double dist(int xi, int yi, int xf, int yf);
void DrawCircle(int x, int y, int r, COLORREF c);
void Clear(int xi, int yi, int xf, int yf);
void MovePiece(int px[], int py[], int dir);
void Undo(int tabla[][total], Moves a[], int &n);
void CreateTable();
void SelectLvl();
void NoPossibleMoves(int tabla[][total]);

void Meniu();
inline void Meniu_Romana(int i);
inline void Meniu_Engleza(int i);
void Meniu_Highlight(int meniu,int &schimbare);
void Interactiune_Meniu();
void Setari(bool &efect, bool &muzica);
void Setari_Romana(bool efect, bool muzica);
void Setari_Engleza(bool efect, bool muzica);
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
            DrawCircle(midx - 2, midy + 2, R, ActiveShadow);
            DrawCircle(midx, midy, R, Active);
        }
        else DrawCircle(midx, midy, Rposibil, Active);

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
            DrawCircle(midx - 2, midy + 2, R, ColorShadow);
            DrawCircle(midx, midy, R, Color);
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
    itoa(n, num, 10);

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

    DrawCircle(midx - 2, midy + 2, R, ColorShadow);
    DrawCircle(midx, midy, R, Color);
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

    DrawCircle(midx - 2, midy + 2, R, ColorShadow);
    DrawCircle(midx, midy, R, Color);

    midx = GetX_mat(a[n].yi + dx[a[n].dir]) + dim / 2;
    midy = GetY_mat(a[n].xi + dy[a[n].dir]) + dim / 2;

    DrawCircle(midx - 2, midy + 2, R, ColorShadow);
    DrawCircle(midx, midy, R, Color);
}

void NoPossibleMoves(int tabla[][total])
{
    int startx = width / 6;
    int starty = height / 2;

    Clear(startx, starty, startx + 2 * dim, starty + dim);

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    setcolor(Active);

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

/**
    actualizez mutarile
*/

void Interact(int tabla[][total], int endx, int endy, bool &b, bool &r)
{
    Moves m[50];
    int px[5], py[5];   /// pozitiile valabile
    int lg = 0, nr_moves = 0, status;
    bool enable = true;
    int undox = width / 2 - dim / 2;
    int undoy = height / 2 - dim * total / 2 - dim;

    NoPossibleMoves(tabla);

    UpdateMoves(m, nr_moves);

    UpdateTime(0);

    int mx, my, ct = 0;
    HDC dc = GetDC(NULL);
    while(1)
    {
        clearmouseclick(WM_LBUTTONDOWN);
        delay(50);
        ct += 50;

        if(ct % 1000 == 0 && enable)
            UpdateTime(ct);


        if(ismouseclick(WM_LBUTTONDOWN))
        {
            mx = mousex();
            my = mousey();
            int x = GetX_coord(mx) - 1;       /// !!! x e coloana si y e linia !!!
            int y = GetY_coord(my) - 1;

            int newx = GetX_mat(x) + dim / 2;
            int newy = GetY_mat(y) + dim / 2;

            if(mx < 100)
            {
                b = true;
                return;
            }


            if(undox - dim <= mx && mx < undox && undoy <= my && my <= undoy + dim)
            {
                r = true;
                return;
            }

            if(!enable)
                continue;


            if(GetPixel(dc, newx, newy) == Color)   /// vad daca am dat click pe o piesa rosie
            {
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

                if(GetPixel(dc, newx, newy) == Active)  /// verific daca am dat click pe o pozitie valida si mut piesa
                {
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

                NoPossibleMoves(tabla);
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

    int undox = width / 2 - dim / 2;
    int undoy = height / 2 - dim * total / 2 - dim;
    readimagefile(img, undox, undoy, undox + dim, undoy + dim);

    strcpy(img, current_folder);
    strcat(img, "/res/reset.jpg");
    readimagefile(img, undox - dim + 10, undoy + 10, undox - 10, undoy + dim - 10);

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
                DrawCircle(midx - 2, midy + 2, R, ColorShadow);
                DrawCircle(midx, midy, R, Color);
            }

        }
        starty += dim;
        startx = aux;
    }

    setcolor(Active);
    number[0] = endx + '0' + 1;
    number[1] = endy + '0' + 1;
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
        settextjustify(0, 2);

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
    DrawCircle(cercx - 2, cercy + 2, R, ColorShadow);
    DrawCircle(cercx, cercy, R, Color);
    setcolor(Create);

    if(variabila_meniu) strcpy(aux, "Add circle");
    else strcpy(aux, "Adauga cerc");
    outtextxy(cercx + dim / 2 + 25, cercy, aux);

    int patratx_end, patraty_end;   /// Finalul

    patratx_end = patratx;
    patraty_end = cercy + dim;

    setcolor(Active);
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
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN, mx, my);

            if(mx < 100)
            {
                setcolor(WHITE);
                return;
            }

            /// vad ce am selectat
            if(patratx <= mx && mx <= patratx + dim && patraty <= my && my <= patraty + dim)
            {
                p = 1;      /// setez patrat alb
                c = 0;      /// setez cerc
                e = 0;      /// setez end point
                d = 0;      /// setez delete
                s = 0;      /// setez save
            }
            else if(dist(mx, my, cercx, cercy) <= R)
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
                    settextstyle(8, HORIZ_DIR, 1);
                    settextjustify(0, 2);
                    outtextxy(midx + dim / 6, midy + dim / 6, number);
                }
                else if(c == 1 && tabla[y][x] == emptys)    /// adaug un cerc
                {
                    tabla[y][x] = fills;
                    newx = GetX_mat(x);
                    newy = GetY_mat(y);
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    DrawCircle(midx - 2, midy + 2, R, ColorShadow);
                    DrawCircle(midx, midy, R, Color);
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
                        settextstyle(8, HORIZ_DIR, 1);
                        settextjustify(0, 2);
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

                    setcolor(Active);
                    rectangle(newx, newy, newx + dim, newy + dim);
                    settextstyle(8, HORIZ_DIR, 1);
                    settextjustify(0, 2);
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
                        settextstyle(8, HORIZ_DIR, 1);
                        settextjustify(0, 2);
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
                        settextstyle(8, HORIZ_DIR, 1);
                        settextjustify(0, 2);
                        outtextxy(midx + dim / 6, midy + dim / 6, number);

                        for(int k = 0; k < 4; k++)      /// refact patratele stricate
                        {
                            int i = x + dx[k];
                            int j = y + dy[k];

                            if(Inmat(i, j) && tabla[j][i] != 0)
                            {
                                if(i == ex && j == ey) setcolor(Active);
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
                    setcolor(Active);
                    rectangle(i, j, i + dim, j + dim);
                }
            }

            int lasts = s;
            while(s == 1)     /// salvez intr un fisier
            {
                setcolor(WHITE);
                settextstyle(8, HORIZ_DIR, 3);
                settextjustify(1, 1);
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

                            Clear(startx + (total + 2) * dim, height / 2 - dim / 2, width, height / 2 + dim);
                            setcolor(Azure);
                            settextjustify(1, 1);
                            settextstyle(8, HORIZ_DIR, 2);
                            outtextxy(startx + (total + 3) * dim, height / 2, nume);
                        }
                    }
                    else if(ch == 13)
                    {
                        if(lgnume == 0)
                        {
                            Clear(60, 0, width, 60);
                            setcolor(Color);
                            settextstyle(8,HORIZ_DIR,4);
                            settextjustify(CENTER_TEXT, CENTER_TEXT);
                            if(variabila_meniu) strcpy(aux, "Add a name!");
                            else strcpy(aux, "Introduceti un nume!");
                            outtextxy(width / 2, 40, aux);
                            continue;
                        }

                        if(ex == -1 || ey == -1)
                        {
                            Clear(60, 0, width, 60);
                            setcolor(Color);
                            settextstyle(8,HORIZ_DIR,4);
                            settextjustify(CENTER_TEXT, CENTER_TEXT);
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
                            settextstyle(8,HORIZ_DIR,4);
                            settextjustify(CENTER_TEXT, CENTER_TEXT);
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
                                settextjustify(1, 1);
                                settextstyle(8, HORIZ_DIR, 2);
                                outtextxy(startx + (total + 3) * dim, height / 2, nume);
                            }
                        }
                    }
                }
            }

            if(lasts && !s)
                break;

            clearmouseclick(WM_LBUTTONDOWN);
        }


        //if(GetAsyncKeyState(VK_ESCAPE)) /// inchid cand apas esc
            //break;

        delay(100);
    }

    setcolor(WHITE);
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
        settextjustify(CENTER_TEXT, CENTER_TEXT);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    bool efect=1, muzica=1;

    ///Reguli_Romana();
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
///                Reguli();
            }

            else if(x>=570 && x<=780 && y>=420 && y<=460)
            {
                setvisualpage(page);
                setactivepage(1-page);
                Setari(efect,muzica);
            }

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

///Coordonate
/*
    "Joaca"
    rectangle(575,205,715,275);

    "Cum se joaca"
    rectangle(520,285,820,350)

    "Setari"
    rectangle(550,360,760,425);

    "Iesire"
    rectangle(566,435,740,495);

    "Limba Romana"
    rectangle(375,185,700,410);

    "Limba Engleza"
    rectangle(800,185,1120,410);

    "Efecte Sonore"
    rectangle(380,450,690,670);

    "Muzica"
    rectangle(875,450,1045,660);
*/

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
    delay(5);
}

void Setari(bool &efect, bool &muzica)
{
    int x,y;

    while(1)
    {
        setvisualpage(page);
        setactivepage(1-page);

        cleardevice();
        Sageata();

        if(!variabila_meniu)
            Setari_Romana(efect,muzica);
        else
            Setari_Engleza(efect, muzica);

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            x=mousex();
            y=mousey();

            if(x>=375 && x<=700 && y>=185 && y<=410 && variabila_meniu)
            {
                variabila_meniu=0;
                return;
            }

            else if(x>=800 && x<=1120 && y>=185 && y<=410 && !variabila_meniu)
            {
                variabila_meniu=1;
                return;
            }

            else if(x>=380 && x<=690 && y>=450 && y<=670)
                efect=1-efect;

            else if(x>=875 && x<=1045 && y>=450 && y<=660)
                if(muzica)
                    muzica=0;
                else
                    muzica=1;

            else if(x<=350)
                return;
        }

        page = 1 - page;
    }
}


void Setari_Romana(bool efect, bool muzica)
{
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(630,60,"Setari");

    settextstyle(8,HORIZ_DIR,5);
    outtextxy(170,150,"Limba:");
    outtextxy(170,420,"Sunet:");

    settextstyle(8,HORIZ_DIR,4);

    setcolor(Color);
    outtextxy(490,370,"Ro");
    setcolor(YELLOW);
    outtextxy(530,370,"ma");
    setcolor(Azure);
    outtextxy(570,370,"na");

    char img[256];
    strcpy(img, current_folder);
    strcat(img, "/res/romana.jpg");

    readimagefile(img,385,195,685,345);

    strcpy(img, current_folder);
    strcat(img, "/res/english.jpg");
    readimagefile(img,810,195,1110,345);

    if(efect)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,470,460,610,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,470,460,610,600);
    }
    outtextxy(410,620,"Efecte sonore");

    if(muzica)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,895,460,1035,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,895,460,1035,600);
    }
    outtextxy(895,620,"Muzica");

    setcolor(WHITE);
    outtextxy(880,370,"English");
}

void Setari_Engleza(bool efect, bool muzica)
{
    setcolor(WHITE);
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(600,60,"Settings");

    settextstyle(8,HORIZ_DIR,5);
    outtextxy(170,150,"Language:");
    outtextxy(170,420,"Sound:");

    settextstyle(8,HORIZ_DIR,4);
    setcolor(Azure);
    outtextxy(900,370,"En");
    setcolor(Color);
    outtextxy(953,370,"gli");
    setcolor(WHITE);
    outtextxy(1000,370,"sh");

    char img[256];
    strcpy(img, current_folder);
    strcat(img, "/res/romana.jpg");

    readimagefile(img,385,195,685,345);

    strcpy(img, current_folder);
    strcat(img, "/res/english.jpg");
    readimagefile(img,810,195,1110,345);

    if(efect)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,470,460,610,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,470,460,610,600);
    }
    outtextxy(410,620,"Sound effects");

    if(muzica)
    {
        setcolor(Active);
        strcpy(img, current_folder);
        strcat(img, "/res/tic.jpg");
        readimagefile(img,895,460,1035,600);
    }
    else
    {
        setcolor(Color);
        strcpy(img, current_folder);
        strcat(img, "/res/cross.jpg");
        readimagefile(img,895,460,1035,600);
    }
    outtextxy(895,620,"Music");

    setcolor(WHITE);
    outtextxy(445,370,"Romana");
}

///in lucru
void Reguli()
{
    int x;

    while(1)
    {
        setvisualpage(page);
        setactivepage(1-page);

        cleardevice();
        Sageata();

        if(!variabila_meniu)
            Reguli_Romana();

        if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            x=mousex();

            if(x<=350)
                return;
        }
        page=1-page;
    }
}
///in lucru
void Reguli_Romana()
{
    Sageata();
    settextstyle(8,HORIZ_DIR,6);
    outtextxy(520,60,"Cum se joaca");

    settextstyle(8,HORIZ_DIR,4);
    outtextxy(170,150,"1) Pe tabla");
    getch();
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
    Meniu();
    return 0;
}

