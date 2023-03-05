#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define Maksymalna_liczba_statkow 10
#define Maksymalna_dlugosc_statku 5
#define n_typow_statkow 4

using namespace std;

enum nazwa_statku_t { CAR, BAT, CRU, DES };
enum typ_pola_t { WODA, RAFA, STATEK, USZKODENIE, SILNIK, DZIALO, RADAR, SILNIK_DZIALO };

struct pozycja_t { int y; int x; };

struct parametry_statku_t
{
    enum nazwa_statku_t rodzaj;
    int dlugosc_statku;
};

struct statek_t {
    struct parametry_statku_t parametry;
    int wlasciciel=3;
    int id=-1;
    char kierunek;
    struct pozycja_t poczatek_statku;
    bool uszkodzenia[5] = { false,false,false,false,false, };
    int liczba_ruchow_w_rundzie=0;
    int liczba_strzalow_w_rundzie=0;
};


struct zawartosc_pola_t
{
    typ_pola_t typ;
    int numer_statku;
    bool odkrycie_przez_graczy[2] = { false, false };
};


int n_raf = 0;
int n_statkow = 0;
int dozwolne_pozycje[2][4];
int y_wysokosc_planszy = 21;
int x_szerokosc_planszy = 10;
int ilosc_statkow_graczy[2][n_typow_statkow];
int poczatkowa_pula_statkow[2][n_typow_statkow] = { 4,3,2,1 };

zawartosc_pola_t** plansza;
statek_t lista_statkow[2 * n_typow_statkow * Maksymalna_liczba_statkow];
pozycja_t* rafy = new pozycja_t[y_wysokosc_planszy * x_szerokosc_planszy];

void przelicz_rafy();
void strzal(int, int);
void inicjalizowanie();
void ustawienie_rafy();
void wymiary_planszy();
void wczytanie_statku();
void obsluga_ruchu(int);
void przelicz_plansze();
void ustawienia_floty();
void obsluga_zapisu(int*);
void drukuj_plansze(int,int);
void inicjalizacja_pozycji();
void stawianie_statku(int, int);
void podstawowe_strzelanie(int);
void zaawansowane_strzelanie(int);
void zaawansowane_drukowanie(int);
void podstawowe_szpiegowanie(int);
void zaawansowane_szpiegowanie(int);
void obsluga_komend_gracza(int, int);
void obsluga_komend_stanu(int*, int*);
void zerownie_liczby_ruchow_strzalow();
void szukaj_pozycji_dziala(int, int[]);
void definiowanie_statku(nazwa_statku_t);
void odkrycie_planszy(int, int, int, double);
void blad_stawiania(int,int, int, int, char, int, char*, char*);

int ruch(int, char);
int pozostale_czesci_statku(int);
int szukaj_po_id(int, int, char[]);
int dlugosc_statku(nazwa_statku_t);
int postawienie_wszystkich_statkow();
int postawienie_na_rafie(int, int, char, int);
int walidacja_ruchu(int, int, int, char, int);
int wyplyniecie_poza_plansze(int, int, char, int);
int poprawnosc_lokalizacji(int, int, int, char, int);
int bliskosc_innych_statkow(int, int, int, char, int);
int obecnosc_statku_na_planszy(int, nazwa_statku_t, int);

double odleglosc(int, int, int, int);



int main()
{
    inicjalizowanie();
    int gracz = 3;
    int tryb_rozszerzony = 0;

    char komenda[20];

    while (cin >> komenda) {
        if (strcmp(komenda, "[playerA]") == 0) {
            if (gracz == 0) cout << "INVALID OPERATION \"" << komenda << " \": THE OTHER PLAYER EXPECTED" << endl;
            gracz = 0;
            obsluga_komend_gracza(gracz, tryb_rozszerzony);
        }
        else if (strcmp(komenda, "[playerB]") == 0) {
            if (gracz == 1)cout << "INVALID OPERATION \"" << komenda << " \": THE OTHER PLAYER EXPECTED" << endl;
            gracz = 1;
            obsluga_komend_gracza(gracz, tryb_rozszerzony);
        }

        else if (strcmp(komenda, "[state]") == 0) {
            obsluga_komend_stanu(&gracz, &tryb_rozszerzony);
        }
    }

    for (int i = 0; i < y_wysokosc_planszy; i++)
        delete[] plansza[i];

    delete[] plansza;
    return 0;
}

