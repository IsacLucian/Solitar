#include <iostream>
#include <fstream>
#include <graphics.h>
#include <math.h>

using namespace std;

const int fills = 1;    // fill square
const int emptys = 2;   // empty square
const int width = 1350;
const int height = 700;
const int total = 7;    // dimensiunea matricei
const int dim = 70;     // dimensiunea unui patrat
const int R = 19;       // raza cerculi
const int Rposibil = 10;
const COLORREF Color = RGB(200, 1, 30);     // culoarea cercului
const COLORREF Active = RGB(0, 255, 0);     // culoarea posibilitatii

int GetX_coord(int x);
int GetY_coord(int y);
int GetX_mat(int x);
int GetY_mat(int y);
inline bool Inmat(int x, int y);
void Posiblemove(int px[], int py[], int lg);
void ClearPosiblemove(int px[], int py[], int lg);
void Interact(int tabla[][total]);
void Start(char loc[]);
void MainProgram();

/**
    transform din coordonata xOy in coordonata din matrice
*/
int GetX_coord(int x)
{
    int startx = width / 2 - dim * total / 2;
    int endx = startx + 8 * dim;
    if(x < startx || x > endx) return -1;

    return (x - startx) / dim;
}

int GetY_coord(int y)
{
    int starty = height / 2 - dim * total / 2;
    int endy = starty + 7 * dim + dim / 2;

    if(y < starty || y > endy) return -1;

    return (y - starty + dim / 2) / dim;
}

/**
    transform din coordonata din matrice in coordonata xOy
*/

int GetX_mat(int x)
{
    int startx = width / 2 - dim * total / 2;

    return startx + dim * (x + 1);
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
    for(int i = 0; i < lg - 1; i++)
    {
        int midx = GetX_mat(px[i]) + dim / 2;
        int midy = GetY_mat(py[i]) + dim / 2;

        setcolor(Active);
        circle(midx, midy, Rposibil);
        setfillstyle(SOLID_FILL, Active);
        floodfill(midx, midy, Active);
        setcolor(WHITE);
    }
}

void ClearPosiblemove(int px[], int py[], int lg)
{
    for(int i = 0; i < lg - 1; i++)
    {
        if(px[i] == -1 || py[i] == -1)
            continue;

        int midx = GetX_mat(px[i]) + dim / 2;
        int midy = GetY_mat(py[i]) + dim / 2;

        setfillstyle(SOLID_FILL, BLACK);
        floodfill(midx, midy, BLACK);
    }
}

/**
    actualizez mutarile
*/

void Interact(int tabla[][total])
{
    int px[5], py[5], lg = 0;   /// pozitiile valabile
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    POINT pt;
    HDC dc = GetDC(NULL);
    while(1)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            GetCursorPos(&pt);

            int x = GetX_coord(pt.x) - 1;       /// !!! x e coloana si y e linia !!!
            int y = GetY_coord(pt.y) - 1;

            int newx = GetX_mat(x) + dim / 2 ;
            int newy = GetY_mat(y) + dim / 2 + dim / 4;

            if(GetPixel(dc, newx, newy) == Color)   /// vad daca am dat click pe o piesa rosie
            {
                ClearPosiblemove(px, py, lg);
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
                Posiblemove(px, py, lg);
            }
            else {

                if(GetPixel(dc, newx, newy) == Active)
                {

                    for(int k = 0; k < 4; k++)
                        if(px[lg - 1] + 2 * dx[k] == x && py[lg - 1] + 2 * dy[k] == y)
                        {
                            tabla[py[lg - 1]][px[lg - 1]] = emptys;
                            tabla[py[lg - 1] + dy[k]][px[lg - 1] + dx[k]] = emptys;
                            tabla[y][x] = fills;

                            for(int j = 0; j < lg - 1; j++)
                                if(px[j] == x && py[j] == y) px[j] = py[j] = -1;

                            int midx = GetX_mat(px[lg - 1]) + dim / 2;
                            int midy = GetY_mat(py[lg - 1]) + dim / 2;

                            setfillstyle(SOLID_FILL, BLACK);
                            floodfill(midx, midy, BLACK);

                            midx = GetX_mat(px[lg - 1] + dx[k]) + dim / 2;
                            midy = GetY_mat(py[lg - 1] + dy[k]) + dim / 2;

                            setfillstyle(SOLID_FILL, BLACK);
                            floodfill(midx, midy, BLACK);
                            midx = GetX_mat(x) + dim / 2;
                            midy = GetY_mat(y) + dim / 2;

                            setcolor(Color);
                            circle(midx, midy, R);
                            setfillstyle(SOLID_FILL, Color);
                            floodfill(midx, midy, Color);
                            setcolor(WHITE);

                            break;
                        }
                }
                ClearPosiblemove(px, py, lg);
            }
            clearmouseclick(WM_LBUTTONDOWN);
        }
        delay(100);
    }

}

/**
    generez tabla de joc
*/

void Start(char loc[])
{
    ifstream fin(loc);
    int tabla[total][total];

    for(int i = 0; i < total; i++)
        for(int j = 0; j < total; j++)
            fin >> tabla[i][j];

    initwindow(width, height);

    int startx = width / 2 - dim * total / 2;
    int starty = height / 2 - dim * total / 2;
    int aux = startx, midx, midy;
    char number[3];

    for(int i = 0; i < total; i++)
    {
        for(int j = 0; j < total; j++)
        {
            startx += dim;
            if(!tabla[i][j])
                continue;

            number[0] = i + '0' + 1;
            number[1] = j + '0' + 1;
            rectangle(startx, starty, startx + dim, starty + dim);
            midx = startx + dim / 2;
            midy = starty + dim / 2;


            outtextxy(midx + dim / 4, midy + dim / 4, number);

            if(tabla[i][j] == fills)
            {
                setcolor(Color);
                circle(midx, midy, R);
                setfillstyle(SOLID_FILL, Color);
                floodfill(midx, midy, Color);
                setcolor(WHITE);
            }

        }
        starty += dim;
        startx = aux;
    }


    Interact(tabla);
    getch();
    closegraph();
    fin.close();
}


int main()
{
    Start("test.in");
    return 0;
}
