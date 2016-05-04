#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern "C" {
#ifdef BIT64
#include"./sdl64/include/SDL.h"
#include"./sdl64/include/SDL_main.h"
#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
#endif
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define fullscreen false
#define szer_pal (50*((pal_wydluzenie>1)?2:1))
#define szybkosc_paletki 500.0
#define wys_pal 10.0
#define max_pilki 100.0
#define promien_pilki 5.0
#define promien_nagrody 5.0
#define max_zmiana_kata 45.0
#define min_kat 15.0
#define LEWA_KRAWEDZ 4.0
#define max_podkrecenie 0.1
#define GORNA_KRAWEDZ 4.0
#define DOLNA_KRAWEDZ ((double)(SCREEN_HEIGHT-GORNA_KRAWEDZ-4)) //czyli wysokość planszy od górnej krawędzi do 4 pikseli od dołu
#define PRAWA_KRAWEDZ ((double)(((3*SCREEN_WIDTH)/5)-LEWA_KRAWEDZ)) //szerokość 3/5 planszy
#define WYS_PAL_NAD_DOLNA_KRAWEDZ 10.0
#define PREDKOSC_POCZATKOWA 100.0
#define CZAS_PRZYSPIESZANIA 30.0
#define PREDKOSC_KONCOWA 300.0 //pilka przyspiesza o 300, nie do 300(czyli w tym przypadku po 30s będzie miała prędkość 400)
#define wysokosc_klocka 15.0
#define szerokosc_klocka ((int)(PRAWA_KRAWEDZ/klocki_w_poziomie))
#define min_wysokosc_nad_paletka 200.0
#define klocki_w_poziomie 15.0
#define klocki_w_pionie 15.0
#define kolor_tla_planszy siwy
#define poczatkowe_zycia 3.0
#define punkty_za_zbicie 100.0
#define punkty_za_skonczenie 200.0
#define punkty_za_nagrode 150.0
#define prawd_nagrody 0.5
#define nag_przyspieszenie 5.0
#define max_nagrody 50.0
#define poc_predkosc 500.0
#define max_strzaly 10.0
#define promien_wiru 7.0
#define max_wiry 5.0
#define predkosc_wiru 100.0
#define max_czas_w_jednym_kierunku 0.5
#define czas_kumulacji 4.0
#define min_delta_polozenia 100
#define min_czas_do_nowego_obiektu 10.0

#define siwy SDL_MapRGB(screen->format, 0x40, 0x40, 0x40)
#define zolty SDL_MapRGB(screen->format, 192, 192, 0)
#define bialy SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF)
#define czarny SDL_MapRGB(screen->format, 0x00, 0x00, 0x00)
#define zielony SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00)
#define czerwony SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00)
#define niebieski SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC)
#define zielony_gleboki SDL_MapRGB(screen->format, 0x3E, 0x70, 0x26)
#define tlo_punkty SDL_MapRGB(screen->format,0x68,0x3C,0xA3)
#define tlo_zycia SDL_MapRGB(screen->format,0xC9,0x6C,0x22)
#define tlo_bonusow SDL_MapRGB(screen->format,0x22,0x7F,0xC9)

struct pilka
{
	double x, y, kierunek, podkrecenie;
	bool przyklejona,puszczona;
};
struct plansza
{
	Uint32 kolor;
	int wytrzymałość;
	bool aktywny;
};
struct najlepsze_wyniki
{
	int punkty;
	char nick[100];
};
struct ruchome_obiekty
{
	double x, y, kierunek, ost_zmiana, max_x, max_y, min_x, min_y, ost_kumulacja;
	Uint32 kolor;
};

enum rodzaj_nagrody
{
	szeroka_paletka,
	wiele_pilek,
	zmiana_tempa,
	dodatkowe_zycie,
	kleista_paletka,
	strzelajaca_paletka
};

struct nagrody
{
	double x, y, predkosc,koniec;
	rodzaj_nagrody nagroda;
};

struct pocisk
{
	double x, y;
};

class arkanoid
{
public:
	arkanoid();

	void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);
	void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);
	void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
	void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);
	void draw_circle(SDL_Surface *surface, int n_cx, int n_cy, int radius, Uint32 pixel);
	void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
	void ruch_pilki(pilka &kulka, double delta, double predkosc,int n);
	void wysw_pilke(SDL_Surface *screen, pilka kulka, Uint32 pixel);
	void glowna(), przykladowa_plansza(), trafiony(int i, int j,pilka kulka);
	void wysw_klocki(SDL_Surface *screen), info();
	bool zderzenie_z_klockiem(pilka &kulka, double x, double y, double szer, double wys, int i, int j);
	bool zderzenie_z_klockiem(ruchome_obiekty &wir, double x, double y, double szer, double wys, int i, int j);
	bool odbicie_od_paletki(pilka &kulka,int n);
	void zderzenie_z_prawej(pilka &kulka), zderzenie_z_gory(pilka &kulka);
	void zderzenie_z_lewej(pilka &kulka), zderzenie_z_dolu(pilka &kulka);
	void wysw_paletke(SDL_Surface *screen, double pal_polozenie, Uint32 pixel);
	void wczytaj_plansze(bool nowa), wczytaj_z_pliku(char nazwa[250]),przegrana();
	void dopisz_do_rankingu(), spadanie_nagrod(), rozszerz_paletke(nagrody &bonus);
	void zmniejsz_paletke(nagrody &bonus), wysw_nagrody();
	void spawn_nagrody(int i, int j, rodzaj_nagrody nagroda,pilka kulka);
	void zeruj_wszystko(), przyklejaj(nagrody &bonus), dodaj_zycie(nagrody &bonus);
	void start_zwolnienia(nagrody &bonus), koniec_zwolnienia(nagrody &bonus);
	void roztrojenie(nagrody &bonus),zacznij_strzelac(nagrody &bonus);
	void wysw_strzaly(), ruch_pocisku(), strzal(), skoncz_strzelac(nagrody &bonus);
	void draw_swirl(SDL_Surface *surface, int n_cx, int n_cy, int radius, Uint32 pixel);
	void wysw_wir(SDL_Surface *screen, ruchome_obiekty obiekt, Uint32 pixel);
	void ruch_wiru(), zniszcz_wir(int i);
	void zderzenie_z_prawej(ruchome_obiekty &wir), zderzenie_z_lewej(ruchome_obiekty &wir);
	void zderzenie_z_dolu(ruchome_obiekty &wir), zderzenie_z_gory(ruchome_obiekty &wir);
	void spawn_wiru();

	int t1, t2, quit, frames, rc, ilosc_pilek, pal_wydluzenie, zycia, punkty, ilosc_klockow, zwolnienie, ilosc_strzalow, strzelanie;
	int ilosc_wirow;
	najlepsze_wyniki ranking[5];
	bool kleista;
	double delta, worldTime, fpsTimer, fps, pal_polozenie, pal_predkosc, pil_predkosc, zacznij_przyspieszac, ostatni_strzal;
	double ost_spawn_wiru,zaplanowany_czas_spawnu;
	plansza klocki[(int)klocki_w_poziomie][(int)klocki_w_pionie];
	pilka pilki[(int)max_pilki];
	pocisk pociski[(int)max_strzaly];
	nagrody bonusy[(int)max_nagrody];
	ruchome_obiekty wiry[(int)max_wiry];
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
};