void obsluga_komend_gracza(int gracz, int tryb_rozszerzony) {
    char komenda[15];
    while (1) {
        cin >> komenda;
        if (strcmp(komenda, "PLACE_SHIP") == 0) {
            stawianie_statku(0, gracz);
        }
        else if (strcmp(komenda, "SHOOT") == 0) {
            if (tryb_rozszerzony == 0)podstawowe_strzelanie(gracz);
            else zaawansowane_strzelanie(gracz);
        }
        if (strcmp(komenda, "SPY") == 0) {
            zaawansowane_szpiegowanie(gracz);
        }
        else if (strcmp(komenda, "MOVE") == 0) {
            obsluga_ruchu(gracz);
        }
        else if (strcmp(komenda, "PRINT") == 0) {
            int parametr;
            cin >> parametr;
            drukuj_plansze(parametr, gracz);
        }
        else if (gracz == 0 && (strcmp(komenda, "[playerA]") == 0) || (gracz == 1 && (strcmp(komenda, "[playerB]") == 0))) {
            zerownie_liczby_ruchow_strzalow();
            break;
        }
        else if (gracz == 1 && (strcmp(komenda, "[playerA]") == 0) || (gracz == 0 && (strcmp(komenda, "[playerB]") == 0))) {
            cout << "INVALID OPERATION \"" << komenda << " \": THE OTHER PLAYER EXPECTED" << endl;
            exit(1);
        }
    }
}
void obsluga_komend_stanu(int* gracz, int* tryb_rozszerzony) {
    char komenda[15];

    while (1) {
        cin >> komenda;
        int parametr;
        if (strcmp(komenda, "PRINT") == 0) {
            cin >> parametr;
            drukuj_plansze(parametr,3);
        }
        if (strcmp(komenda, "SAVE") == 0) {
            obsluga_zapisu(gracz);
        }
        if (strcmp(komenda, "SET_FLEET") == 0) {
            ustawienia_floty();
        }
        if (strcmp(komenda, "INIT") == 0) {
            wymiary_planszy();
        }
        if (strcmp(komenda, "INIT_POSITION") == 0) {
            inicjalizacja_pozycji();
        }
        if (strcmp(komenda, "REEF") == 0) {
            ustawienie_rafy();
        }
        if (strcmp(komenda, "SHIP") == 0) {
            wczytanie_statku();
        }
        if (strcmp(komenda, "EXTENDED_SHIPS") == 0) {
            if (*tryb_rozszerzony == 0) { *tryb_rozszerzony = 1; }
            else *tryb_rozszerzony = 0;
        }
        if (strcmp(komenda, "NEXT_PLAYER") == 0) {
            char p;
            cin >> p;
            if (p == 'A') *gracz = 1;
            else if (p == 'B') *gracz = 0;
        }
        if (strcmp(komenda, "[state]") == 0)  break;
    }
}
void obsluga_ruchu(int gracz) {
    int id;
    char typ[4];
    char typ_ruchu;
    cin >> id;
    cin >> typ;
    int numer = szukaj_po_id(id, gracz, typ);
    cin >> typ_ruchu;
    int wynik = ruch(numer, typ_ruchu);
    lista_statkow[numer].liczba_ruchow_w_rundzie++;
    if (lista_statkow[numer].liczba_ruchow_w_rundzie>3||(lista_statkow[numer].parametry.rodzaj==CAR &&lista_statkow[numer].liczba_ruchow_w_rundzie>2)) {
        cout << "INVALID OPERATION \"MOVE " << id << " " << typ << " " << typ_ruchu << "\": SHIP MOVED ALREADY"<<endl;
        exit(1);
    }
    if (wynik == 1) {
        cout << "INVALID OPERATION \"MOVE " << id << " " << typ << " " << typ_ruchu << "\": PLACING SHIP TOO CLOSE TO OTHER SHIP" << endl;
        exit(1);
    }
    if (wynik == 2) {
        cout << "INVALID OPERATION \"MOVE " << id << " " << typ << " " << typ_ruchu << "\": SHIP WENT FROM BOARD" << endl;
        exit(1);
    }
    if (wynik == 3) {
        cout << "INVALID OPERATION \"MOVE " << id << " " << typ << " " << typ_ruchu << "\": SHIP CANNOT MOVE" << endl;
        exit(1);
    }
    if (wynik == 4) {
        cout << "INVALID OPERATION \"MOVE " << id << " " << typ << " " << typ_ruchu << "\": PLACING SHIP ON REEF" << endl;
        exit(1);
    }
    przelicz_plansze();

}

void inicjalizowanie() {
    plansza = new zawartosc_pola_t * [y_wysokosc_planszy];
    for (int i = 0; i < y_wysokosc_planszy; i++)
    {
        plansza[i] = new zawartosc_pola_t[x_szerokosc_planszy];
    }

    for (int i = 0; i < y_wysokosc_planszy; i++)
    {
        for (int j = 0; j < x_szerokosc_planszy; j++)
        {
            plansza[i][j].typ = WODA;

        }
    }
    for (int i = 0; i < 2; i++)
    {
        poczatkowa_pula_statkow[i][0] = 4;
        poczatkowa_pula_statkow[i][1] = 3;
        poczatkowa_pula_statkow[i][2] = 2;
        poczatkowa_pula_statkow[i][3] = 1;
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < n_typow_statkow; j++)
        {
            ilosc_statkow_graczy[i][j] = 0;
        }
    }
    dozwolne_pozycje[0][0] = 0;
    dozwolne_pozycje[0][1] = 0;
    dozwolne_pozycje[0][2] =  ((y_wysokosc_planszy - 1) / 2)-1;
    dozwolne_pozycje[0][3] = x_szerokosc_planszy - 1;
    dozwolne_pozycje[1][0] = ((y_wysokosc_planszy - 1) / 2) + 1;
    dozwolne_pozycje[1][1] = 0;
    dozwolne_pozycje[1][2] = y_wysokosc_planszy-1;
    dozwolne_pozycje[1][3] = x_szerokosc_planszy-1;
}
void ustawienia_floty() {
    char gracz_char;
    int gracz;
    cin >> gracz_char;
    if (gracz_char == 'A') { gracz = 0; }
    if (gracz_char == 'B') { gracz = 1; }
    for (int i = n_typow_statkow - 1; i >= 0; i--)
    {
        cin >> poczatkowa_pula_statkow[gracz][i];
    }
}
void wymiary_planszy() {
    cin >> y_wysokosc_planszy;
    cin >> x_szerokosc_planszy;
}
void inicjalizacja_pozycji() {
    char gracz_ch;
    int gracz;
    cin >> gracz_ch;
    if (gracz_ch == 'A')gracz = 0;
    else if (gracz_ch == 'B')gracz = 1;
    else {
        cout << "INVALID PLAYER NAME";
        exit(1);
    }
    cin >> dozwolne_pozycje[gracz][0];
    cin >> dozwolne_pozycje[gracz][1];
    cin >> dozwolne_pozycje[gracz][2];
    cin >> dozwolne_pozycje[gracz][3];
}
void ustawienie_rafy() {
    int y;
    int x;
    cin >> y;
    cin >> x;
    rafy[n_raf].y = y;
    rafy[n_raf].x = x;
    przelicz_rafy();
    n_raf++;
}
void zaawansowane_szpiegowanie(int gracz ){
    int id, y, x;
    cin >> id;
    cin >> y;
    cin >> x;
    char nazwa[4] = { 'C','A','R'};
    int numer = szukaj_po_id(id, gracz, nazwa);
    if (lista_statkow[numer].uszkodzenia[1] == true) {
        cout << "INVALID OPERATION \"SPY " << id << " " << y << " " << x << "\": CANNOT SEND PLANE";
        exit(1);
    }
    if (lista_statkow[numer].liczba_strzalow_w_rundzie == lista_statkow[numer].parametry.dlugosc_statku) {
        cout << "INVALID OPERATION \"SPY " << id << " " << y << " " << x << "\": ALL PLANES SENT";
        exit(1);
    }
    odkrycie_planszy(gracz, y, x, 1.5);
    lista_statkow[numer].liczba_strzalow_w_rundzie++;
}
void podstawowe_szpiegowanie(int gracz) {
    int y,x;
    cin >> y;
    cin >> x;
    odkrycie_planszy(gracz, y, x, 1.5);
}
void zaawansowane_strzelanie(int gracz) {
    int id, y, x, dlugosc;
    char typ[4];
    int pozycja_dziala[2];
    cin >> id;
    cin >> typ;
    int n = szukaj_po_id(id, gracz, typ);
    szukaj_pozycji_dziala(n, pozycja_dziala);
    dlugosc = lista_statkow[n].parametry.dlugosc_statku;
    cin >> y;
    cin >> x;

    if (lista_statkow[n].uszkodzenia[1] == true) {
        cout << "INVALID OPERATION \"SHOOT " << id << " " << typ << " " << y << " " << x << "\": SHIP CANNOT SHOOT" << endl;
        exit(1);
    }
    if (lista_statkow[n].liczba_strzalow_w_rundzie == dlugosc) {
        cout << "INVALID OPERATION \"SHOOT " << id << " " << typ << " " << y << " " << x << "\": TOO MANY SHOOTS" << endl;
        exit(1);
    }
    if (odleglosc(pozycja_dziala[0], pozycja_dziala[1], y, x) > dlugosc) {
        cout << "INVALID OPERATION \"SHOOT " << id << " " << typ << " " << y << " " << x << "\": SHOOTING TOO FAR" << endl;
        exit(1);
    }
    strzal(y, x);
    lista_statkow[n].liczba_strzalow_w_rundzie++;
}

