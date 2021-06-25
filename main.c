#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHORT_STRING 32
#define LONG_STRING 512

typedef struct Aukcja {
    int id; // Unikalny numer
    char nazwa_przedmiotu[SHORT_STRING];
    char kategoria[SHORT_STRING];
    int status;
    int cena;
    char opis[LONG_STRING];
    struct Uzytkownik* wlasciciel;
    struct Uzytkownik* kupujacy;
} Aukcja;

typedef struct Uzytkownik {
    int id; // Unikalny numer
    char imie[SHORT_STRING];
    char nazwisko[SHORT_STRING];
    int kwota_kupione;
    int kwota_sprzedane;
    struct ListaAukcji* lista_aukcji;
} Uzytkownik;

typedef struct ListaAukcji {
    struct ListaAukcji* nast;
    int aukcja;
} ListaAukcji;

// Deklaracje na gorze, by moc uzywac w dowolnym miejscu w kodzie
void dodaj_uzytkownika();
void usun_uzytkownika();
void edytuj_uzytkownika();
Uzytkownik *wczytaj_uzytkownika(FILE *plik);
void zapisz_uzytkownika(FILE *plik, Uzytkownik *uzytkownik);

void zarzadzaj_uzytkownikami();
void filtruj_uzytkownikow();
void filtruj_uzytkownikow_nazwisko();

void sortuj_uzytkownikow();
void sortuj_uzytkownikow_nazwisko();
void sortuj_uzytkownikow_kupno();
void sortuj_uzytkownikow_sprzedaz();

void dodaj_aukcje();
void usun_aukcje();
void edytuj_aukcje();
Aukcja *wczytaj_aukcje(FILE *plik);
void zapisz_aukcje(FILE* plik, Aukcja *aukcja);

void szukaj_aukcji();
void kup_aukcje();

void filtruj_aukcje();
void filtruj_aukcje_kategoria();
void filtruj_aukcje_status();

void sortuj_aukcje();
void sortuj_aukcje_nazwa();
void sortuj_aukcje_cena();

void clear();

int policz_aukcje();
int policz_uzytkownikow();
int policz_liste(struct ListaAukcji *start);

int generuj_id_aukcji();
int generuj_id_uzytkownika();

Aukcja *znajdz_aukcje(int id);
void usun_aukcje_akcja(Aukcja *aukcja);

Uzytkownik *znajdz_uzytkownika(int id);
void usun_uzytkownika_akcja(struct Uzytkownik *uzytkownik);

int zaczyna_sie(char nazwisko[SHORT_STRING], char igla[SHORT_STRING]);
// Troche tego jest :D

int main() {

    // Glowne menu programu
    while (1) {
        clear();
        printf("Program aukcyjny\n");
        printf("\n");
        printf("1) Wystaw przedmiot\n");
        printf("2) Kup przedmiot\n");
        printf("3) Filtruj aukcje\n");
        printf("4) Sortuj aukcje\n");
        printf("5) Szukaj aukcji\n");
        printf("\n");
        printf("6) Edytuj aukcje\n");
        printf("7) Usun aukcje\n");
        printf("\n");
        printf("8) Zarzadzaj uzytkownikami\n");
        printf("\n");
        printf("0) Zakoncz program\n");

        int input;
        scanf("%d", &input);
        clear();

        switch (input) {
            case 1: dodaj_aukcje(); break;
            case 2: kup_aukcje(); break;
            case 3: filtruj_aukcje(); break;
            case 4: sortuj_aukcje(); break;
            case 5: szukaj_aukcji(); break;
            case 6: edytuj_aukcje(); break;
            case 7: usun_aukcje(); break;
            case 8: zarzadzaj_uzytkownikami(); break;
            case 0: return 0;
            default: break;
        }
    }
}