void arkanoid::zeruj_wszystko()
{
	t1 = SDL_GetTicks();
	frames = 0;
	fpsTimer = 0;
	fps = 0;
	zacznij_przyspieszac = 0;
	quit = 0;
	worldTime = 0;
	pal_polozenie = PRAWA_KRAWEDZ/2-szer_pal/2;
	pal_predkosc = 0;
	ilosc_pilek = 1;
	pal_wydluzenie = 1;
	strzelanie = false;
	zaplanowany_czas_spawnu = min_czas_do_nowego_obiektu + rand() % 10;
	kleista = false;
	ilosc_strzalow = 0;
	ostatni_strzal = 0.0;
	ilosc_wirow = 0;
	ost_spawn_wiru = worldTime;
	zwolnienie = 0;
	pilki[0].kierunek = 135;
	pil_predkosc = PREDKOSC_POCZATKOWA;
	pilki[0].przyklejona = true;
	pilki[0].puszczona = false;
	pilki[0].podkrecenie = 0;
	pilki[0].y = DOLNA_KRAWEDZ-WYS_PAL_NAD_DOLNA_KRAWEDZ-wys_pal;
	pilki[0].x = ((szer_pal) / 2) + 1;//pilka na środku paletki
	for (int i = 0; i < max_nagrody; i++)
	{
		pociski[i].x = 0;
		pociski[i].y = 0;
	}
	for (int i = 0; i < max_nagrody; i++)
	{
		bonusy[i].x = bonusy[i].y = bonusy[i].predkosc = bonusy[i].koniec = 0;
		bonusy[i].nagroda = szeroka_paletka;
	}
}

void arkanoid::wczytaj_z_pliku(char nazwa[250])
{
	FILE *fp;
	ilosc_klockow = 0;
	if ((fp = fopen(nazwa, "r")) == NULL)
		return;
	char tekst[3];
	for (int i = 0; i < klocki_w_pionie; i++)
		for (int j = 0; j < klocki_w_poziomie; j++)
		{
			fscanf(fp, "%s", tekst);
			if (tekst[0] == '0' && tekst[0] == '0')
			{
				klocki[j][i].wytrzymałość = 0;
				klocki[j][i].kolor = czarny;
				klocki[j][i].aktywny = false;
				continue;
			}
			if (tekst[0] == '2')
			{
				klocki[j][i].wytrzymałość = 1;
				klocki[j][i].kolor = czarny;
				klocki[j][i].aktywny = true;
				continue;
			}
			/*
			
			
			*/
			if (tekst[0] == '1')
			{
				ilosc_klockow++;
				klocki[j][i].wytrzymałość = 0;
				klocki[j][i].aktywny = true;
				switch (tekst[1])
				{
				case '1':
					klocki[j][i].kolor = niebieski;
					break;
				case '2':
					klocki[j][i].kolor = czerwony;
					break;
				case '3':
					klocki[j][i].kolor = zielony;
					break;
				default:
					klocki[j][i].kolor = czarny;
					break;
				}
				continue;
			}
		}
	fclose(fp);
	return;
}

void arkanoid::dopisz_do_rankingu()
{
	char text[100];
	char nick[100];
	int i = 0;
	bool done = false;
	while (!done)
	{
		SDL_FillRect(screen, NULL, czarny);
		sprintf(text, "Przegrana");
		DrawString(screen, screen->w / 2 - strlen(text) * 4, 26, text, charset);

		sprintf(text, "Mozesz sie wpisac do rankingu. Podaj swoj nick");
		DrawString(screen, screen->w / 2 - strlen(text) * 4, screen->h / 2, text, charset);

		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN)
					done = true;
				else
				{
					nick[i++] = event.key.keysym.sym;
					nick[i] = NULL;
					sprintf(text, "%s", nick);
					DrawString(screen, screen->w / 2 - strlen(text) * 4, screen->h / 2 + 10, text, charset);
					SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
					SDL_RenderCopy(renderer, scrtex, NULL, NULL);
					SDL_RenderPresent(renderer);
				}
				break;
			}
		}
	}
	nick[i] = NULL;

	ranking[4].punkty = punkty;
	nick[26] = NULL;
	strcpy(ranking[4].nick, nick);
	for (int i = 4; i >= 1; i--)
		if (ranking[i].punkty > ranking[i - 1].punkty)
		{
			char pomn[100];
			int pomp=ranking[i].punkty;
			strcpy(pomn, ranking[i].nick);
			strcpy(ranking[i].nick, ranking[i - 1].nick);
			strcpy(ranking[i - 1].nick, pomn);
			ranking[i].punkty = ranking[i - 1].punkty;
			ranking[i - 1].punkty = pomp;
		}
	FILE *fp;
	if ((fp = fopen("ranking.txt", "w")) == NULL)
		return;
	for (int i = 0; i < 5;i++)
		fprintf(fp, "%s %d\n", ranking[i].nick, ranking[i].punkty);
	fclose(fp);
}

void arkanoid::przegrana()
{
	SDL_FillRect(screen, NULL, czerwony);
	char text[100];
	sprintf(text, "Przegrana");
	DrawString(screen, screen->w / 2 - strlen(text) * 4, 26, text, charset);
	
	sprintf(text, "Ranking");
	//DrawRectangle(screen, 3 * (screen->w / 5) + 1, 2.25 * (screen->h / 3) - 5, 2 * (screen->w / 5) - 2, 70, zolty, zielony_gleboki);
	DrawString(screen, screen->w / 2 - strlen(text) * 4, (screen->h / 3), text, charset);
	for (int i = 0; i < 5; i++)
	{
		sprintf(text, "%d %s", ranking[i].punkty, ranking[i].nick);
		DrawString(screen, screen->w / 2 - strlen(text) * 4, (screen->h / 3) + (i + 1) * 10, text, charset);
	}

	if (punkty>ranking[4].punkty)
		sprintf(text, "Mozesz sie wpisac do rankingu. Podaj swoj nick");
	else
		sprintf(text, "Masz za malo punktow zeby sie wpisac do rankingu");
	DrawString(screen, screen->w / 2 - strlen(text) * 4, screen->h/2, text, charset);
	
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
	if (punkty > ranking[4].punkty)
		dopisz_do_rankingu();

	sprintf(text, "n - gra od nowa; q - wyjscie");
	DrawString(screen, (screen->w / 2) - strlen(text) * 4, 2*(screen->h / 3), text, charset);
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
	while (1)
	{
		SDL_PollEvent(&event);
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'q')
		{
			exit(1);
			break;
		}
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == 'n')
		{
			punkty = 0;
			zycia = poczatkowe_zycia;
			wczytaj_plansze(true);
			zeruj_wszystko();
			break;
		}
	}

}