void podstawowe_strzelanie(int gracz) {
    int y;
    int x;
    cin >> y;
    cin >> x;
    strzal(y, x);
}

void strzal(int y, int x) {
    if (y<0 || y>y_wysokosc_planszy || x < 0 || x > x_szerokosc_planszy) {
        cout << "INVALID OPERATION \"SHOOT " << y << " " << x << "\": FIELD DOES NOT EXIST" << endl;
        exit(1);
    }
    if (postawienie_wszystkich_statkow() == 1) {
        cout << "INVALID OPERATION \"SHOOT " << y << " " << x << "\": NOT ALL SHIPS PLACED" << endl;
        exit(1);
    }
    if (plansza[y][x].typ != WODA && plansza[y][x].typ != RAFA && plansza[y][x].typ != USZKODENIE) {
        int numer = plansza[y][x].numer_statku;
        int dlugosc = lista_statkow[numer].parametry.dlugosc_statku;
        if (plansza[y][x].typ == STATEK) {
            int y2 = lista_statkow[numer].poczatek_statku.y;
            int x2 = lista_statkow[numer].poczatek_statku.x;
            if (odleglosc(y, x, y2, x2) <= 2)lista_statkow[numer].uszkodzenia[2] = true;
            else lista_statkow[numer].uszkodzenia[3] = true;
        }
        else if (plansza[y][x].typ == RADAR) {
            lista_statkow[numer].uszkodzenia[0] = true;
        }
        else if (plansza[y][x].typ == SILNIK_DZIALO) {
            lista_statkow[numer].uszkodzenia[1] = true;
        }
        else if (plansza[y][x].typ == SILNIK) {
            lista_statkow[numer].uszkodzenia[dlugosc - 1] = true;
        }
        else if (plansza[y][x].typ == DZIALO) {
            lista_statkow[numer].uszkodzenia[1] = true;
        }
    }
    przelicz_plansze();
    if (pozostale_czesci_statku(1) == 0) {
        cout << "A won" << endl;
        exit(1);
    }
    if (pozostale_czesci_statku(0) == 0) {
        cout << "B won" << endl;
        exit(1);
    }
}

void wczytanie_statku() {
    char gracz_ch;
    int gracz = 3;
    cin >> gracz_ch;
    if (gracz_ch == 'A')gracz = 0;
    else if (gracz_ch == 'B')gracz = 1;
    stawianie_statku(1, gracz);
    przelicz_plansze();
}