// ===
// Dodawanie/usuwanie/edytowanie aukcji
// ===
void dodaj_aukcje() {
    printf("Identyfikator wystawiajacego aukcje:\n");
    int id;
    scanf("%d", &id);
    Uzytkownik* uzytkownik = znajdz_uzytkownika(id);

    if (uzytkownik == NULL) {
        printf("Nie znaleziono uzytkownika o takim identyfikatorze\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    printf("Wystawiasz jako %s %s\n", uzytkownik->imie, uzytkownik->nazwisko);

    Aukcja* aukcja = malloc(sizeof(Aukcja));
    aukcja->id = generuj_id_aukcji();
    aukcja->wlasciciel = uzytkownik;
    aukcja->kupujacy = NULL;
    aukcja->status = 0;

    printf("Nazwa przedmiotu:\n");
    scanf("%s", aukcja->nazwa_przedmiotu);

    // Ignoruje enter
    getchar();

    printf("Opis:\n");
    // Wczytuje cala linie
    scanf("%[^\n]", aukcja->opis);

    printf("Kategoria:\n");
    scanf("%s", aukcja->kategoria);

    // Wczytuje jako float i zapisuje jako int
    printf("Cena:\n");
    float cena;
    scanf("%f", &cena);
    aukcja->cena = (int) (cena * 100);

    FILE* plik = fopen("aukcje.txt", "a");
    zapisz_aukcje(plik, aukcja);
    fclose(plik);

    printf("Dodano %s!\n", aukcja->nazwa_przedmiotu);

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void usun_aukcje() {
    int id;
    printf("Identyfikator aukcji:\n");
    scanf("%d", &id);
    Aukcja* aukcja = znajdz_aukcje(id);

    if (aukcja == NULL) {
        printf("Nie znaleziono aukcji o takim identyfikatorze\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    if (aukcja->kupujacy == NULL || znajdz_uzytkownika(aukcja->kupujacy->id) == NULL) {
        usun_aukcje_akcja(aukcja);
        printf("Pomyslnie usunieto aukcje %s\n", aukcja->nazwa_przedmiotu);
    } else {
        printf("Nie mozna usunac aukcji, bo kupujacy jest w bazie\n");
    }

    free(aukcja);

    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void edytuj_aukcje() {
    int id;
    printf("Identyfikator aukcji:\n");
    scanf("%d", &id);
    Aukcja* aukcja = znajdz_aukcje(id);

    if (aukcja == NULL) {
        printf("Nie znaleziono aukcji o takim identyfikatorze\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    // Pozwalamy na zmiane tylko gdy nie byla kupiona
    if (aukcja->status) {
        printf("Nie mozna edytowac zakonczonej aukcji\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;

    }

    usun_aukcje_akcja(aukcja);

    printf("Nazwa przedmiotu (%s):\n", aukcja->nazwa_przedmiotu);
    scanf("%s", aukcja->nazwa_przedmiotu);

    // Ignoruje enter
    getchar();

    printf("Opis (%s):\n", aukcja->opis);
    // Wczytuje cala linie
    scanf("%[^\n]", aukcja->opis);

    printf("Kategoria (%s):\n", aukcja->kategoria);
    scanf("%s", aukcja->kategoria);

    // Wczytuje jako float i zapisuje jako int
    printf("Cena (%.2f):\n", (float) aukcja->cena / 100);
    float cena;
    scanf("%f", &cena);
    aukcja->cena = (int) (cena * 100);

    FILE* plik = fopen("aukcje.txt", "a");
    zapisz_aukcje(plik, aukcja);
    fclose(plik);
    free(aukcja);
}

// ===
// Kupowanie aukcji
// ===
void kup_aukcje() {
    printf("Identyfikator kupujacego aukcje:\n");
    int id;
    scanf("%d", &id);
    Uzytkownik* uzytkownik = znajdz_uzytkownika(id);

    if (uzytkownik == NULL) {
        printf("Nie znaleziono uzytkownika o takim identyfikatorze\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    printf("Kupujesz jako %s %s\n", uzytkownik->imie, uzytkownik->nazwisko);

    printf("Identyfikator aukcji:\n");
    scanf("%d", &id);
    Aukcja* aukcja = znajdz_aukcje(id);

    if (aukcja == NULL) {
        printf("Nie znaleziono aukcji o takim identyfikatorze\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    if (aukcja->status) {
        printf("Aukcja zakonczona. %s zostalo sprzedane %s %s\n",
               aukcja->nazwa_przedmiotu,
               aukcja->kupujacy->imie,
               aukcja->kupujacy->nazwisko);

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    if (aukcja->wlasciciel->id == uzytkownik->id) {
        printf("Nie mozna kupic wlasnej aukcji;\n");

        // Ignoruje input uzytkownika
        printf("0) Cofnij\n");
        int _; scanf("%d", &_);
        return;
    }

    printf("Kupiles %s za %.2f od %s %s\n",
           aukcja->nazwa_przedmiotu,
           (float) aukcja->cena / 100,
           aukcja->wlasciciel->imie,
           aukcja->wlasciciel->nazwisko);

    aukcja->status = 1;
    aukcja->kupujacy = uzytkownik;

    aukcja->kupujacy->kwota_kupione += aukcja->cena;
    aukcja->wlasciciel->kwota_sprzedane += aukcja->cena;

    // Aktualizuje uzytkownikow
    usun_uzytkownika_akcja(aukcja->kupujacy);
    usun_uzytkownika_akcja(aukcja->wlasciciel);
    FILE* plik = fopen("uzytkownicy.txt", "a");
    zapisz_uzytkownika(plik, aukcja->kupujacy);
    zapisz_uzytkownika(plik, aukcja->wlasciciel);
    fclose(plik);

    // Aktualizuje akcje
    usun_aukcje_akcja(aukcja);
    plik = fopen("aukcje.txt", "a");
    zapisz_aukcje(plik, aukcja);
    fclose(plik);

    free(aukcja->kupujacy);
    free(aukcja->wlasciciel);
    free(aukcja);

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

// ===
// Wczytywnie aukcji z pliku
// ===
Aukcja *wczytaj_aukcje(FILE *plik) {
    Aukcja* aukcja = malloc(sizeof(Aukcja));

    int id_kupujacego;
    int id_wlasciciela;
    fscanf(plik,
           "%d %s %s %d %d %d %d %[^\n]\n",
           &aukcja->id,
           aukcja->nazwa_przedmiotu,
           aukcja->kategoria,
           &aukcja->status,
           &aukcja->cena,
           &id_wlasciciela,
           &id_kupujacego,
           aukcja->opis);

    aukcja->kupujacy = NULL;
    aukcja->wlasciciel = NULL;

    if (id_kupujacego >= 0) {
        aukcja->kupujacy = znajdz_uzytkownika(id_kupujacego);
    }

    if (id_wlasciciela >= 0) {
        aukcja->wlasciciel = znajdz_uzytkownika(id_wlasciciela);
    }

    return aukcja;
}

// ===
// Zapis aukcji do pliku
// ===
void zapisz_aukcje(FILE* plik, Aukcja *aukcja) {
    int id_kupujacego = -1;
    if (aukcja->kupujacy != NULL) {
        id_kupujacego = aukcja->kupujacy->id;
    }

    fprintf(plik,
            "%d %s %s %d %d %d %d %s\n",
            aukcja->id,
            aukcja->nazwa_przedmiotu,
            aukcja->kategoria,
            aukcja->status,
            aukcja->cena,
            aukcja->wlasciciel->id,
            id_kupujacego,
            aukcja->opis);
}

Uzytkownik *znajdz_uzytkownika(int id) {
    int n = policz_uzytkownikow();
    FILE* plik = fopen("uzytkownicy.txt", "r");

    for (int i = 0; i < n; i++) {
        Uzytkownik * uzytkownik = wczytaj_uzytkownika(plik);
        if (uzytkownik->id == id) {
            fclose(plik);
            return uzytkownik;
        }

        free(uzytkownik);
    }

    fclose(plik);
    return NULL;
}


void filtruj_aukcje() {
    while (1) {
        clear();
        printf("Filtruj aukcje\n");
        printf("\n");
        printf("1) Filtruj wg kategorii\n");
        printf("2) Filtruj wg statusu\n");
        printf("\n");
        printf("0) Cofnij\n");

        int input;
        scanf("%d", &input);

        clear();

        switch (input) {
            case 1: filtruj_aukcje_kategoria(); break;
            case 2: filtruj_aukcje_status(); break;
            case 0:
                return;

            default: break;
        }
    }
}

// ===
// Filtrowanie/szukanie aukcji
// ===

void szukaj_aukcji() {
    printf("Szukaj:\n");
    char igla[SHORT_STRING];
    scanf("%s", igla);

    int n = policz_aukcje();
    FILE* plik = fopen("aukcje.txt", "r");
    printf("Nazwa przedmiotu | Cena | Status | Kategoria | Opis\n");
    for (int i = 0; i < n; ++i) {
        Aukcja* aukcja = wczytaj_aukcje(plik);
        if (!zaczyna_sie(aukcja->nazwa_przedmiotu, igla)) {
            free(aukcja);
            continue;
        }
        char* status = "Nie kupione";
        if (aukcja->status) {
            status = "Kupione";
        }

        printf("%s | %.2f | %s | %s | %s\n",
               aukcja->nazwa_przedmiotu,
               (float) aukcja->cena / 100,
               status,
               aukcja->kategoria,
               aukcja->opis);

        free(aukcja);
    }
    fclose(plik);

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void filtruj_aukcje_status() {
    printf("Podaj filtr statusu (0 - Nie kupione, 1 - kupione):\n");
    int status_filtr;
    scanf("%d", &status_filtr);

    if (status_filtr != 0) {
        status_filtr = 1;
    }

    int n = policz_aukcje();
    FILE* plik = fopen("aukcje.txt", "r");
    printf("Nazwa przedmiotu | Cena | Status | Kategoria | Opis\n");
    for (int i = 0; i < n; ++i) {
        Aukcja* aukcja = wczytaj_aukcje(plik);
        if (status_filtr != aukcja->status) {
            free(aukcja);
            continue;
        }
        char* status = "Nie kupione";
        if (aukcja->status) {
            status = "Kupione";
        }

        printf("%s | %.2f | %s | %s | %s\n",
               aukcja->nazwa_przedmiotu,
               (float) aukcja->cena / 100,
               status,
               aukcja->kategoria,
               aukcja->opis);

        free(aukcja);
    }
    fclose(plik);

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);

}

void filtruj_aukcje_kategoria() {
    printf("Podaj filtr kategorii:\n");
    char igla[SHORT_STRING];
    scanf("%s", igla);

    int n = policz_aukcje();
    FILE* plik = fopen("aukcje.txt", "r");
    printf("Nazwa przedmiotu | Cena | Status | Kategoria | Opis\n");
    for (int i = 0; i < n; ++i) {
        Aukcja* aukcja = wczytaj_aukcje(plik);
        if (!zaczyna_sie(aukcja->kategoria, igla)) {
            free(aukcja);
            continue;
        }
        char* status = "Nie kupione";
        if (aukcja->status) {
            status = "Kupione";
        }

        printf("%s | %.2f | %s | %s | %s\n",
                aukcja->nazwa_przedmiotu,
               (float) aukcja->cena / 100,
               status,
               aukcja->kategoria,
               aukcja->opis);

        free(aukcja);
    }
    fclose(plik);

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

// ===
// Sortowanie aukcji
// ===

void sortuj_aukcje() {
    while (1) {
        clear();
        printf("Sortuj aukcje\n");
        printf("\n");
        printf("1) Sortuj po nazwie\n");
        printf("2) Sortuj po cenie\n");
        printf("\n");
        printf("0) Cofnij\n");

        int input;
        scanf("%d", &input);

        clear();

        switch (input) {
            case 1: sortuj_aukcje_nazwa(); break;
            case 2: sortuj_aukcje_cena(); break;
            case 0:
                return;

            default: break;
        }
    }
}

void sortuj_aukcje_cena() {
    int n = policz_aukcje();
    Aukcja* aukcje[n];
    FILE* plik = fopen("aukcje.txt", "r");

    for (int i = 0; i < n; i++) {
        aukcje[i] = wczytaj_aukcje(plik);
    }

    fclose(plik);

    // Sortowanie
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n - i; j++) {
            if (aukcje[j]->cena > aukcje[j + 1]->cena) {
                Aukcja* temp = aukcje[j];
                aukcje[j] = aukcje[j + 1];
                aukcje[j + 1] = temp;
            }
        }
    }

    printf("Nazwa przedmiotu | Cena | Status | Kategoria | Opis\n");
    for (int i = 0; i < n; i++) {
        Aukcja* aukcja = aukcje[i];
        char* status = "Nie kupione";
        if (aukcja->status) {
            status = "Kupione";
        }

        printf("%s | %.2f | %s | %s | %s\n",
               aukcja->nazwa_przedmiotu,
               (float) aukcja->cena / 100,
               status,
               aukcja->kategoria,
               aukcja->opis);
        free(aukcja);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void sortuj_aukcje_nazwa() {
    int n = policz_aukcje();
    Aukcja* aukcje[n];
    FILE* plik = fopen("aukcje.txt", "r");

    for (int i = 0; i < n; i++) {
        aukcje[i] = wczytaj_aukcje(plik);
    }

    fclose(plik);

    // Sortowanie
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n - i; j++) {
            if (strcmp(aukcje[j]->nazwa_przedmiotu, aukcje[j + 1]->nazwa_przedmiotu) > 0) {
                Aukcja* temp = aukcje[j];
                aukcje[j] = aukcje[j + 1];
                aukcje[j + 1] = temp;
            }
        }
    }

    printf("Nazwa przedmiotu | Cena | Status | Kategoria | Opis\n");
    for (int i = 0; i < n; i++) {
        Aukcja* aukcja = aukcje[i];
        char* status = "Nie kupione";
        if (aukcja->status) {
            status = "Kupione";
        }

        printf("%s | %.2f | %s | %s | %s\n",
               aukcja->nazwa_przedmiotu,
               (float) aukcja->cena / 100,
               status,
               aukcja->kategoria,
               aukcja->opis);
        free(aukcja);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

// ===
// Zarzadzanie uzytkownikami
// ===
void zarzadzaj_uzytkownikami() {
    while (1) {
        clear();
        printf("Zrzadzaj uzytkownikami\n");
        printf("\n");
        printf("1) Dodaj uzytkownika\n");
        printf("2) Edytuj uzytkownika\n");
        printf("3) Usun uzytkownika\n");
        printf("\n");
        printf("4) Filtruj uzytkownikow\n");
        printf("5) Sortuj uzytkownikow\n");
        printf("\n");
        printf("0) Cofnij\n");

        int input;
        scanf("%d", &input);

        clear();

        switch (input) {
            case 1: dodaj_uzytkownika(); break;
            case 2: edytuj_uzytkownika(); break;
            case 3: usun_uzytkownika(); break;
            case 4: filtruj_uzytkownikow(); break;
            case 5: sortuj_uzytkownikow(); break;
            case 0:
                return;

            default: break;
        }
    }
}

// ===
// Ddawanie/usuwanie/edycja
// ===

void dodaj_uzytkownika() {
    int nowe_id = generuj_id_uzytkownika();
    FILE* plik = fopen("uzytkownicy.txt", "a");
    if (plik == NULL) {
        printf("Nie mozna otworzyc pliku uzytkownicy.txt");
        return;
    }

    Uzytkownik* uzytkownik = malloc(sizeof(Uzytkownik));

    printf("Imie uzytkownika: ");
    scanf("%s", uzytkownik->imie);

    printf("Nazwisko uzytkownika: ");
    scanf("%s", uzytkownik->nazwisko);

    uzytkownik->id = nowe_id;
    uzytkownik->kwota_sprzedane = 0;
    uzytkownik->kwota_kupione = 0;
    uzytkownik->lista_aukcji = NULL;

    zapisz_uzytkownika(plik, uzytkownik);

    fclose(plik);
    free(uzytkownik);
}

void usun_uzytkownika() {
    int id;
    printf("Podaj identyfikator (Id) uzytkownika:\n");
    scanf("%d", &id);

    int n = policz_uzytkownikow();
    Uzytkownik* uzytkownicy[n];
    FILE* plik = fopen("uzytkownicy.txt", "r");

    // Wczytuje wszystkich uzytkownikow do tabeli
    for (int i = 0; i < n; i++) {
        uzytkownicy[i] = wczytaj_uzytkownika(plik);
    }

    fclose(plik);

    plik = fopen("uzytkownicy.txt", "w");

    // Nadpisuje plik i wpisuje wszystkich uzytkownikow bez tego o danym id
    int znaleziono = 0;
    for (int i = 0; i < n; i++) {
        if (uzytkownicy[i]->id == id) {
            printf("Pomyslnie usunieto uzytkownika %s %s\n", uzytkownicy[i]->imie, uzytkownicy[i]->nazwisko);
            znaleziono = 1;
            continue;
        }

        zapisz_uzytkownika(plik, uzytkownicy[i]);
        free(uzytkownicy[i]);
    }

    fclose(plik);

    if (!znaleziono) {
        printf("Nie znaleziono uzytkownika o danym identyfikatorze\n");
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void edytuj_uzytkownika() {
    int id;
    printf("Podaj identyfikator (Id) uzytkownika:\n");
    scanf("%d", &id);

    Uzytkownik* uzytkownik = znajdz_uzytkownika(id);
    if (uzytkownik == NULL) {
        printf("Nie znaleziono uzytkownika o danym identyfikatorze\n");
    } else {
        usun_uzytkownika_akcja(uzytkownik);
        printf("Podaj imie (dawniej %s):\n", uzytkownik->imie);
        scanf("%s", uzytkownik->imie);

        // Ustawiam nazwisko
        printf("Podaj nazwisko (dawniej %s):\n", uzytkownik->nazwisko);
        scanf("%s", uzytkownik->nazwisko);

        printf("Pomyslnie usunieto edytowano %s %s\n", uzytkownik->imie, uzytkownik->nazwisko);
        FILE* plik = fopen("uzytkownicy.txt", "a");
        zapisz_uzytkownika(plik, uzytkownik);
        fclose(plik);

        free(uzytkownik);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

// ===
// Wczytywanie uzytkownika
// ===
Uzytkownik *wczytaj_uzytkownika(FILE *plik) {
    Uzytkownik* uzytkownik = malloc(sizeof(Uzytkownik));
    int ileAukcji;
    fscanf(plik,
           "%d %s %s %d %d %d",
           &uzytkownik->id,
           uzytkownik->imie,
           uzytkownik->nazwisko,
           &uzytkownik->kwota_kupione,
           &uzytkownik->kwota_sprzedane,
           &ileAukcji);

    uzytkownik->lista_aukcji = NULL;

    ListaAukcji* akt = uzytkownik->lista_aukcji;
    for (int i = 0; i < ileAukcji; i++) {
        ListaAukcji* element = malloc(sizeof(ListaAukcji));
        fscanf(plik, "%d", &element->aukcja);
        element->nast = NULL;

        // Ustawiam nastepny aktualnego na element i aktualizuje aktualny
        akt->nast = element;
        akt = element;
    }

    return uzytkownik;
}

// ===
// Zapisywanie uzytkownika
// ===
void zapisz_uzytkownika(FILE* plik, Uzytkownik *uzytkownik) {
    fprintf(plik,
            "%d %s %s %d %d ",
            uzytkownik->id,
            uzytkownik->imie,
            uzytkownik->nazwisko,
            uzytkownik->kwota_kupione,
            uzytkownik->kwota_sprzedane);

    // Zapisuje ilosc aukcji
    fprintf(plik, "%d ", policz_liste(uzytkownik->lista_aukcji));

    // Zapisuje id tych aukcji
    ListaAukcji* akt = uzytkownik->lista_aukcji;
    while (akt != NULL) {
        fprintf(plik,"%d ", akt->aukcja);
        akt = akt->nast;
    }

    fprintf(plik, "\n");
}

// ===
// Filtrowanie uzytkownikow
// ===
void filtruj_uzytkownikow() {
    while (1) {
        clear();
        printf("Filtruj uzytkownikow\n");
        printf("\n");
        printf("1) Filtruj po nazwisku\n");
        printf("\n");
        printf("0) Cofnij\n");

        int input;
        scanf("%d", &input);

        clear();

        switch (input) {
            case 1: filtruj_uzytkownikow_nazwisko();
            case 0:
                return;

            default: break;
        }
    }
}

void filtruj_uzytkownikow_nazwisko() {
    int n = policz_uzytkownikow();
    Uzytkownik* uzytkownicy[n];

    printf("Podaj filtr nazwiska:\n");
    char igla[SHORT_STRING];
    scanf("%s", igla);

    FILE* plik = fopen("uzytkownicy.txt", "r");

    for (int i = 0; i < n; i++) {
        uzytkownicy[i] = wczytaj_uzytkownika(plik);
    }

    fclose(plik);

    printf("Imie | Nazwisko | Id | Wydal | Zarobil\n");
    for (int i = 0; i < n; i++) {
        if (!zaczyna_sie(uzytkownicy[i]->nazwisko, igla)) {
            continue;
        }

        printf("%s | %s | %d | %.2fzl | %.2fzl\n",
               uzytkownicy[i]->imie,
               uzytkownicy[i]->nazwisko,
               uzytkownicy[i]->id,
               (float) uzytkownicy[i]->kwota_kupione / 100,
               (float) uzytkownicy[i]->kwota_sprzedane / 100);
        free(uzytkownicy[i]);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

// ===
// Sortowanie uzytkownikow
// ===
void sortuj_uzytkownikow() {
    while (1) {
        clear();
        printf("Sortuj uzytkownikow\n");
        printf("\n");
        printf("1) Sortuj po nazwisku\n");
        printf("2) Sortuj po kwocie kupna\n");
        printf("3) Sortuj po kwocie sprzedazy\n");
        printf("\n");
        printf("0) Cofnij\n");

        int input;
        scanf("%d", &input);

        clear();

        switch (input) {
            case 1: sortuj_uzytkownikow_nazwisko(); break;
            case 2: sortuj_uzytkownikow_kupno(); break;
            case 3: sortuj_uzytkownikow_sprzedaz(); break;
            case 0:
                return;

            default: break;
        }
    }
}

void sortuj_uzytkownikow_nazwisko() {
    int n = policz_uzytkownikow();
    Uzytkownik* uzytkownicy[n];
    FILE* plik = fopen("uzytkownicy.txt", "r");

    for (int i = 0; i < n; i++) {
        uzytkownicy[i] = wczytaj_uzytkownika(plik);
    }

    fclose(plik);

    // Sortowanie
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n - i; j++) {
            if (strcmp(uzytkownicy[j]->nazwisko, uzytkownicy[j + 1]->nazwisko) > 0) {
                Uzytkownik* temp = uzytkownicy[j];
                uzytkownicy[j] = uzytkownicy[j + 1];
                uzytkownicy[j + 1] = temp;
            }
        }
    }

    printf("Imie | Nazwisko | Id | Wydal | Zarobil\n");
    for (int i = 0; i < n; i++) {
        printf("%s | %s | %d | %.2fzl | %.2fzl\n",
               uzytkownicy[i]->imie,
               uzytkownicy[i]->nazwisko,
               uzytkownicy[i]->id,
               (float) uzytkownicy[i]->kwota_kupione / 100,
               (float) uzytkownicy[i]->kwota_sprzedane / 100);
        free(uzytkownicy[i]);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void sortuj_uzytkownikow_kupno() {
    int n = policz_uzytkownikow();
    Uzytkownik* uzytkownicy[n];
    FILE* plik = fopen("uzytkownicy.txt", "r");

    for (int i = 0; i < n; i++) {
        uzytkownicy[i] = wczytaj_uzytkownika(plik);
    }

    fclose(plik);

    // Sortowanie
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n - i; j++) {
            if (uzytkownicy[j]->kwota_kupione > uzytkownicy[j + 1]->kwota_kupione) {
                Uzytkownik* temp = uzytkownicy[j];
                uzytkownicy[j] = uzytkownicy[j + 1];
                uzytkownicy[j + 1] = temp;
            }
        }
    }

    printf("Imie | Nazwisko | Id | Wydal | Zarobil\n");
    for (int i = 0; i < n; i++) {
        printf("%s | %s | %d | %.2fzl | %.2fzl\n",
               uzytkownicy[i]->imie,
               uzytkownicy[i]->nazwisko,
               uzytkownicy[i]->id,
               (float) uzytkownicy[i]->kwota_kupione / 100,
               (float) uzytkownicy[i]->kwota_sprzedane / 100);
        free(uzytkownicy[i]);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

void sortuj_uzytkownikow_sprzedaz() {
    int n = policz_uzytkownikow();
    Uzytkownik* uzytkownicy[n];
    FILE* plik = fopen("uzytkownicy.txt", "r");

    for (int i = 0; i < n; i++) {
        uzytkownicy[i] = wczytaj_uzytkownika(plik);
    }

    fclose(plik);

    // Sortowanie
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < n - i; j++) {
            if (uzytkownicy[j]->kwota_sprzedane > uzytkownicy[j + 1]->kwota_sprzedane) {
                Uzytkownik* temp = uzytkownicy[j];
                uzytkownicy[j] = uzytkownicy[j + 1];
                uzytkownicy[j + 1] = temp;
            }
        }
    }

    printf("Imie | Nazwisko | Id | Wydal | Zarobil\n");
    for (int i = 0; i < n; i++) {
        printf("%s | %s | %d | %.2fzl | %.2fzl\n",
               uzytkownicy[i]->imie,
               uzytkownicy[i]->nazwisko,
               uzytkownicy[i]->id,
               (float) uzytkownicy[i]->kwota_kupione / 100,
               (float) uzytkownicy[i]->kwota_sprzedane / 100);
        free(uzytkownicy[i]);
    }

    // Ignoruje input uzytkownika
    printf("0) Cofnij\n");
    int _; scanf("%d", &_);
}

// ===
// Czyszczenie ekranu
// ===
void clear() {
#if _WIN32 || _WIN64
    system("cls");
#else
    system("clear");
#endif
}

// ===
// Usuwanie uzytkownika (pomocnicza)
// ===
void usun_uzytkownika_akcja(struct Uzytkownik *uzytkownik) {
    int n = policz_uzytkownikow();
    Uzytkownik * uzytkownicy[n];

    FILE* plik = fopen("uzytkownicy.txt", "r");

    for (int i = 0; i < n; i++) {
        uzytkownicy[i] = wczytaj_uzytkownika(plik);
    }

    fclose(plik);

    plik = fopen("uzytkownicy.txt", "w");
    for (int i = 0; i < n; i++) {
        if (uzytkownicy[i]->id != uzytkownik->id) {
            zapisz_uzytkownika(plik, uzytkownicy[i]);
        }

        free(uzytkownicy[i]);
    }
    fclose(plik);
}

// ===
// Usuwanie aukcji (pomocnicza)
// ===
void usun_aukcje_akcja(Aukcja *aukcja) {
    int n = policz_aukcje();
    Aukcja* aukcje[n];

    FILE* plik = fopen("aukcje.txt", "r");

    for (int i = 0; i < n; i++) {
        aukcje[i] = wczytaj_aukcje(plik);
    }

    fclose(plik);

    plik = fopen("aukcje.txt", "w");
    for (int i = 0; i < n; i++) {
        if (aukcje[i]->id != aukcja->id) {
            zapisz_aukcje(plik, aukcje[i]);
        }

        free(aukcje[i]);
    }

    fclose(plik);
}

// ===
// Znajdowanie aukcji w pliku po id (pomocnicza)
// ===
Aukcja *znajdz_aukcje(int id) {
    int n = policz_aukcje();
    FILE* plik = fopen("aukcje.txt", "r");

    for (int i = 0; i < n; i++) {
        Aukcja * aukcja = wczytaj_aukcje(plik);
        if (aukcja->id == id) {
            fclose(plik);
            return aukcja;
        }

        free(aukcja);
    }

    fclose(plik);
    return NULL;
}

// ===
// Liczenie ilosci aukcji/uzytkownikow w pliku (pomocnicza)
// ===
int policz_linie(char* nazwa) {
    int ilosc = 0;
    FILE* plik = fopen(nazwa, "r");

    while (!feof(plik)) {
        if(fgetc(plik) == '\n') {
            ilosc++;
        }
    }

    fclose(plik);
    return ilosc;
}

int policz_aukcje() {
    return policz_linie("aukcje.txt");
}

int policz_uzytkownikow() {
    return policz_linie("uzytkownicy.txt");
}

// ===
// Generowanie identyfikatora (pomocnicza)
// ===
int generuj_id_uzytkownika() {
    // Generuje nowe id dla uzytkownika
    // Zawsze wieksze o 1 od najwiekszego id
    // Lub 0 gdy nie ma zadnych uzytkownikow
    // tak samo w generuj_id_aukcji()
    int id = 0;
    FILE* plik = fopen("uzytkownicy.txt", "r");

    while (!feof(plik)) {
        int kandydat;
        // Wczytuje id uzytkownika i pomijam reszte argumentow az do nowej linii
        fscanf(plik, "%d %*[^\n]", &kandydat);

        // Jezeli kandydat na nowe id > aktualnego id -> zamieniam aktualne id na kandydata
        if (kandydat + 1 > id) {
            id = kandydat + 1;
        }
    }

    fclose(plik);
    return id;
}

int generuj_id_aukcji() {
    int id = 0;
    FILE* plik = fopen("aukcje.txt", "r");

    while (!feof(plik)) {
        int kandydat;
        // Wczytuje id uzytkownika i pomijam reszte argumentow az do nowej linii
        fscanf(plik, "%d %*[^\n]", &kandydat);

        // Jezeli kandydat na nowe id > aktualnego id -> zamieniam aktualne id na kandydata
        if (kandydat + 1 > id) {
            id = kandydat + 1;
        }
    }

    fclose(plik);
    return id;
}

// ===
// Sprawdzanie czy string siano zaczyna sie od stringu igla
// ===
int zaczyna_sie(char siano[SHORT_STRING], char igla[SHORT_STRING]) {
    return strncmp(igla, siano, strlen(igla)) == 0;
}

// ===
// Liczenie ile aukcji jest w liscie (pomocnicza)
// ===
int policz_liste(struct ListaAukcji *start) {
    int ilosc = 0;
    while (start != NULL) {
        ilosc += 1;
        start = start->nast;
    }

    return ilosc;
}