void arkanoid::wczytaj_plansze(bool nowa)
{
	FILE *fp;
	static int ostatnia = 0;
	int ile;
	char tekst[250];
	if ((fp = fopen("poziomy.txt", "r")) == NULL)
	{
		przykladowa_plansza();
		return;
	}
	fscanf(fp, "%d", &ile);
	if (ile==ostatnia || nowa)
	{
		fscanf(fp, "%s", tekst);
		wczytaj_z_pliku(tekst);
		ostatnia = 1;
	}
	else
	{
		int i;
		for (i = 0; i < ostatnia; i++)
			fscanf(fp, "%s", tekst);
		fscanf(fp, "%s", tekst);
		wczytaj_z_pliku(tekst);
		ostatnia++;
	}
	fclose(fp);
}

void arkanoid::wysw_klocki(SDL_Surface *screen)
{
	for (int i = 0; i < klocki_w_pionie; i++)
		for (int j = 0; j < klocki_w_poziomie; j++)
			if (klocki[j][i].aktywny==true)
				DrawRectangle(screen, LEWA_KRAWEDZ + (j*szerokosc_klocka)+2, GORNA_KRAWEDZ+i*wysokosc_klocka+1,
				szerokosc_klocka, wysokosc_klocka, kolor_tla_planszy, klocki[j][i].kolor);
}
// narysowanie napisu txt na powierzchni screen, zaczynając od punktu (x, y)
// charset to bitmapa 128x128 zawierająca znaki
void arkanoid::DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset)
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text)
	{
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	}
}

// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt środka obrazka sprite na ekranie
void arkanoid::DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y)
{
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
}

// rysowanie pojedynczego pixela
void arkanoid::DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color)
{
	if (x < 0 || y < 0 || x>=SCREEN_WIDTH || y>=SCREEN_HEIGHT)
		return;
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
}

// rysowanie linii o długości l w pionie (gdy dx = 0, dy = 1) 
// bądź poziomie (gdy dx = 1, dy = 0)
void arkanoid::DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color)
{
	for(int i = 0; i < l; i++)
	{
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	}
}

//okrąg w punkcie S=(n_cx,n_cy) i promieniu radius, koloru pxel
void arkanoid::draw_circle(SDL_Surface *surface, int n_cx, int n_cy, int radius, Uint32 pixel)
{
	double error = (double)-radius;
	double x = (double)radius +0.5;
	double y = (double)0.5;
	double cx = n_cx +0.5;
	double cy = n_cy +0.5;

	while (x >= y)
	{
		DrawPixel(surface, (int)(cx + x), (int)(cy + y), pixel);
		DrawPixel(surface, (int)(cx + y), (int)(cy + x), pixel);

		if (x != 0)
		{
			DrawPixel(surface, (int)(cx - x), (int)(cy + y), pixel);
			DrawPixel(surface, (int)(cx + y), (int)(cy - x), pixel);
		}

		if (y != 0)
		{
			DrawPixel(surface, (int)(cx + x), (int)(cy - y), pixel);
			DrawPixel(surface, (int)(cx - y), (int)(cy + x), pixel);
		}

		if (x != 0 && y != 0)
		{
			DrawPixel(surface, (int)(cx - x), (int)(cy - y), pixel);
			DrawPixel(surface, (int)(cx - y), (int)(cy - x), pixel);
		}

		error += y;
		++y;
		error += y;

		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}

void arkanoid::draw_swirl(SDL_Surface *surface, int n_cx, int n_cy, int radius, Uint32 pixel)
{
	//draw_circle(surface, n_cx, n_cy, radius, pixel);
	double x = (double)radius + 0.5;
	double y = (double)0.5;
	double cx = n_cx + 0.5;
	double cy = n_cy + 0.5;
	DrawLine(surface, cx - x, cy, x * 2, 1, 0, pixel);
	DrawLine(surface, cx, cy-x, x * 2, 0, 1, pixel);
	DrawLine(surface, cx, cy, x , -1, -1, pixel);
	DrawLine(surface, cx, cy, x, 1, 1, pixel);
	DrawLine(surface, cx, cy, x, 1, -1, pixel);
	DrawLine(surface, cx, cy, x, -1, 1, pixel);
}

void arkanoid::DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor)
{
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
}

bool arkanoid::odbicie_od_paletki(pilka &kulka,int n)
{
	if (abs(kulka.x - (pal_polozenie + ((szer_pal) / 2) + 2)) <= (szer_pal / 2) && kulka.puszczona == false)
	{
		kulka.kierunek = 360 - kulka.kierunek;
		kulka.kierunek += ((kulka.x - (pal_polozenie + ((szer_pal) / 2) + 2)) / (szer_pal / 2)) * (max_zmiana_kata / 2);
		if (kulka.kierunek > 180 - min_kat)
			kulka.kierunek = 180 - min_kat;
		if (kulka.kierunek < min_kat)
			kulka.kierunek = min_kat;
		if (pal_predkosc>0)
			kulka.podkrecenie = -max_podkrecenie;
		if (pal_predkosc<0)
			kulka.podkrecenie = +max_podkrecenie;
		if (pal_predkosc == 0)
			kulka.podkrecenie = 0;
		if (kleista == true)
		{
			kulka.x = kulka.x - pal_polozenie;
			kulka.przyklejona = true;
			kulka.podkrecenie = 0;
		}
	}
	else
	{
		kulka.puszczona = true;
		if (kulka.y <= DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal)
		{
			if ((kulka.y - (DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal))*(kulka.y - (DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal))
				+ (kulka.x - pal_polozenie)*(kulka.x - pal_polozenie) <= promien_pilki*promien_pilki)
			{
				kulka.kierunek = 360 - kulka.kierunek;
				kulka.kierunek -= max_zmiana_kata / 2 + ((promien_pilki - (pal_polozenie - kulka.x)) / promien_pilki)*(max_zmiana_kata / 2);
				if (kulka.kierunek > 180 - min_kat)
					kulka.kierunek = 180 - min_kat;
				if (kulka.kierunek < min_kat)
					kulka.kierunek = min_kat;
				kulka.puszczona = false;
				if (kleista == true)
				{
					kulka.x = 0;
					kulka.przyklejona = true;
					kulka.podkrecenie = 0;
				}
			}
			if ((kulka.y - (DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal))*(kulka.y - (DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal))
				+ (kulka.x - pal_polozenie - szer_pal)*(kulka.x - pal_polozenie - szer_pal) <= promien_pilki*promien_pilki)
			{
				kulka.kierunek = 360 - kulka.kierunek;
				kulka.kierunek += max_zmiana_kata / 2 + ((promien_pilki - (kulka.x - (pal_polozenie + szer_pal))) / promien_pilki)*(max_zmiana_kata / 2);
				if (kulka.kierunek > 180 - min_kat)
					kulka.kierunek = 180 - min_kat;
				if (kulka.kierunek < min_kat)
					kulka.kierunek = min_kat;
				kulka.puszczona = false;
				if (kleista == true)
				{
					kulka.x = szer_pal;
					kulka.przyklejona = true;
					kulka.podkrecenie = 0;
				}
			}
		}
		if (kulka.y >= DOLNA_KRAWEDZ )//- WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal)
		{
			if (ilosc_pilek == 1)
			{
				zeruj_wszystko();
				zycia--;
				if (zycia == 0)
					przegrana();
				return false;
			}
			else
			{
				for (int i = n; i < ilosc_pilek - 1; i++)
					pilki[i] = pilki[i + 1];
				ilosc_pilek--;
			}
		}
	}
	return true;
}

