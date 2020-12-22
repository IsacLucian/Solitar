#include <iostream>
#include <fstream>
#include <graphics.h>
#include <math.h>

using namespace std;

const int total = 7;    // dimensiunea matricei
int fills = 1;    // fill square
int emptys = 2;   // empty square
int width = 1350;
int height = 700;
int dim = 70;     // dimensiunea unui patrat
int R = 19;       // raza cerculi
int Rposibil = 10;
COLORREF Color = RGB(200, 1, 30);     // culoarea cercului
COLORREF Active = RGB(0, 255, 0);     // culoarea posibilitatii
COLORREF Create = RGB(105, 105, 105); // patrat de vreau sa l creez
int dx[] = {-1, 1, 0, 0};     /// vectori de directie
int dy[] = {0, 0, 1, -1};

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

/**
    transform din coordonata xOy in coordonata din matrice
*/
int GetX_coord(int x)
{
    int startx = width / 2 - dim * total / 2 - dim;
    int endx = startx + 8 * dim;
    if(x < startx || x > endx) return -1;

    return (x - startx) / dim;
}

int GetY_coord(int y)
{
    int starty = height / 2 - dim * total / 2 - dim;
    int endy = starty + 8 * dim;

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

    int mx, my;
    HDC dc = GetDC(NULL);
    while(1)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN, mx, my);


            int x = GetX_coord(mx) - 1;       /// !!! x e coloana si y e linia !!!
            int y = GetY_coord(my) - 1;

            int newx = GetX_mat(x) + dim / 2;
            int newy = GetY_mat(y) + dim / 2;

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

        if(GetAsyncKeyState(VK_ESCAPE)) /// inchid cand apas esc
            break;

        delay(100);
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


    int startx = width / 2 - dim * total / 2 - dim;
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

    setcolor(Active);
    number[0] = endx + '0' + 1;
    number[1] = endy + '0' + 1;
    startx = GetX_mat(endx);
    starty = GetY_mat(endy);
    rectangle(startx, starty, startx + dim, starty + dim);
    midx = startx + dim / 2;
    midy = starty + dim / 2;

    outtextxy(midx + dim / 4, midy + dim / 4, number);

    Interact(tabla);
}

inline double dist(int xi, int yi, int xf, int yf)
{
    return sqrt((xi - xf) * (xi - xf) + (yi - yf) * (yi - yf));
}

/**
    Tabla customizabila
*/