void stawianie_statku(int parametr, int gracz) {
    int y, x, id, dlugosc = 0;
    char nazwa[4];
    char stan[6];
    char kierunek;

    cin >> y;
    cin >> x;
    cin >> kierunek;
    cin >> id;
    cin >> nazwa;
    if (parametr == 1) {
        cin >> stan;
        for (int i = 0; i < (strlen(stan)); i++)
        {
            if (stan[i] == '0')lista_statkow[n_statkow].uszkodzenia[i] = true;
        }
    }
    nazwa_statku_t moj_typ_statku = DES;
    if ((strcmp(nazwa, "CAR") == 0)) {
        moj_typ_statku = nazwa_statku_t::CAR;
        dlugosc = dlugosc_statku(nazwa_statku_t::CAR);
    }
    else if ((strcmp(nazwa, "BAT") == 0)) {
        moj_typ_statku = nazwa_statku_t::BAT;
        dlugosc = dlugosc_statku(nazwa_statku_t::BAT);
    }
    else if ((strcmp(nazwa, "CRU") == 0)) {
        moj_typ_statku = nazwa_statku_t::CRU;
        dlugosc = dlugosc_statku(nazwa_statku_t::CRU);
    }
    else if ((strcmp(nazwa, "DES") == 0)) {
        moj_typ_statku = nazwa_statku_t::DES;
        dlugosc = dlugosc_statku(nazwa_statku_t::DES);
    }

    if (poprawnosc_lokalizacji(gracz, y, x, kierunek, dlugosc_statku(moj_typ_statku)) == 1) {
        blad_stawiania(parametr, gracz, y, x, kierunek, id, nazwa, stan);
        cout << "NOT IN STARTING POSITION" << endl;
        exit(1);
    }
    if (obecnosc_statku_na_planszy(gracz, moj_typ_statku, id)) {
        blad_stawiania(parametr,gracz, y, x, kierunek, id, nazwa,stan);
        cout<<"SHIP ALREADY PRESENT" << endl;
        exit(1);
    }
    if (ilosc_statkow_graczy[gracz][dlugosc - 2] == poczatkowa_pula_statkow[gracz][dlugosc - 2]) {
        blad_stawiania(parametr, gracz, y, x, kierunek, id, nazwa, stan);
        cout << "ALL SHIPS OF THE CLASS ALREADY SET" << endl;
        exit(1);
    }
    if (bliskosc_innych_statkow(n_statkow, y, x, kierunek, dlugosc_statku(moj_typ_statku)) == 1) {
        blad_stawiania(parametr, gracz, y, x, kierunek, id, nazwa, stan);
        cout << "PLACING SHIP TOO CLOSE TO OTHER SHIP" << endl;
        exit(1);
    }
    if (postawienie_na_rafie(y, x, kierunek, dlugosc_statku(moj_typ_statku)) == 1) {
        blad_stawiania(parametr, gracz, y, x, kierunek, id, nazwa, stan);
        cout << "PLACING SHIP ON REEF" << endl;
        exit(1);
    }

    lista_statkow[n_statkow].wlasciciel = gracz;
    lista_statkow[n_statkow].poczatek_statku.y = y;
    lista_statkow[n_statkow].poczatek_statku.x = x;
    lista_statkow[n_statkow].kierunek = kierunek;
    lista_statkow[n_statkow].id = id;
    definiowanie_statku(moj_typ_statku);

    ilosc_statkow_graczy[gracz][dlugosc_statku(moj_typ_statku) - 2]++;
    n_statkow++;
    przelicz_plansze();

}
int walidacja_ruchu(int numer, int y, int x, char kierunek, int dlugosc) {
    if ((bliskosc_innych_statkow(numer, y - 1, x, kierunek, dlugosc)) == 1) return 1;
    if ((wyplyniecie_poza_plansze(y,x, kierunek, dlugosc)) == 0) return 2;
    if (postawienie_na_rafie(y, x, kierunek, dlugosc) == 1)return 4;
    else return 0;
}
int ruch(int numer, char manewr) {
    int y = lista_statkow[numer].poczatek_statku.y;
    int x = lista_statkow[numer].poczatek_statku.x;
    int dlugosc = lista_statkow[numer].parametry.dlugosc_statku;
    if (lista_statkow[numer].uszkodzenia[dlugosc - 1] == true)return 3;
    if (lista_statkow[numer].kierunek == 'N') {
        if (manewr == 'F') {
            int wynik = walidacja_ruchu(numer, y - 1, x, 'N', dlugosc);
            if (wynik!=0) return wynik;
            lista_statkow[numer].poczatek_statku.y -= 1;
        }
        if (manewr == 'L') {
            int wynik = walidacja_ruchu(numer, y - 1, x - (dlugosc - 1), 'W', dlugosc);
            if (wynik != 0)return wynik;
            lista_statkow[numer].poczatek_statku.y -= 1;
            lista_statkow[numer].poczatek_statku.x -= (dlugosc - 1);
            lista_statkow[numer].kierunek = 'W';
        }
        if (manewr == 'R') {
            int wynik = walidacja_ruchu(numer, y - 1, x + (dlugosc - 1), 'E', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y -= 1;
            lista_statkow[numer].poczatek_statku.x += (dlugosc - 1);
            lista_statkow[numer].kierunek = 'E';
        }
        for (int i = 0; i < dlugosc; i++)
        {
            plansza[y + i][x].typ = WODA;
        }
        return 0;
    }
    else if (lista_statkow[numer].kierunek == 'E') {
        if (manewr == 'F') {
            int wynik = walidacja_ruchu(numer, y, x + 1, 'E', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.x += 1;
        }
        if (manewr == 'L') {
            int wynik = walidacja_ruchu(numer, y - (dlugosc - 1), x + 1, 'N', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y -= (dlugosc - 1);
            lista_statkow[numer].poczatek_statku.x += 1;
            lista_statkow[numer].kierunek = 'N';
        }
        if (manewr == 'R') {
            int wynik = walidacja_ruchu(numer, y + (dlugosc - 1), x + 1, 'S', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y += (dlugosc - 1);
            lista_statkow[numer].poczatek_statku.x += 1;
            lista_statkow[numer].kierunek = 'S';
        }
        for (int i = 0; i < dlugosc; i++)
        {
            plansza[y][x - i].typ = WODA;
        }
        return 0;
    }
    else if (lista_statkow[numer].kierunek == 'W') {
        if (manewr == 'F') {
            int wynik = walidacja_ruchu(numer, y, x - 1, 'W', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.x -= 1;
        }
        if (manewr == 'L') {
            int wynik = walidacja_ruchu(numer, y + (dlugosc - 1), x - 1, 'S', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y += (dlugosc - 1);
            lista_statkow[numer].poczatek_statku.x -= 1;
            lista_statkow[numer].kierunek = 'S';
        }
        if (manewr == 'R') {
            int wynik = walidacja_ruchu(numer, y - (dlugosc - 1), x - 1, 'N', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y -= (dlugosc - 1);
            lista_statkow[numer].poczatek_statku.x -= 1;
            lista_statkow[numer].kierunek = 'N';
        }
        for (int i = 0; i < dlugosc; i++)
        {
            plansza[y][x + i].typ = WODA;
        }
        return 0;
    }
    else if (lista_statkow[numer].kierunek == 'S') {
        if (manewr == 'F') {
            int wynik = walidacja_ruchu(numer, y + 1, x, 'S', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y += 1;
        }
        if (manewr == 'L') {
            int wynik = walidacja_ruchu(numer, y + 1, x + (dlugosc - 1), 'E', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y += 1;;
            lista_statkow[numer].poczatek_statku.x += (dlugosc - 1);
            lista_statkow[numer].kierunek = 'E';
        }
        if (manewr == 'R') {
            int wynik = walidacja_ruchu(numer, y + 1, x - (dlugosc - 1), 'W', dlugosc);
            if (wynik != 0) return wynik;
            lista_statkow[numer].poczatek_statku.y += 1;
            lista_statkow[numer].poczatek_statku.x -= (dlugosc - 1);
            lista_statkow[numer].kierunek = 'W';
        }
        for (int i = 0; i < dlugosc; i++)
        {
            plansza[y - i][x].typ = WODA;
        }
        return 0;
    }
    return 2;
}
void odkrycie_planszy(int gracz, int y, int x,double dlugosc) {
    for (int i = 0; i < y_wysokosc_planszy; i++)
    {
        for (int j = 0; j < x_szerokosc_planszy; j++)
        {
            if (odleglosc(y, x, i, j) <= dlugosc) {
                plansza[i][j].odkrycie_przez_graczy[gracz] = true;
            }
        }
    }
}

int dlugosc_statku(nazwa_statku_t nazwa) {
    if (nazwa == nazwa_statku_t::CAR) return 5;
    if (nazwa == nazwa_statku_t::BAT) return 4;
    if (nazwa == nazwa_statku_t::CRU) return 3;
    if (nazwa == nazwa_statku_t::DES) return 2;
    else return 0;
}
int pozostale_czesci_statku(int gracz) {
    int czesci_statkow = 0;
    for (int i = 0; i < n_statkow; i++)
    {
        if (lista_statkow[i].wlasciciel == gracz) {
            czesci_statkow += lista_statkow[i].parametry.dlugosc_statku;
            for (int j = 0; j < lista_statkow[i].parametry.dlugosc_statku; j++)
            {
                if (lista_statkow[i].uszkodzenia[j] == true) { czesci_statkow--; }
            }
        }
    }
    return czesci_statkow;
}

void definiowanie_statku(nazwa_statku_t nazwa) {
    lista_statkow[n_statkow].parametry.rodzaj = nazwa;
    lista_statkow[n_statkow].parametry.dlugosc_statku = dlugosc_statku(nazwa);
}

void drukuj_plansze(int x,int gracz) {
    if (x == 1) {
        zaawansowane_drukowanie(gracz);
    }
    if (x == 0) {
        for (int i = 0; i < y_wysokosc_planszy; i++)
        {
            for (int j = 0; j < x_szerokosc_planszy; j++)
            {
                if ((gracz==0||gracz==1)&&plansza[i][j].odkrycie_przez_graczy[gracz] == false) {
                    cout << '?';
                    continue;
                }
                if (plansza[i][j].typ == WODA)cout << " ";
                else if (plansza[i][j].typ == RAFA)cout << "#";
                else if (plansza[i][j].typ == USZKODENIE) cout << "x";
                else { cout << "+"; }
            }
            cout << endl;
        }
    }
    if (gracz==3)cout << "PARTS REMAINING:: A : " << pozostale_czesci_statku(0) << " B : " << pozostale_czesci_statku(1) << endl;
}
void zaawansowane_drukowanie(int gracz) {
    if (x_szerokosc_planszy > 10) {
        cout << " ";
        if (y_wysokosc_planszy > 10)cout << " ";
        for (int i = 0; i < x_szerokosc_planszy; i++)
        {
            cout << (i / 10);
        }
        cout << endl;
    }
    cout << " ";
    if (y_wysokosc_planszy > 9)cout << " ";
    for (int i = 0; i < x_szerokosc_planszy; i++)
    {
        cout << i - (i / 10) * 10;
    }
    cout << endl;
    for (int i = 0; i < y_wysokosc_planszy; i++)
    {
        if (i < 10) { cout << 0; }
        cout << i;
        for (int j = 0; j < x_szerokosc_planszy; j++)
        {
            if((gracz == 0 || gracz == 1) && plansza[i][j].odkrycie_przez_graczy[gracz] == false) {
                cout << '?';
                continue;
            }
            if (plansza[i][j].typ == WODA)cout << " ";
            else if (plansza[i][j].typ == STATEK)cout << "+";
            else if (plansza[i][j].typ == RADAR)cout << "@";
            else if (plansza[i][j].typ == DZIALO)cout << "!";
            else if (plansza[i][j].typ == SILNIK)cout << "%";
            else if (plansza[i][j].typ == SILNIK_DZIALO)cout << "%";
            else if (plansza[i][j].typ == RAFA)cout << "#";
            else if (plansza[i][j].typ == USZKODENIE)cout << "x";
        }
        cout << endl;
    }
}
void obsluga_zapisu(int *gracz) {
    cout << "[state]" << endl;
    cout << "BOARD_SIZE " << y_wysokosc_planszy << " " << x_szerokosc_planszy << endl<<"NEXT_PLAYER ";
    if (*gracz == 0)cout << 'B'<<endl;
    else cout << 'A'<<endl;
    char gracze[2] = { 'A','B' };

    for (int i = 0; i < 2; i++)
    {
        cout << "INIT_POSITION " << gracze[i] << " ";
        for (int j = 0; j < 4; j++)
        {
            cout << dozwolne_pozycje[i][j] << " ";
        }
        cout << endl;
        cout << "SET_FLEET " << gracze[i]<<" ";
        for (int j = 3; j >=0; j--)
        {
            cout << poczatkowa_pula_statkow[i][j] << " ";
        }
        cout << endl;
        for (int k = 0; k < n_typow_statkow; k++)
        {
            for (int j = 0; j < n_statkow; j++)
            {
                if (lista_statkow[j].wlasciciel == i && lista_statkow[j].parametry.rodzaj == k) {
                    cout << "SHIP " << gracze[i] << " " << lista_statkow[j].poczatek_statku.y << " " << lista_statkow[j].poczatek_statku.x << " ";
                    cout << lista_statkow[j].kierunek << " " << lista_statkow[j].id << " ";
                    switch (k) {
                        case 0: {cout << "CAR "; break; }
                        case 1: {cout << "BAT "; break; }
                        case 2: {cout << "CRU "; break; }
                        case 3: {cout << "DES "; break; }
                    }
                    for (int h = 0; h < lista_statkow[j].parametry.dlugosc_statku; h++)
                    {
                        if (lista_statkow[j].uszkodzenia[h] == true)cout << 0;
                        else cout << 1;
                    }
                    cout<< endl;
                }
            }
        }
    }
    cout << "[state]" << endl;
}
int wyplyniecie_poza_plansze(int y,int x,char kierunek,int dlugosc){
    if (kierunek == 'N') {
        for (int i = 0; i < dlugosc; i++)
        {
            if ((y + i) < 0 || x<0 || (y + i) > y_wysokosc_planszy || x > x_szerokosc_planszy)return 0;
        }
    }
    else if (kierunek == 'E') {
        for (int i = 0; i < dlugosc; i++)
        {
            if (y < 0 || (x - i) < 0 || y > y_wysokosc_planszy || (x - i) > x_szerokosc_planszy)return 0;
        }
    }
    else if (kierunek == 'W') {
        for (int i = 0; i < dlugosc; i++)
        {
            if (y < 0 || (x + i) < 0 || y > y_wysokosc_planszy || (x + i) > x_szerokosc_planszy)return 0;
        }
    }
    else if (kierunek == 'S') {
        for (int i = 0; i < dlugosc; i++) {
            if ((y - i) < 0 || x<0 || (y - i) > y_wysokosc_planszy || x > x_szerokosc_planszy)return 0;
        }
    }
    return 3;
}

int bliskosc_innych_statkow(int n, int y, int x, char kierunek, int dlugosc) {
    if (kierunek == 'N') {
        for (int i = 0; i < dlugosc + 2; i++)
        {
            if ((y - 1 + i) < 0 || x - 1 < 0)continue;
            if ((y - 1 + i) >= y_wysokosc_planszy || x + 1 >= x_szerokosc_planszy)continue;
            if (plansza[y - 1 + i][x - 1].typ != WODA && plansza[y - 1 + i][x - 1].typ != RAFA) { if (plansza[y - 1 + i][x - 1].numer_statku != n) return 1; }
            if (plansza[y - 1 + i][x].typ != WODA && plansza[y - 1 + i][x].typ != RAFA) { if (plansza[y - 1 + i][x].numer_statku != n)return 1; }
            if (plansza[y - 1 + i][x + 1].typ != WODA && plansza[y - 1 + i][x + 1].typ != RAFA) { if (plansza[y - 1 + i][x + 1].numer_statku != n)return 1; }
        }
    }
    if (kierunek == 'E') {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < dlugosc + 2; i++)
            {
                if ((y - 1 + j) < 0 || x - dlugosc + i < 0)continue;
                if ((y - 1 + j) >= y_wysokosc_planszy || x - dlugosc + i >= x_szerokosc_planszy)continue;
                if (plansza[y - 1 + j][x - dlugosc + i].typ != WODA &&
                    plansza[y - 1 + j][x - dlugosc + i].typ != RAFA)
                {
                    if (plansza[y - 1 + j][x - dlugosc + i].numer_statku == n) {
                        return 0;
                    }
                    else return 1;
                }
            }
        }
    }
    if (kierunek == 'W') {
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < dlugosc + 2; i++)
            {
                if ((y - 1 + j) < 0 || x - 1 + i < 0)continue;
                if ((y - 1 + j) >= y_wysokosc_planszy || x - 1 + i >= x_szerokosc_planszy)continue;
                if (plansza[y - 1 + j][x - 1 + i].typ != WODA && plansza[y - 1 + j][x - 1 + i].typ != RAFA) {
                    if (plansza[y - 1 + j][x - 1 + i].numer_statku == n) {
                        return 0;
                    }
                    else return 1;
                }
            }
        }
    }

    if (kierunek == 'S') {
        for (int i = 0; i < dlugosc + 2; i++)
        {
            if ((y - dlugosc + i) < 0 || x - 1 < 0)continue;
            if ((y - dlugosc + i) >= y_wysokosc_planszy || x + 1 >= x_szerokosc_planszy)continue;
            if (plansza[y - dlugosc + i][x - 1].typ != WODA && plansza[y - dlugosc + i][x - 1].typ != RAFA) {
                if (plansza[y - dlugosc + i][x - 1].numer_statku != n) return 1;
            }
            if (plansza[y - dlugosc + i][x].typ != WODA && plansza[y - dlugosc + i][x].typ != RAFA) {
                if (plansza[y - dlugosc + i][x].numer_statku != n) return 1;
            }
            if (plansza[y - dlugosc + i][x + 1].typ != WODA && plansza[y - dlugosc + i][x + 1].typ != RAFA) {
                if (plansza[y - dlugosc + i][x + 1].numer_statku != n)return 1;
            }
        }
    }
    return 0;
}

int poprawnosc_lokalizacji(int gracz, int y, int x, char kierunek, int dlugosc) {
    if (kierunek == 'N') {
        for (int i = 0; i < dlugosc; i++)
        {
            if ((y + i) < dozwolne_pozycje[gracz][0] || x<dozwolne_pozycje[gracz][1] || (y + i) > dozwolne_pozycje[gracz][2]||x>dozwolne_pozycje[gracz][3])return 1;
        }
    }
    else if (kierunek == 'E') {
        for (int i = 0; i < dlugosc; i++)
        {
            if (y < dozwolne_pozycje[gracz][0] || (x-i)<dozwolne_pozycje[gracz][1] || y > dozwolne_pozycje[gracz][2] || (x-i) > dozwolne_pozycje[gracz][3])return 1;
        }
    }
    else if (kierunek == 'W') {
        for (int i = 0; i < dlugosc; i++)
        {
            if (y < dozwolne_pozycje[gracz][0] || (x+i)<dozwolne_pozycje[gracz][1] || y > dozwolne_pozycje[gracz][2] || (x+i) > dozwolne_pozycje[gracz][3])return 1;
        }
    }
    else if (kierunek == 'S') {
        for (int i = 0; i < dlugosc; i++) {
            if ((y - i) < dozwolne_pozycje[gracz][0] || x<dozwolne_pozycje[gracz][1] || (y - i) > dozwolne_pozycje[gracz][2] || x > dozwolne_pozycje[gracz][3])return 1;
        }
    }
    return 0;
}
int postawienie_na_rafie(int y, int x, char kierunek, int dlugosc) {
    if (kierunek == 'N') {
        for (int i = 0; i < dlugosc; i++)
        {
            if (plansza[y + i][x].typ == RAFA)return 1;
        }
    }
    else if (kierunek == 'E') {

        for (int i = 0; i < dlugosc; i++)
        {
            if (plansza[y][x - i].typ == RAFA)return 1;
        }
    }
    else if (kierunek == 'W') {

        for (int i = 0; i < dlugosc; i++)
        {
            if (plansza[y][x + i].typ == RAFA)return 1;
        }
    }
    else if (kierunek == 'S') {

        for (int i = 0; i < dlugosc; i++) {
            if (plansza[y - i][x].typ == RAFA)return 1;
        }
    }
    return 0;
}

int obecnosc_statku_na_planszy(int gracz, nazwa_statku_t typ, int id) {
    for (int i = 0; i < y_wysokosc_planszy; i++)
    {
        for (int j = 0; j < x_szerokosc_planszy; j++)
        {
            if (plansza[i][j].typ == STATEK) {
                int n = plansza[i][j].numer_statku;
                if (lista_statkow[n].wlasciciel == gracz && lista_statkow[n].parametry.rodzaj == typ && lista_statkow[n].id == id)return 1;
            }
        }
    }
    return 0;
}


void przelicz_plansze() {
    przelicz_rafy();
    for (int i = 0; i < n_statkow; i++)
    {
        int y = lista_statkow[i].poczatek_statku.y;
        int x = lista_statkow[i].poczatek_statku.x;
        int dlugosc = lista_statkow[i].parametry.dlugosc_statku;
        int gracz = lista_statkow[i].wlasciciel;
        plansza[y][x].typ = RADAR;
        plansza[y][x].numer_statku = i;

        if (lista_statkow[i].uszkodzenia[0] == false)odkrycie_planszy(gracz, y, x, dlugosc);
        else odkrycie_planszy(gracz, y, x, 1);

        if (lista_statkow[i].kierunek == 'N') {
            if (dlugosc == 2) {
                plansza[y + 1][x].typ = SILNIK_DZIALO;
                plansza[y + 1][x].odkrycie_przez_graczy[gracz] = true;
                plansza[y + 1][x].numer_statku = i;
            }
            else {
                plansza[y + (dlugosc - 1)][x].typ = SILNIK;
                plansza[y + 1][x].typ = DZIALO;
                plansza[y + (dlugosc - 1)][x].numer_statku = i;
                plansza[y + 1][x].numer_statku = i;
                plansza[y + (dlugosc - 1)][x].odkrycie_przez_graczy[gracz] = true;
                plansza[y + 1][x].odkrycie_przez_graczy[gracz] = true;
                for (int j = 2; j <= (dlugosc - 2); j++)
                {
                    plansza[y + j][x].typ = STATEK;
                    plansza[y + j][x].numer_statku = i;
                    plansza[y + j][x].odkrycie_przez_graczy[gracz] = true;
                }
            }
            for (int k = 0; k < dlugosc; k++)
            {
                if (lista_statkow[i].uszkodzenia[k] == true) {
                    plansza[y + k][x].typ = USZKODENIE;
                    plansza[y + k][x].odkrycie_przez_graczy[gracz] = true;
                }
            }

        }


        else if (lista_statkow[i].kierunek == 'E') {
            if (dlugosc == 2) {
                plansza[y][x - 1].typ = SILNIK_DZIALO;
                plansza[y][x - 1].numer_statku = i;
                plansza[y][x - 1].odkrycie_przez_graczy[gracz] = true;
            }
            else {
                plansza[y][x - (dlugosc - 1)].typ = SILNIK;
                plansza[y][x - 1].typ = DZIALO;
                plansza[y][x - (dlugosc - 1)].numer_statku = i;
                plansza[y][x - 1].numer_statku = i;
                plansza[y][x - (dlugosc - 1)].odkrycie_przez_graczy[gracz] = true;
                plansza[y][x - 1].odkrycie_przez_graczy[gracz] = true;

                for (int j = 2; j <= (dlugosc - 2); j++)
                {
                    plansza[y][x - j].typ = STATEK;
                    plansza[y][x - j].numer_statku = i;
                    plansza[y][x - j].odkrycie_przez_graczy[gracz] = true;
                }
            }
            for (int k = 0; k < dlugosc; k++)
            {
                if (lista_statkow[i].uszkodzenia[k] == true) {
                    plansza[y][x - k].typ = USZKODENIE;
                    plansza[y][x - k].odkrycie_przez_graczy[gracz] = true;
                }
            }

        }
        else if (lista_statkow[i].kierunek == 'W') {
            if (dlugosc == 2) {
                plansza[y][x + 1].typ = SILNIK_DZIALO;
                plansza[y][x + 1].numer_statku = i;
                plansza[y][x + 1].odkrycie_przez_graczy[gracz] = true;
            }
            else {
                plansza[y][x + (dlugosc - 1)].typ = SILNIK;
                plansza[y][x + 1].typ = DZIALO;
                plansza[y][x + (dlugosc - 1)].numer_statku = i;
                plansza[y][x + 1].numer_statku = i;
                plansza[y][x + (dlugosc - 1)].odkrycie_przez_graczy[gracz] = true;
                plansza[y][x + 1].odkrycie_przez_graczy[gracz] = true;
                for (int j = 2; j <= (dlugosc - 2); j++)
                {
                    plansza[y][x + j].typ = STATEK;
                    plansza[y][x + j].numer_statku = i;
                    plansza[y][x + j].odkrycie_przez_graczy[gracz] = true;
                }
            }
            for (int k = 0; k < dlugosc; k++)
            {
                if (lista_statkow[i].uszkodzenia[k] == true) {
                    plansza[y][x + k].typ = USZKODENIE;
                    plansza[y][x + k].odkrycie_przez_graczy[gracz] = true;
                }
            }

        }

        else if (lista_statkow[i].kierunek == 'S') {
            if (dlugosc == 2) {
                plansza[y - 1][x].typ = SILNIK_DZIALO;
                plansza[y - 1][x].numer_statku = i;
                plansza[y - 1][x].odkrycie_przez_graczy[gracz] = true;
            }
            else {
                plansza[y - (dlugosc - 1)][x].typ = SILNIK;
                plansza[y - 1][x].typ = DZIALO;
                plansza[y - (dlugosc - 1)][x].numer_statku = i;
                plansza[y - 1][x].numer_statku = i;
                plansza[y - (dlugosc - 1)][x].odkrycie_przez_graczy[gracz] = true;
                plansza[y - 1][x].odkrycie_przez_graczy[gracz] = true;
                for (int j = 2; j <= (dlugosc - 2); j++)
                {
                    plansza[y - j][x].typ = STATEK;
                    plansza[y - j][x].numer_statku = i;
                    plansza[y - j][x].odkrycie_przez_graczy[gracz] = true;
                }
            }
            for (int k = 0; k < dlugosc; k++)
            {
                if (lista_statkow[i].uszkodzenia[k] == true) {
                    plansza[y - k][x].typ = USZKODENIE;
                    plansza[y - k][x].odkrycie_przez_graczy[gracz] = true;
                }
            }

        }
    }
}
void przelicz_rafy() {
    for (int i = 0; i < n_raf; i++)
    {
        plansza[rafy[i].y][rafy[i].x].typ = RAFA;
    }
}
void szukaj_pozycji_dziala(int n, int pozycja[]) {
    for (int i = 0; i < y_wysokosc_planszy; i++)
    {
        for (int j = 0; j < x_szerokosc_planszy; j++)
        {
            if ((plansza[i][j].typ == DZIALO || plansza[i][j].typ == SILNIK_DZIALO) && plansza[i][j].numer_statku == n) {
                pozycja[0] = i;
                pozycja[1] = j;
                break;
            }
        }
    }
}
void blad_stawiania(int parametr,int gracz,int y,int x,char kierunek,int id,char *nazwa,char *stan) {
    cout << "INVALID OPERATION \"";
    if (parametr == 0)cout << "PLACE_SHIP ";
    if (parametr == 1)cout << "SHIP ";
    if (parametr==1&&gracz == 0)cout << "A ";
    if (parametr == 1 && gracz == 1) cout << "B ";
    cout << y << " " << x << " " << kierunek << " " << id << " " << nazwa;
    if (parametr == 1)cout <<" " << stan;
    cout << "\": ";
}

int szukaj_po_id(int id, int gracz, char nazwa[]) {
    nazwa_statku_t typ_statku;
    if ((strcmp(nazwa, "CAR") == 0)) {
        typ_statku = nazwa_statku_t::CAR;
    }
    else if ((strcmp(nazwa, "BAT") == 0)) {
        typ_statku = nazwa_statku_t::BAT;
    }
    else if ((strcmp(nazwa, "CRU") == 0)) {
        typ_statku = nazwa_statku_t::CRU;
    }
    else if ((strcmp(nazwa, "DES") == 0)) {
        typ_statku = nazwa_statku_t::DES;
    }
    for (int i = 0; i < n_statkow; i++)
    {
        if (gracz == lista_statkow[i].wlasciciel && typ_statku == lista_statkow[i].parametry.rodzaj && (id == lista_statkow[i].id)) return i;
    }
    return -1;
}

double odleglosc(int y1, int x1, int y2, int x2) {
    double odleglosc = sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
    return odleglosc;
}

int postawienie_wszystkich_statkow() {
    for (int i = 0; i < n_typow_statkow; i++)
    {
        if (poczatkowa_pula_statkow[0][i] - ilosc_statkow_graczy[0][i] != 0)return 1;
        if (poczatkowa_pula_statkow[1][i] - ilosc_statkow_graczy[1][i] != 0)return 1;
    }
    return 0;
}
void zerownie_liczby_ruchow_strzalow() {
    for (int i = 0; i < n_statkow; i++)
    {
        lista_statkow[i].liczba_ruchow_w_rundzie = 0;
        lista_statkow[i].liczba_strzalow_w_rundzie = 0;
    }
}