void arkanoid::rozszerz_paletke(nagrody &bonus)
{
	pal_wydluzenie++;
	if (pal_wydluzenie == 2)
	{
		for (int i = 0; i < ilosc_pilek; i++)
			if (pilki[i].przyklejona == true)
				pilki[i].x *= 2;
		pal_polozenie -= szer_pal / 4;
	}
	bonus.koniec = worldTime + 20;
	bonus.nagroda = szeroka_paletka;
	bonus.y = 0;
	punkty += punkty_za_nagrode;
}

void arkanoid::zmniejsz_paletke(nagrody &bonus)
{
	pal_wydluzenie--;
	if (pal_wydluzenie == 1)
	{
		for (int i = 0; i < ilosc_pilek; i++)
			if (pilki[i].przyklejona == true)
				pilki[i].x /= 2;
		pal_polozenie += szer_pal / 2;
	}
	bonus.koniec = 0;
}
void arkanoid::przyklejaj(nagrody &bonus)
{
	bonus.koniec = 0;
	kleista = true;
	bonus.y = 0;
	punkty += punkty_za_nagrode;
}

void arkanoid::dodaj_zycie(nagrody &bonus)
{
	bonus.koniec = 0;
	zycia++;
	bonus.y = 0;
	punkty += punkty_za_nagrode;
}

void arkanoid::start_zwolnienia(nagrody &bonus)
{
	zwolnienie++;
	bonus.koniec = worldTime + 20;
	bonus.y = 0;
	bonus.nagroda = zmiana_tempa;
	punkty += punkty_za_nagrode;
}
void arkanoid::koniec_zwolnienia(nagrody &bonus)
{
	zwolnienie--;
	if (zwolnienie==0)
		zacznij_przyspieszac = worldTime;
	bonus.koniec = 0;
}

void arkanoid::roztrojenie(nagrody &bonus)
{
	pilki[ilosc_pilek + 1] = pilki[ilosc_pilek] = pilki[0];
	pilki[ilosc_pilek + 1].kierunek += rand()%15+1;
	pilki[ilosc_pilek].kierunek -= rand()%15+1;
	pilki[ilosc_pilek + 1].podkrecenie = pilki[ilosc_pilek].podkrecenie = 0;
	ilosc_pilek += 2;
	bonus.y = 0;
	bonus.nagroda = wiele_pilek;
	bonus.koniec = 0;
	punkty += punkty_za_nagrode;
}

void arkanoid::zacznij_strzelac(nagrody &bonus)
{
	strzelanie++;
	bonus.y = 0;
	bonus.nagroda = strzelajaca_paletka;
	bonus.koniec = worldTime + 20;
	punkty += punkty_za_nagrode;
}

void arkanoid::skoncz_strzelac(nagrody &bonus)
{
	strzelanie--;
	bonus.koniec = 0;
}

void arkanoid::spadanie_nagrod()
{
	for (int i = 0; i < max_nagrody; i++)
	{
		if (bonusy[i].koniec == 0 && bonusy[i].y == 0)
			continue;
		if (bonusy[i].koniec == 0)
		{
			bonusy[i].predkosc += nag_przyspieszenie;
			bonusy[i].y += bonusy[i].predkosc*delta;
			if (bonusy[i].y + promien_nagrody >= DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal &&
				bonusy[i].y - promien_nagrody <= DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ &&
				bonusy[i].x >= pal_polozenie && bonusy[i].x <= pal_polozenie + szer_pal)
			{
				switch (bonusy[i].nagroda)
				{
				case szeroka_paletka:
					rozszerz_paletke(bonusy[i]);
					break;
				case kleista_paletka:
					przyklejaj(bonusy[i]);
					break;
				case zmiana_tempa:
					start_zwolnienia(bonusy[i]);
					break;
				case dodatkowe_zycie:
					dodaj_zycie(bonusy[i]);
					break;
				case wiele_pilek:
					roztrojenie(bonusy[i]);
					break;
				case strzelajaca_paletka:
					zacznij_strzelac(bonusy[i]);
					break;
				}
			}
			if (bonusy[i].y - promien_nagrody > DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ)
			{
				bonusy[i].y = 0;
				bonusy[i].koniec = 0;
			}
		}
		else
		{
			if (worldTime >= bonusy[i].koniec)
				switch (bonusy[i].nagroda)
			{
				case szeroka_paletka:
					zmniejsz_paletke(bonusy[i]);
					break;
				case zmiana_tempa:
					koniec_zwolnienia(bonusy[i]);
					break;
				case strzelajaca_paletka:
					skoncz_strzelac(bonusy[i]);
					break;
				default:
					break;
			}
		}
	}
}

void arkanoid::spawn_nagrody(int i, int j, rodzaj_nagrody nagroda,pilka kulka)
{
	for (int n = 0; n < max_nagrody; n++)
		if (bonusy[n].koniec == 0 && bonusy[n].y == 0)
		{
			bonusy[n].x = szerokosc_klocka*i + szerokosc_klocka / 2;//kulka.x;//i*(PRAWA_KRAWEDZ / klocki_w_poziomie) + szer_pal / 2;
			bonusy[n].y = wysokosc_klocka*j + wysokosc_klocka / 2;//kulka.y;//(j + 1)*(GORNA_KRAWEDZ / wysokosc_klocka);
			bonusy[n].nagroda = nagroda;
			bonusy[n].predkosc = 0;
			bonusy[n].koniec = 0;
			break;
		}
}

void arkanoid::trafiony(int i, int j,pilka kulka)
{
	if (!klocki[i][j].wytrzymałość)
	{
		ilosc_klockow--;
		klocki[i][j].aktywny = false;
		punkty += punkty_za_zbicie;
		if (ilosc_klockow == 0)
		{
			punkty += punkty_za_skonczenie;
			zeruj_wszystko();
			wczytaj_plansze(false);
			return;
		}
		if (rand() % 100 < prawd_nagrody * 100)
		{
			switch (rand() % 6)
			{
			case 0:
				spawn_nagrody(i, j, szeroka_paletka, kulka);
				break;
			case 1:
				spawn_nagrody(i, j, kleista_paletka, kulka);
				break;
			case 2:
				spawn_nagrody(i, j, zmiana_tempa, kulka);
				break;
			case 3:
				spawn_nagrody(i, j, dodatkowe_zycie, kulka);
				break;
			case 4:
				spawn_nagrody(i, j, wiele_pilek, kulka);
				break;
			case 5:
				spawn_nagrody(i, j, strzelajaca_paletka, kulka);
				break;
			}
		}
	}
}