void CreateTable()
{
    int tabla[total][total];
    memset(tabla, 0, sizeof(tabla));

    int patratx, patraty;       /// Patrat gol
    patratx = 50;
    patraty = height / 2 - dim;

    setcolor(WHITE);
    rectangle(patratx, patraty, patratx + dim, patraty + dim);
    setcolor(Create);
    outtextxy(patratx + dim + 25, patraty + dim / 2, "Adauga patrat");

    int cercx, cercy;       /// Cerc
    cercx = patratx + dim / 2;
    cercy = patraty + 2 * dim;

    setcolor(Color);
    circle(cercx, cercy, R);
    setfillstyle(SOLID_FILL, Color);
    floodfill(cercx, cercy, Color);
    setcolor(Create);
    outtextxy(cercx + dim / 2 + 25, cercy, "Adauga cerc");

    int patratx_end, patraty_end;   /// Finalul

    patratx_end = patratx;
    patraty_end = cercy + dim;

    setcolor(Active);
    rectangle(patratx_end, patraty_end, patratx_end + dim, patraty_end + dim);

    setcolor(Create);
    outtextxy(patratx_end + dim + 25, patraty_end + dim / 2, "Adauga end point");

    int deletex, deletey;       /// Buton de delete
    deletex = patratx;
    deletey = patraty - dim;

    setcolor(RED);
    rectangle(deletex, deletey, deletex + 2 * dim, deletey + dim / 2);
    outtextxy(deletex + dim / 2, deletey + dim / 8, "Sterge");
    setcolor(Create);

    int savex, savey;       /// Buton de delete
    savex = deletex;
    savey = deletey - dim;

    setcolor(BLUE);
    rectangle(savex, savey, savex + 2 * dim, savey + dim / 2);
    outtextxy(savex + dim / 2, savey + dim / 8, "Salveaza");
    setcolor(Create);

    int startx = width / 2 - dim * total / 2 - dim;
    int starty = height / 2 - dim * total / 2;
    int aux = startx;
    int midx, midy;
    char number[3];

    for(int i = 0; i < total; i++)
    {
        for(int j = 0; j < total; j++)
        {
            startx += dim;
            number[0] = i + '0' + 1;
            number[1] = j + '0' + 1;
            rectangle(startx, starty, startx + dim, starty + dim);
            midx = startx + dim / 2;
            midy = starty + dim / 2;

            outtextxy(midx + dim / 4, midy + dim / 4, number);
        }
        starty += dim;
        startx = aux;
    }

    int ex = -1, ey = -1;
    int mx, my, p = 0, c = 0, e = 0, d = 0, s = 0;
    while(1)
    {
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN, mx, my);

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
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    setcolor(WHITE);
                    rectangle(newx, newy, newx + dim, newy + dim);
                    outtextxy(midx + dim / 4, midy + dim / 4, number);
                }
                else if(c == 1 && tabla[y][x] == emptys)    /// adaug un cerc
                {
                    tabla[y][x] = fills;
                    newx = GetX_mat(x);
                    newy = GetY_mat(y);
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    setcolor(Color);
                    circle(midx, midy, R);
                    setfillstyle(SOLID_FILL, Color);
                    floodfill(midx, midy, Color);
                    setcolor(WHITE);
                }
                else if(e == 1 && (tabla[y][x] == emptys || tabla[y][x] == fills))
                {
                    if(ex != -1 && ey != -1) /// clear patrat sfarsit
                    {
                        newx = GetX_mat(ex);
                        newy = GetY_mat(ey);

                        number[0] = ey + '0' + 1;
                        number[1] = ex + '0' + 1;
                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setcolor(WHITE);
                        rectangle(newx, newy, newx + dim, newy + dim);
                        outtextxy(midx + dim / 4, midy + dim / 4, number);
                    }

                    ex = x;         /// adaug un patrat verde
                    ey = y;
                    newx = GetX_mat(ex);
                    newy = GetY_mat(ey);

                    number[0] = ey + '0' + 1;
                    number[1] = ex + '0' + 1;
                    midx = newx + dim / 2;
                    midy = newy + dim / 2;

                    setcolor(Active);
                    rectangle(newx, newy, newx + dim, newy + dim);
                    outtextxy(midx + dim / 4, midy + dim / 4, number);

                }
                else if(d == 1)
                {
                    if(x == ex && y == ey)      /// elimin patratul de final
                    {
                        newx = GetX_mat(ex);
                        newy = GetY_mat(ey);

                        number[0] = ey + '0' + 1;
                        number[1] = ex + '0' + 1;
                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setcolor(WHITE);
                        rectangle(newx, newy, newx + dim, newy + dim);
                        outtextxy(midx + dim / 4, midy + dim / 4, number);
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
                        midx = newx + dim / 2;
                        midy = newy + dim / 2;

                        setcolor(Create);
                        rectangle(newx, newy, newx + dim, newy + dim);
                        outtextxy(midx + dim / 4, midy + dim / 4, number);

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

            if(s == 1)     /// salvez intr un fisier
            {
                ofstream fout("test.in");

                fout << ex << " " << ey << "\n";
                for(int i = 0; i < total; i++, fout << "\n")
                    for(int j = 0; j < total; j++)
                        fout << tabla[i][j] << " ";

                fout.close();
                break;
            }

            clearmouseclick(WM_LBUTTONDOWN);
        }
        if(GetAsyncKeyState(VK_ESCAPE)) /// inchid cand apas esc
            break;
    }
}

void Main()
{
    RECT desktop;       /// iau rezolutia ecranului
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    height = desktop.bottom;
    width = desktop.right;

    cout << "TEST 1 sau 2:";
    int x;
    cin >> x;

    initwindow(width, height, "", -3, -3);

    if(x == 1)
        Start("test.in");
    else if(x == 2)
        CreateTable();

    closegraph();
}

int main()
{
    Main();
    return 0;
}