bool arkanoid::zderzenie_z_klockiem(pilka &kulka, double x, double y, double szer, double wys,int i, int j)
{
	if (klocki[i][j].aktywny == false)
		return false;
	if (kulka.x >= x && kulka.x <= x + szer)
	{
		if ((kulka.y>y + wys && kulka.y - (y + wys) <= promien_pilki))
		{
			zderzenie_z_dolu(kulka);
			trafiony(i, j,kulka);
			return true;
		}
		if ((y>kulka.y && y-kulka.y<=promien_pilki))
		{
			zderzenie_z_gory(kulka);
			trafiony(i, j, kulka);
			return true;
		}
	}
	if (kulka.y >= y && kulka.y <= y + wys)
	{
		if (kulka.x>x + wys && kulka.x - (x + szer) <= promien_pilki)
		{
			zderzenie_z_lewej(kulka);
			trafiony(i, j, kulka);
			return true;
		}
		if (x>kulka.x && x - kulka.x <= promien_pilki)
		{
			zderzenie_z_prawej(kulka);
			trafiony(i, j, kulka);
			return true;
		}
	}
	if ((kulka.y - y)*(kulka.y - y) + (kulka.x - x)*(kulka.x - x) <= promien_pilki*promien_pilki)
	{
		if (kulka.kierunek <= 180)
			zderzenie_z_prawej(kulka);
		else if (kulka.kierunek >= 270)
			zderzenie_z_dolu(kulka);
		else
		{
			if (tan(kulka.kierunek) >= (y - kulka.y) / (x - kulka.x))
				zderzenie_z_dolu(kulka);
			else
				zderzenie_z_prawej(kulka);
		}
		trafiony(i, j, kulka);
		return true;
	}
	if (((kulka.y - y)*(kulka.y - y) + (kulka.x - x - szer)*(kulka.x - x - szer) <= promien_pilki*promien_pilki))
	{
		if (kulka.kierunek < 90)
			zderzenie_z_lewej(kulka);
		else if (kulka.kierunek > 180)
			zderzenie_z_dolu(kulka);
		else
		{
			if (tan(kulka.kierunek) >= (y - kulka.y) / (kulka.x - (x + szer)))
				zderzenie_z_dolu(kulka);
			else
				zderzenie_z_lewej(kulka);
		}
		trafiony(i, j, kulka);
		return true;
	}
	if ((kulka.y - y - wys)*(kulka.y - y - wys) + (kulka.x - x)*(kulka.x - x) <= promien_pilki*promien_pilki)
	{
		if (kulka.kierunek <= 90)
			zderzenie_z_gory(kulka);
		else if (kulka.kierunek >= 180)
			zderzenie_z_prawej(kulka);
		else
		{
			if (tan(kulka.kierunek) <= (kulka.y - (y + wys)) / (x - kulka.x))
				zderzenie_z_gory(kulka);
			else
				zderzenie_z_prawej(kulka);
		}
		trafiony(i, j, kulka);
		return true;
	}
	if((kulka.y - y - wys)*(kulka.y - y - wys) + (kulka.x - x - szer)*(kulka.x - x - szer) <= promien_pilki*promien_pilki)
	{
		if (kulka.kierunek >= 270)
			zderzenie_z_lewej(kulka);
		else if (kulka.kierunek <= 90)
			zderzenie_z_gory(kulka);
		else
		{
			if (tan(kulka.kierunek) <= (kulka.y - (y + wys)) / (kulka.x-(x+szer)))
				zderzenie_z_gory(kulka);
			else
				zderzenie_z_lewej(kulka);
		}
		trafiony(i, j, kulka);
		return true;
	}
	return false;
}

void arkanoid::zderzenie_z_prawej(pilka &kulka)
{
	kulka.kierunek = 180 * (int)(kulka.kierunek / 180 + 1) - (kulka.kierunek - (180 * (int)(kulka.kierunek / 180)));
	kulka.podkrecenie = 0;
}
void arkanoid::zderzenie_z_lewej(pilka &kulka)
{
	kulka.kierunek = (kulka.kierunek > 180) ? 540 - kulka.kierunek : 180 - kulka.kierunek;
	kulka.podkrecenie = 0;
}
void arkanoid::zderzenie_z_gory(pilka &kulka)
{
	kulka.kierunek = 360 - kulka.kierunek;
	kulka.podkrecenie = 0;
}
void arkanoid::zderzenie_z_dolu(pilka &kulka)
{
	kulka.kierunek = 360 - kulka.kierunek;
	kulka.podkrecenie = 0;
}

void arkanoid::ruch_pilki(pilka &kulka,double delta,double predkosc,int n)
{
	kulka.kierunek += kulka.podkrecenie*(predkosc*((zwolnienie)?0.5:1))*delta;
	kulka.y -= sin((kulka.kierunek / 180)*M_PI)*(predkosc*((zwolnienie) ? 0.5 : 1))*delta;
	kulka.x -= cos((kulka.kierunek / 180)*M_PI)*(predkosc*((zwolnienie) ? 0.5 : 1))*delta;
	if (kulka.y - promien_pilki < 0)
		kulka.y = promien_pilki;
	if (kulka.x - promien_pilki < 0)
		kulka.x = promien_pilki;
	if (kulka.x + promien_pilki > PRAWA_KRAWEDZ)
		kulka.x = PRAWA_KRAWEDZ - promien_pilki;

	if (kulka.x+promien_pilki >= PRAWA_KRAWEDZ)
	{
		zderzenie_z_prawej(kulka);
	}
	if (kulka.y-promien_pilki <= 0)
	{
		zderzenie_z_gory(kulka);
	}
	if (kulka.x-promien_pilki <= 0)
	{
		zderzenie_z_lewej(kulka);
	}
	for (int i = (int)((kulka.x - promien_pilki - 2) / szerokosc_klocka); i < (kulka.x + promien_pilki) / szerokosc_klocka; i++)
		for (int j = (int)((kulka.y - promien_pilki - 2) / (wysokosc_klocka)); j < (kulka.y + promien_pilki) / (wysokosc_klocka); j++)
			if (j<klocki_w_pionie && i<klocki_w_poziomie && j >= 0 && i >= 0)
				if (zderzenie_z_klockiem(kulka, i*szerokosc_klocka, j*wysokosc_klocka,
					szerokosc_klocka, wysokosc_klocka, i, j) == true)
				{
					i = klocki_w_poziomie;
					j = klocki_w_pionie;
				}
	if ((int)kulka.kierunek % 180 < min_kat || 180 - ((int)kulka.kierunek % 180) < min_kat)
	{
		kulka.podkrecenie = 0;
		if (kulka.kierunek < min_kat)
			kulka.kierunek = min_kat + rand() % 5;
		else if (360 - kulka.kierunek<min_kat)
			kulka.kierunek = min_kat - rand() % 5;
		else if (180 + min_kat >= kulka.kierunek && kulka.kierunek - min_kat <= 180)
			kulka.kierunek = 180 + min_kat + rand() % 5;
		else if (180 - min_kat <= kulka.kierunek && kulka.kierunek + min_kat >= 180)
			kulka.kierunek = 180 - min_kat - rand() % 5;
	}
	if (kulka.y > DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal - promien_pilki && kulka.kierunek>180)
		odbicie_od_paletki(kulka,n);
	for (int i = 0; i < ilosc_wirow; i++)
		if ((kulka.x - wiry[i].x)*(kulka.x - wiry[i].x) + (kulka.y - wiry[i].y)*(kulka.y - wiry[i].y) <= (promien_pilki + promien_wiru)*(promien_pilki + promien_wiru))
		{
			zniszcz_wir(i);
			kulka.kierunek = rand() % 360;
		}
}

void arkanoid::wysw_pilke(SDL_Surface *screen, pilka kulka, Uint32 pixel)
{
	draw_circle(screen, kulka.x+LEWA_KRAWEDZ+((kulka.przyklejona)?pal_polozenie:0), kulka.y+GORNA_KRAWEDZ, promien_pilki, pixel);
}

void arkanoid::wysw_wir(SDL_Surface *screen, ruchome_obiekty obiekt, Uint32 pixel)
{
	draw_swirl(screen, obiekt.x + LEWA_KRAWEDZ, obiekt.y + GORNA_KRAWEDZ, promien_wiru, pixel);
}

void arkanoid::wysw_nagrody()
{
	for (int i = 0; i < max_nagrody; i++)
	{
		if (bonusy[i].y == 0)
			continue;
		draw_circle(screen, bonusy[i].x + LEWA_KRAWEDZ, bonusy[i].y + GORNA_KRAWEDZ, promien_nagrody, zielony);
		switch (bonusy[i].nagroda)
		{
		case szeroka_paletka:
			DrawString(screen, bonusy[i].x+2, bonusy[i].y+2, "S", charset);
			break;
		case kleista_paletka:
			DrawString(screen, bonusy[i].x + 2, bonusy[i].y + 2, "K", charset);
			break;
		case zmiana_tempa:
			DrawString(screen, bonusy[i].x + 2, bonusy[i].y + 2, "Z", charset);
			break;
		case dodatkowe_zycie:
			DrawString(screen, bonusy[i].x + 2, bonusy[i].y + 2, "D", charset);
			break;
		case wiele_pilek:
			DrawString(screen, bonusy[i].x + 2, bonusy[i].y + 2, "W", charset);
			break;
		case strzelajaca_paletka:
			DrawString(screen, bonusy[i].x + 2, bonusy[i].y + 2, "F", charset);
			break;
		}
	}
}

void arkanoid::wysw_paletke(SDL_Surface *screen, double pal_polozenie, Uint32 pixel)
{
	DrawRectangle(screen, LEWA_KRAWEDZ + 1 + pal_polozenie, DOLNA_KRAWEDZ + GORNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal+1,
		szer_pal, wys_pal, bialy, bialy);

}

void arkanoid::strzal()
{
	if (ilosc_strzalow+1 < max_strzaly)
	{
		pociski[ilosc_strzalow].x = pal_polozenie + 5;
		pociski[ilosc_strzalow].y = DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal;
		pociski[ilosc_strzalow + 1].x = pal_polozenie + szer_pal - 5;
		pociski[ilosc_strzalow + 1].y = DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal;
		ilosc_strzalow += 2;
	}
}

void arkanoid::ruch_pocisku()
{
	for (int i = 0; i < ilosc_strzalow; i++)
	{
		pociski[i].y -= poc_predkosc*delta;
		if ((int)((pociski[i].y) / (wysokosc_klocka)) < klocki_w_pionie)
		{
			if (klocki[(int)((pociski[i].x) / szerokosc_klocka)][(int)((pociski[i].y) / (wysokosc_klocka))].aktywny == true)
			{
				pilka pom;
				pom.x = pociski[i].x;
				pom.y = pociski[i].y;
				trafiony((int)((pociski[i].x) / szerokosc_klocka),
					(int)((pociski[i].y) / (wysokosc_klocka)) , pom);
				for (int j = i; j < ilosc_strzalow - 1; j++)
					pociski[i] = pociski[i + 1];
				ilosc_strzalow--;
			}
		}
		if (pociski[i].y <= 0)
		{
			for (int j = i; j < ilosc_strzalow - 1; j++)
				pociski[i] = pociski[i + 1];
			ilosc_strzalow--;
		}
	}
}

bool arkanoid::zderzenie_z_klockiem(ruchome_obiekty &wir, double x, double y, double szer, double wys, int i, int j)
{
	if (klocki[i][j].aktywny == false)
		return false;
	if (wir.x >= x && wir.x <= x + szer)
	{
		if ((wir.y>y + wys && wir.y - (y + wys) <= promien_wiru))
		{
			zderzenie_z_gory(wir);
			return true;
		}
		if ((y > wir.y && y - wir.y <= promien_wiru))
		{
			zderzenie_z_dolu(wir);
			return true;
		}
	}
	if (wir.y >= y && wir.y <= y + wys)
	{
		if (wir.x > x + wys && wir.x - (x + szer) <= promien_wiru)
		{
			zderzenie_z_lewej(wir);
			return true;
		}
		if (x > wir.x && x - wir.x <= promien_wiru)
		{
			zderzenie_z_prawej(wir);
			return true;
		}
	}
	
	if ((wir.y - y)*(wir.y - y) + (wir.x - x)*(wir.x - x) <= promien_wiru*promien_wiru)
	{
		if (wir.kierunek <= 225)
			zderzenie_z_dolu(wir);
		else
			zderzenie_z_prawej(wir);
		return true;
	}
	
	if (((wir.y - y)*(wir.y - y) + (wir.x - x - szer)*(wir.x - x - szer) <= promien_wiru*promien_wiru))
	{
		if (wir.kierunek > 315 || wir.kierunek < 90)
			zderzenie_z_lewej(wir);
		else
			zderzenie_z_gory(wir);
		return true;
	}
	
	if ((wir.y - y - wys)*(wir.y - y - wys) + (wir.x - x)*(wir.x - x) <= promien_wiru*promien_wiru)
	{
		if (wir.kierunek <= 135)
			zderzenie_z_dolu(wir);
		else
			zderzenie_z_prawej(wir);
		return true;
	}
	
	if ((wir.y - y - wys)*(wir.y - y - wys) + (wir.x - x - szer)*(wir.x - x - szer) <= promien_wiru*promien_wiru)
	{
		if (wir.kierunek <= 45 || wir.kierunek>=270)
			zderzenie_z_gory(wir);
		else
			zderzenie_z_dolu(wir);
		return true;
	}
	return false;
}

void arkanoid::ruch_wiru()
{
	for (int i = 0; i < ilosc_wirow; i++)
	{
		wiry[i].y -= sin((wiry[i].kierunek / 180)*M_PI)*predkosc_wiru*delta;
		wiry[i].x -= cos((wiry[i].kierunek / 180)*M_PI)*predkosc_wiru*delta;

		if (wiry[i].y - promien_wiru < 0)
			wiry[i].y = promien_wiru;
		if (wiry[i].x - promien_wiru < 0)
			wiry[i].x = promien_wiru;
		if (wiry[i].x + promien_wiru > PRAWA_KRAWEDZ)
			wiry[i].x = PRAWA_KRAWEDZ - promien_wiru;
		if (wiry[i].y + promien_wiru > DOLNA_KRAWEDZ)
			wiry[i].y = DOLNA_KRAWEDZ - promien_wiru;

		if (wiry[i].x + promien_wiru >= PRAWA_KRAWEDZ)
			zderzenie_z_prawej(wiry[i]);
		if (wiry[i].y - promien_wiru <= 0)
			zderzenie_z_gory(wiry[i]);
		if (wiry[i].x - promien_wiru <= 0)
			zderzenie_z_lewej(wiry[i]);
		if (wiry[i].y + promien_wiru >= DOLNA_KRAWEDZ)
			zderzenie_z_dolu(wiry[i]);

		if (worldTime - wiry[i].ost_zmiana >= max_czas_w_jednym_kierunku)
		{
			wiry[i].kierunek += (rand() % 20) - 10;
			wiry[i].ost_zmiana = worldTime;
		}

		if (wiry[i].max_x<wiry[i].x)
			wiry[i].max_x = wiry[i].x;
		if (wiry[i].min_x>wiry[i].x)
			wiry[i].min_x = wiry[i].x;
		if (wiry[i].max_y<wiry[i].y)
			wiry[i].max_y = wiry[i].y;
		if (wiry[i].min_y>wiry[i].y)
			wiry[i].min_y = wiry[i].y;

		if (worldTime - wiry[i].ost_kumulacja >= czas_kumulacji)//sprawdzanie_kumulacji
		{
			if (wiry[i].max_x - wiry[i].min_x <= min_delta_polozenia && wiry[i].max_y - wiry[i].min_y <= min_delta_polozenia)
			{
				if (rand() % 2)
				{
					if ((wiry[i].max_x + wiry[i].min_x) / 2 > PRAWA_KRAWEDZ / 2)
						wiry[i].kierunek = 0;
					else
						wiry[i].kierunek = 180;
				}
				else
				{
					if ((wiry[i].max_y + wiry[i].min_y) / 2 > DOLNA_KRAWEDZ / 2)
						wiry[i].kierunek = 90;
					else
						wiry[i].kierunek = 270;
				}
			}
			wiry[i].ost_kumulacja = worldTime;
			wiry[i].max_x = 0;
			wiry[i].max_y = 0;
			wiry[i].min_x = PRAWA_KRAWEDZ;
			wiry[i].min_y = LEWA_KRAWEDZ;
		}

		for (int k = (int)((wiry[i].x - promien_wiru - 2) / szerokosc_klocka); k < (wiry[i].x + promien_wiru) / szerokosc_klocka; k++)
			for (int j = (int)((wiry[i].y - promien_wiru - 2) / (wysokosc_klocka)); j < (wiry[i].y + promien_wiru) / (wysokosc_klocka); j++)
				if (j<klocki_w_pionie && k<klocki_w_poziomie && j >= 0 && k >= 0)
					if (zderzenie_z_klockiem(wiry[i], k*szerokosc_klocka, j*wysokosc_klocka,
						szerokosc_klocka, wysokosc_klocka, k, j) == true)
					{
						k = klocki_w_poziomie;
						j = klocki_w_pionie;
					}
		if (wiry[i].x + promien_wiru >= pal_polozenie && wiry[i].x - promien_wiru <= pal_polozenie + szer_pal &&
			wiry[i].y + promien_wiru >= DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal &&
			wiry[i].y - promien_wiru <= DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ)
			zniszcz_wir(i);
	}
}

void arkanoid::zniszcz_wir(int i)
{
	ilosc_wirow--;
	for (int j = i; j < ilosc_wirow; j++)
	{
		wiry[j] = wiry[j + 1];
	}
}

void arkanoid::spawn_wiru()
{
	if (ilosc_wirow >= max_wiry)
		return;
	if (ost_spawn_wiru + zaplanowany_czas_spawnu <= worldTime)
	{
		wiry[ilosc_wirow].x = PRAWA_KRAWEDZ;
		wiry[ilosc_wirow].y = DOLNA_KRAWEDZ / 2;
		wiry[ilosc_wirow].kolor = zolty;
		wiry[ilosc_wirow].kierunek = rand() % 360;
		wiry[ilosc_wirow].ost_zmiana = worldTime;
		wiry[ilosc_wirow].ost_kumulacja = worldTime;
		wiry[ilosc_wirow].max_x = 0;
		wiry[ilosc_wirow].max_y = 0;
		wiry[ilosc_wirow].min_x = PRAWA_KRAWEDZ;
		wiry[ilosc_wirow].min_y = LEWA_KRAWEDZ;
		zaplanowany_czas_spawnu = min_czas_do_nowego_obiektu + rand() % 10;
		ost_spawn_wiru = worldTime;
		ilosc_wirow++;
	}
}

void arkanoid::zderzenie_z_prawej(ruchome_obiekty &wir)
{
	wir.kierunek = rand() % 89 + (rand() % 2) ? 0 : 271;
	wir.x -= 2;
	wir.ost_zmiana = worldTime;
}
void arkanoid::zderzenie_z_lewej(ruchome_obiekty &wir)
{
	wir.kierunek = 91 + rand() % 179;
	wir.x += 2;
	wir.ost_zmiana = worldTime;
}
void arkanoid::zderzenie_z_gory(ruchome_obiekty &wir)
{
	wir.kierunek = 181 + rand() % 179;
	wir.y += 2;
	wir.ost_zmiana = worldTime;
}
void arkanoid::zderzenie_z_dolu(ruchome_obiekty &wir)
{
	wir.kierunek = 1 + rand() % 179;
	wir.y -= 2;
	wir.ost_zmiana = worldTime;
}

void arkanoid::wysw_strzaly()
{
	for (int i = 0; i < ilosc_strzalow; i++)
	{
		draw_circle(screen, pociski[i].x + LEWA_KRAWEDZ, pociski[i].y + GORNA_KRAWEDZ, 2, czerwony);
		DrawPixel(screen, pociski[i].x + LEWA_KRAWEDZ, pociski[i].y + GORNA_KRAWEDZ, czerwony);
		DrawPixel(screen, pociski[i].x - 1 + LEWA_KRAWEDZ, pociski[i].y + GORNA_KRAWEDZ, czerwony);
		DrawPixel(screen, pociski[i].x + 1 + LEWA_KRAWEDZ, pociski[i].y + GORNA_KRAWEDZ, czerwony);
		DrawPixel(screen, pociski[i].x + LEWA_KRAWEDZ, pociski[i].y - 1 + GORNA_KRAWEDZ, czerwony);
		DrawPixel(screen, pociski[i].x + LEWA_KRAWEDZ, pociski[i].y + 1 + GORNA_KRAWEDZ, czerwony);
	}
}

void arkanoid::info()
{
	char text[100];
	sprintf(text, "Pozostala ilosc rozgrywek");
	DrawRectangle(screen, 3 * (screen->w / 5)+1, 20, 2 * (screen->w / 5)-2, 30, zielony, tlo_zycia);
	DrawString(screen, 4*(screen->w / 5) - strlen(text) * 4, 26, text, charset);
	sprintf(text, "\3%d", zycia);
	DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, 36, text, charset);

	sprintf(text, "Zdobyte punkty");
	DrawRectangle(screen, 3 * (screen->w / 5) + 1, screen->h / 5 - 5, 2 * (screen->w / 5) - 2, 30, zielony, tlo_punkty);
	DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, screen->h/5, text, charset);
	sprintf(text, "%d", punkty);
	DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, screen->h / 5 +10, text, charset);

	sprintf(text, "Ranking");
	DrawRectangle(screen, 3 * (screen->w / 5) + 1, 2.25 * (screen->h / 3) - 5, 2 * (screen->w / 5) - 2, 70, zolty, zielony_gleboki);
	DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, 2.25*(screen->h / 3), text, charset);
	for (int i = 0; i < 5; i++)
	{
		sprintf(text, "%d %s", ranking[i].punkty,ranking[i].nick);
		DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, 2.25 * (screen->h / 3) + (i+1)*10, text, charset);
	}

	sprintf(text, "Bonusy");
	DrawRectangle(screen, 3 * (screen->w / 5) + 1, (screen->h / 3) - 5, 2 * (screen->w / 5) - 2, 150, zolty, tlo_bonusow);
	DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, (screen->h / 3), text, charset);
	int n = 0;
	for (int i = 0; i < max_nagrody; i++)
	{
		if (bonusy[i].koniec == 0)
			continue;
		switch (bonusy[i].nagroda)
		{
		case szeroka_paletka:
			sprintf(text, "szeroka paletka - %f",bonusy[i].koniec-worldTime);
			DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, (screen->h / 3) + ((n++) + 1) * 10, text, charset);
			break;
		case zmiana_tempa:
			sprintf(text, "Zwolnienie - %f", bonusy[i].koniec - worldTime);
			DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, (screen->h / 3) + ((n++) + 1) * 10, text, charset);
			break;
		case strzelajaca_paletka:
			sprintf(text, "Strzelanie - %f", bonusy[i].koniec - worldTime);
			DrawString(screen, 4 * (screen->w / 5) - strlen(text) * 4, (screen->h / 3) + ((n++) + 1) * 10, text, charset);
			break;
		}
	}
}

void arkanoid::glowna()
{
	srand(SDL_GetTicks());
	//przykladowa_plansza();
	wczytaj_plansze(true);
	char text[100];
	while (!quit)
	{
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		spawn_wiru();

		pal_polozenie += pal_predkosc * delta;
		if (pal_polozenie <= 0)
		{
			pal_polozenie = 0;
			pal_predkosc = 0;
		}
		if (pal_polozenie >= PRAWA_KRAWEDZ - szer_pal - 2)//te 2 to 2 piksele białej krawędzi planszy
		{
			pal_polozenie = PRAWA_KRAWEDZ - szer_pal - 2;
			pal_predkosc = 0;
		}

		for (int i = 0; i < ilosc_pilek; i++)
			if (pilki[i].przyklejona == true)
			{
				pilki[i].y = DOLNA_KRAWEDZ - WYS_PAL_NAD_DOLNA_KRAWEDZ - wys_pal - promien_pilki;
			}
			else
			{
				ruch_pilki(pilki[i], delta, pil_predkosc, i);
			}

		//tło
		SDL_FillRect(screen, NULL, czarny);

		fpsTimer += delta;
		if (fpsTimer > 0.5)
		{
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		}

		if (CZAS_PRZYSPIESZANIA - (worldTime-zacznij_przyspieszac) > 0)
			pil_predkosc = ((worldTime-zacznij_przyspieszac) / CZAS_PRZYSPIESZANIA)*PREDKOSC_KONCOWA + PREDKOSC_POCZATKOWA;

		//plansza gry
		DrawRectangle(screen, LEWA_KRAWEDZ, GORNA_KRAWEDZ, PRAWA_KRAWEDZ, DOLNA_KRAWEDZ, bialy, kolor_tla_planszy);
		//paletka
		wysw_paletke(screen, pal_polozenie, bialy);
		spadanie_nagrod();
		ruch_pocisku();
		ruch_wiru();
		for (int i = 0; i < ilosc_wirow; i++)
			wysw_wir(screen, wiry[i], wiry[i].kolor);
		wysw_strzaly();
		wysw_klocki(screen);
		//rysowanie pilek
		
		wysw_nagrody();
		for (int i = 0; i < ilosc_pilek; i++)
			wysw_pilke(screen, pilki[i], bialy);
		
		info();

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == 's')
				{
					for (int i = 0; i < ilosc_pilek; i++)
						if (pilki[i].przyklejona == true)
						{
							pilki[i].x += pal_polozenie;
							pilki[i].przyklejona = false;
						}
				}
				else if (event.key.keysym.sym == 'f'){
					if (strzelanie && worldTime - ostatni_strzal>0.5){ strzal(); ostatni_strzal = worldTime; }}
				else if (event.key.keysym.sym == SDLK_LEFT)pal_predkosc = -szybkosc_paletki;
				else if (event.key.keysym.sym == SDLK_RIGHT)pal_predkosc = szybkosc_paletki;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT)
					pal_predkosc = 0;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			}
		}
		frames++;
	}

	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void arkanoid::przykladowa_plansza()
{
	return;
	for (int j = 0; j < klocki_w_pionie;j++)
		for (int i = 0; i < klocki_w_poziomie; i++)
		{
			klocki[i][j].aktywny = 0;
			klocki[i][j].kolor = bialy;
			klocki[i][j].wytrzymałość = 0;
		}
	for (int i = 0; i < klocki_w_poziomie; i++)
	{
		klocki[i][5].aktywny = 1;
		klocki[i][5].kolor = niebieski;
		klocki[i][5].wytrzymałość = 0;
	}
	for (int i = 0; i < klocki_w_poziomie; i++)
	{
		klocki[i][6].aktywny = 1;
		klocki[i][6].kolor = czerwony;
		klocki[i][6].wytrzymałość = 0;
	}
	for (int i = 0; i < klocki_w_poziomie; i++)
	{
		klocki[i][7].aktywny = 1;
		klocki[i][7].kolor = zielony;
		klocki[i][7].wytrzymałość = 0;
	}
	for (int i = 0; i < klocki_w_poziomie; i++)
	{
		klocki[i][8].aktywny = 1;
		klocki[i][8].kolor = niebieski;
		klocki[i][8].wytrzymałość = 0;
	}
}

arkanoid::arkanoid()
{
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(1);
	}

	if (fullscreen)
		rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	else
		rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if (rc != 0)
	{
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "149174 projekt 2");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	// wyłączenie widoczności kursora myszy
	SDL_ShowCursor(SDL_DISABLE);
	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL)
	{
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		exit(1);
		//return 1;
	}
	SDL_SetColorKey(charset, true, 0x000000);
	punkty = 0;
	zycia = poczatkowe_zycia;
	zeruj_wszystko();
	FILE *fp;
	if ((fp = fopen("ranking.txt", "r")) == NULL)
		return;
	for (int i = 0; i < 5; i++)
	{
		fscanf(fp, "%s", ranking[i].nick);
		fscanf(fp, "%d", &ranking[i].punkty);
	}
	fclose(fp);
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv)
{
	arkanoid gierka;
	gierka.glowna();
	return 0;
}
