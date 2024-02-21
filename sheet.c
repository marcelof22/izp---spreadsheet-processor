/**
 * @name IZP spreadsheet processor
 * @file sheet.c
 * @author Marcel Feiler - xfeile 00
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//--------------------------DEKLARACIA FUNKCII-------------------------------------------
int delim(char *ch, char* *p, int argc, char* argv[]); //ukazovatel na string
int kontrola_parametrov(int argc, char* argv[]);
int kontrola_N(char* s);
int kontrola_append(char* s);
int kontrola_dlzky(int p, char* s[]);
int vstup(char* p, char ch, int argc, char* argv[]);
int dalsi_riadok(char (*arr)[101], char* p, int *b, bool *koniec);
int vyprazdnenie_cyklu(char (*arr)[101], int b);
int zistim_argumenty(int argc, char* argv[], char* (*prikazy)[10241],int *m, int *selekcia);
int triedenie_argumentov(int argc, char* argv[], char* (*r_prikazy)[10241], char* (*c_prikazy)[10241], int *m_r, int *m_c, char* (*index_r)[10241], char* (*index_c)[10241], char* (*sel)[10241]);
int vetvenie(int *a, char* (*prikazy)[10241], int j);
int vypis (char arr[10241][101], int n_cols, char ch);
int r_upravy(char (*arr)[10241][101], char* p, int *m_r, int b, int *n_row, char* (*prikazy)[10241], int j, char* index_r[10241], int *vypisanie, char (*pomoc)[10241][101], char (*zamen)[10241][101], int *zamena, int *arow, int *help, int *p_opak, int *ind3);
int c_upravy(char (*arr)[10241][101], int *m_c, int *b, int *n_cols, char* (*prikazy)[10241], int j, char* index_c[10241], int *vymaz_c);
int sel_riadku(char arr[10241][101], char* sel[10241], int n_row, bool koniec, int n_cols);
//----------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
	
	char* p;
	char ch;	

	int ret_code = delim(&ch, &p, argc, argv);

	if (ret_code != 0) {
		return ret_code;
	}
	
	ret_code = kontrola_parametrov(argc, argv);
	if (ret_code != 0) {
		return ret_code;
	}

	ret_code = vstup(p, ch, argc, argv);
	if (ret_code != 0) {
		return ret_code;
	}
   	
	return 0;

}

//----------------------------------------------------------------------------------------

/*
 * Funkcia delim:
 *-------------
 *	*p vstupno-vystupny parameter, uklada hodnotu DELIM-u
 *	*ch ukazatel na prvy znak DELIM-u 
 * 
 * 	Vracia pripadnu chybovu hlasku
 */


int delim(char *ch, char* *p, int argc, char* argv[]) {

	*p = " "; //nastavenie povodnej hodnoty DELIM-u
	if (argc > 2) {
		if (strcmp(argv[1], "-d") == 0) {
			*p = argv[2];
			*ch = argv [2][0];
		}
	}
	if (argc == 2 && strcmp(argv[1], "-d") == 0) {	
		fprintf(stderr, "No delim after -d\n"); //ked po -d nic nenasleduje
		return 1;				
	}
	
	return 0; 
		
}


/*
 *Funkcia kontrola_parametrov:
 *----------------------------
 *	Kontroluje korektnost vstupu
 *
 *	argc pocet argumentov
 * 	*argv argumenty
 *
 * 	Vracia pripadne chybove hlasky
 */


int kontrola_parametrov(int argc, char *argv[]) { //kontrolujem argumenty stdin, vypisujem chybove stavy
	int j = 1;
	int i = 1;
	int k = 0;
	int s = 0;
	int p_u = 0; //pocet prikazov pre upravu
	int p_s = 0; //pocet prikazov pre spracovanie udajov a selekciu
	char* str;
	int n_sprac = 0;
	int n_sel = 0;	
	int pos_sel = 0;
	int pos_spr = 0;
	char* pole_uprav[8] = {"irow","arow","drow","drows","icol","acol","dcol","dcols"};
	char* pole_sprac[8] = {"cset","tolower","toupper","round","int","copy","swap","move"};
	char* pole_selekce[3] = {"rows","beginswith","contains"};
	
	if (argc > 2 && strcmp(argv[1], "-d") == 0) { //zistim pritomnost delimu
		j = 3;
	}

	if (kontrola_dlzky(argc, argv) != 0) {
		fprintf(stderr, "Invalid length of argument\n"); //chybova hlaska v pripade dlheho argumentu
		return 1;
	}


	for (i = 1; i < argc; i++) { //zistim pocet arg. uprav tabulky, sprac. dat a selekciu riadkov
		for (k = 0; k < 8; k++) {
			if (strcmp(argv[i], pole_uprav[k]) == 0) {
				p_u++;
			}

			else if (strcmp(argv[i], pole_sprac[k]) == 0) {
				p_s++;
			}
		}

		for (s = 0; s < 3; s++) {
			if (strcmp(argv[i], pole_selekce[s]) == 0) {
				p_s++;
			}
		}

		
	}

	

	if ((p_u != 0 && p_s > 0) || (p_u > 0 && p_s != 0)) {
		fprintf(stderr, "Not possible to combine editing and processing functions\n"); //v pripade kombinacie prikazov pre upravu tabulky a sprac. dat vypisat chybovu hlasku
		return 1;
	}

	
	///////////////////////////////////////////////////////////////////////
	//			Kontrola argumentov pre upravu tabulky					 //
	///////////////////////////////////////////////////////////////////////


	while (j < argc) {
		if (strcmp(argv[j], "irow") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after irow\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number \n", argv[j + 1]);
				return 1;
			}

			j += 2;
		}

		else if(strcmp(argv[j], "drow") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after drow\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}

			j += 2;
		}

		else if (strcmp(argv[j], "icol") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after icol\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}

			j += 2;
		}

		else if (strcmp(argv[j], "dcol") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after dcol\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}

			j += 2;
		}
		
		else if (strcmp(argv[j], "arow") == 0) {
			if (j + 1 < argc) {
				if (kontrola_append(argv[j + 1]) != 0) {
					fprintf(stderr, "Arow must not have parameter\n");
					return 1;
				}
			}	

			j += 1;
		}

		else if (strcmp(argv[j], "acol") == 0) {	
			if (j + 1 < argc) {
				if (kontrola_append(argv[j + 1]) != 0) {
					fprintf(stderr, "Acol must not have parameter\n");
					return 1;
				}				

			}
		
			j += 1;
		}

		else if (strcmp(argv[j], "drows") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after drows\n");
				return 1;
			}
			else if (argc <= j + 2) {
				fprintf(stderr, "You typed one drows's  argument instead of two\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}
			else if (kontrola_N(argv[j + 2]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 2]);
				return 1;
			}
			if (atoi(argv[j + 1]) > atoi(argv[j + 2])) {
				
				fprintf(stderr, "First value of drows cannot be bigger than second one: %s,%s\n", argv[j + 1], argv[j + 2]);
				return 1;
			}
	
			j += 3;
		}

		else if (strcmp(argv[j], "dcols") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after dcols\n");
				return 1;
			}
			else if (argc <= j + 2) {
				fprintf(stderr, "You typed just one dcols's argument instead of two\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}
			else if (kontrola_N(argv[j + 2]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 2]);
				return 1;
			}
			if (atoi(argv[j + 1]) > atoi(argv[j + 2])) {
				fprintf(stderr, "First value of dcols cannot be bigger than second one\n");
				return 1;
			}

			j += 3;
		}

		//////////////////////////////////////////////////////////////////////
		//		Kontrola argumentov na spracovanie dat					    //
		//////////////////////////////////////////////////////////////////////

		else if (strcmp(argv[j], "cset") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after cset\n");
				return 1;
			}
			else if (argc <= j + 2) {
				fprintf(stderr, "You typed just one cset's argument instead of two\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}

			str = argv[j + 2];
			pos_spr = j;
			j += 3;

			n_sprac++;		

		}

		else if (strcmp(argv[j], "tolower") == 0) {	
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after tolower\n");
				return 1;
			}

			if (kontrola_N(argv[j + 1]) != 0) {
            	fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
            }
			
			pos_spr = j;
			j += 2;
			n_sprac++;
		}

		else if (strcmp(argv[j], "toupper") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after toupper\n");
				return 1;
			}
			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}
			
			pos_spr = j;	
			j += 2;
			n_sprac++;
		}

		else if (strcmp(argv[j], "round") == 0) {
			if (argc <= j + 1) {
				fprintf(stderr, "No argument after round\n");
				return 1;
			}

			if (kontrola_N(argv[j + 1]) != 0) {
				fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
			}

			pos_spr = j;
			j += 2;
			n_sprac++;
		}

		else if (strcmp(argv[j], "int") == 0) {
        	if (argc <= j + 1) {
            	fprintf(stderr, "No argument after int\n");
				return 1;
            }

            if (kontrola_N(argv[j + 1]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
				return 1;
            }

			pos_spr = j;
            j += 2;
			n_sprac++;
        }

		
		else if (strcmp(argv[j], "copy") == 0) {
            if (argc <= j + 1) {
            	fprintf(stderr, "No argument after copy\n");
                return 1;
            }
            else if (argc <= j + 2) {
            	fprintf(stderr, "You typed just one copy's argument instead of two\n");
                return 1;
            }
            if (kontrola_N(argv[j + 1]) != 0) {
            	fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
                return 1;
            }

            if (kontrola_N(argv[j + 2]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 2]);
                return 1;
            }

			pos_spr = j;
            j += 3;
			n_sprac++;
         }

		
		else if (strcmp(argv[j], "swap") == 0) {
            if (argc <= j + 1) {
                fprintf(stderr, "No argument after swap\n");
                return 1;
            }
            else if (argc <= j + 2) {
                fprintf(stderr, "You typed just one swap's argument instead of two\n");
                return 1;
            }
            if (kontrola_N(argv[j + 1]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
                return 1;
            }

            if (kontrola_N(argv[j + 2]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 2]);
                return 1;
            }
	
			pos_spr = j;
            j += 3;
			n_sprac++;
      	}


		else if (strcmp(argv[j], "move") == 0) {
            if (argc <= j + 1) {
                fprintf(stderr, "No argument after move\n");
                return 1;
            }
            else if (argc <= j + 2) {
                fprintf(stderr, "You typed just one move's argument instead of two\n");
                return 1;
            }
            if (kontrola_N(argv[j + 1]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
                return 1;
            }

            if (kontrola_N(argv[j + 2]) != 0) {
            	fprintf(stderr, "%s is not natural number\n", argv[j + 2]);
                return 1;
            }

			pos_spr = j;
            j += 3;
			n_sprac++;	
         }

		/////////////////////////////////////////////////////////////////////
		//		Kontrola argumentov pre spracovanie riadkov				   //
		/////////////////////////////////////////////////////////////////////


		else if (strcmp(argv[j], "rows") == 0) {
        	if (argc <= j + 1) {
            	fprintf(stderr, "No argument after rows\n");
                return 1;
            }
            else if (argc <= j + 2) {
                fprintf(stderr, "You typed just one rows's argument instead of two\n");
                return 1;
            }

			
            
            if ((kontrola_N(argv[j + 1]) != 0) && (strcmp(argv[j + 1], "-") != 0)) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
                return 1;
            }

            if ((kontrola_N(argv[j + 2]) != 0) && (strcmp(argv[j + 2], "-") != 0)) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 2]);
                return 1;
            }

			if ((strcmp(argv[j + 1], "-") == 0) && (strcmp(argv[j + 2], "-") != 0)) {
				fprintf(stderr, "If first argument of rows is -, second argument must be -, too\n");
				return 1;

			}
			
			if ((atoi(argv[j + 1]) > atoi(argv[j + 2])) && (strcmp(argv[j + 1], "-") != 0) && (strcmp(argv[j + 2], "-") != 0)) {
                fprintf(stderr, "First value of rows cannot be bigger than second one\n");
                return 1;
            }


			pos_sel = j;
            j += 3;
            n_sel++;
         }

		else if (strcmp(argv[j], "beginswith") == 0) {
        	if (argc <= j + 1) {
            	fprintf(stderr, "No argument after beginswith\n");
                return 1;
            }
         
				else if (argc <= j + 2) {
                fprintf(stderr, "You typed just one beginswith's argument instead of two\n");
                return 1;
            }
            
            if (kontrola_N(argv[j + 1]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
                return 1;
            }

            
			pos_sel = j;
			str = argv[j + 2];	
            j += 3;
            n_sel++;
        }
	
		else if (strcmp(argv[j], "contains") == 0) {
            if (argc <= j + 1) {
                fprintf(stderr, "No argument after contains\n");
                return 1;
            }
            else if (argc <= j + 2) {
                fprintf(stderr, "You typed just one contain's argument instead of two\n");
                return 1;
            }

            if (kontrola_N(argv[j + 1]) != 0) {
                fprintf(stderr, "%s is not natural number\n", argv[j + 1]);
                return 1;
            }


			pos_sel = j;
			str = argv[j + 2];
            j += 3;
            n_sel++;
        }
		
		/////////////////////////////////////////////////////////////////////////////
		//         Kontrola validity a vyskytnosti argumentov v pripade sprac. dat //
		/////////////////////////////////////////////////////////////////////////////


		else if (kontrola_append(argv[j]) != 0 || strcmp(argv[j], str) != 0) {
			fprintf(stderr, "%s is not valid argument\n", argv[j]);
			return 1;
		}
		
		if ((n_sprac > 1) || (n_sel > 1)) {
            fprintf(stderr, "Not possible to make more than 1 command of processing and selective functions\n");
            return 1;
        }

		if ((pos_sel > pos_spr) && (n_sprac > 0)) {
			fprintf(stderr, "Selective function is located after processing function\n");
			return 1;
		}

	}
	

	return 0;

}



/*
 * Funkcia kontrola_N:
 * -----------------------
 * 	s: vstupny parameter, obsahuje hodnotu poctu riadku/stlpcu
 *
 * vracia hodnotu 1 v pripade ak cislo riadku (stlpcu) nie je N
 */


int kontrola_N(char* s) { //checking prirodzenych cisel (riadkov, stlpcov)

	if (s[0] == '0') {
		return 1;
	}
	for (int i = 0; s[i] != '\0'; i++) {
		if (s[i] < '0' ||  s[i] > '9') {
			return 1;
		}

	}

	return 0;

}


/*
 * Funkcia kontrola_append:
 * --------------------------
 *  s: vstupny parameter, oznacuje argument
 *
 *	Vracia 0 len ked je za argumentom arow/acol vhodny neciselny prikaz
 */


int kontrola_append(char* s) {	//povolene argumenty za arow/acol

	if (strcmp(s, "irow") == 0)
		return 0;
    if (strcmp(s, "arow") == 0)
		return 0;
    if (strcmp(s, "drow") == 0)
		return 0;
    if (strcmp(s, "drows") == 0)
		return 0;
    if (strcmp(s, "icol") == 0)
		return 0;
    if (strcmp(s, "acol") == 0)
		return 0;
    if (strcmp(s, "dcol") == 0)
		return 0;
    if (strcmp(s, "dcols") == 0)
		return 0;

	return 1;

}


/*
 * Funkcia kontrola_dlzky:
 * ------------------------
 *  p: pocet argumentov
 *	s: kontrolovany retazec
 *
 *	Vracia 1 v pripade ak 1 retazec alebo vsetky retazce presahuju povolenu dlzku
 */


int kontrola_dlzky(int p, char* s[]) { //kontrolujem povolenu dlzku argumentu, v pripade prekrocenia vypisujem chybovy stav

	int d = 0;
	for (int i = 1; i < p; i++) {
		if (strlen(s[i]) > 101) {
			return 1;
		}
		d += strlen(s[i]);
	}
	
	if (d > 10241) 
		return 1;

	return 0;

}


/*Funkcia vstup:
 *-----------------
 *	p: vstupny p., retazec delim
 *	ch: prvy znak retazca delim
 *	argc: pocet argumentov
 *	argv: "stringove" pole argumentov
 *
 *	Funkcia nam vracia konecny vystup, t.j. tabulku, vykonava
 *	vyvolava funkcie na zistenie argumentov a ich usporiadanie,
 *	nacitava postupne riadok po riadku, na zaklade aktualneho prikazu
 *	vykonava riadkove, stlpcove upr., pripadne spracovava data
 */


int vstup(char* p, char ch, int argc, char* argv[]) { 

	int zamena = 0;	//pomocna premenna pri irow
	char pomoc[10241][101];	// pomocne pole pri zamienani dvoch hodnot pri irow
	char zamen[10241][101];	// uchovava hodnotu dalsieho riadku vo funkcii irow
	int  b = 0;	// deklaracia pre pocet riadkov
	char arr[10241][101];	// uchovava retazec aktualneho riadku
	char* c_prikazy [10241];	// obsahuje prikazy pre upravu stlpcov zadanych uzivatelom
	char* r_prikazy[10241];		// 				-||-    upravu riadkov	 -||-
	bool koniec = false;	//	pravdivostna hodnota ci sa jedna o konecny znak
	int n_row = -1; //	deklaracia poctu riadkov
	int n_cols = b; //pomocna premenna pre pocet stlpcov
	int  m_r = 0, m_c = 0;	// deklaracia indexu pre a poctu prikazov riad. a stlpc. uprav
	char* sel[10241];	// deklaracia pola pre prikazy selekcie riadkov
	char* index_r[10241];	// cislo riadku pre riadkove upravy
	char* index_c[10241];	// cislo stlpcu pre stlpcove upravy
	char* prikazy[10241];	// deklaracia len pre upravy zadane uzivatelom bez parametrov
	int m;	// premenna uchovava pocet prikazov, ktore ma vykonat celkovo
	int a = 0; // pomocna premenna pre vetvenie (ci konat riadkove alebo stlpcove upravy)
	int vypisanie = 0; // pomocna premenna, v pripade hodnoty 0 nevypise dany riadok
	int arow = 0;	// v pripade hodnoty >0 vypise na konci tabulky dany pocet novych riadkov	
	int help = 0;	// pomocna premenna uchovavajuca hodnotu prveho parametru drows
	int p_opak = 0;	// pomocna premenna uchovavajuca pocet opakovani vyrazu irow
	int ind3 = 0;	// pomocna premenna pre irow 
	int vymaz_c = 0;	// pomocna premenna pre funkcie dcol a dcols
	int selekcia = 0;	// pomocna premenna pre vetvenie kvoli pripadnemu prikazu selekcie riadkov
	int s = 0;	// pomocna premenna pre selekciu riadkov	

	//VYVOLAVANIE FUNKCII NA ZISTENIE A TRIEDENIE VSTUPNYCH ARGUMENTOV
	zistim_argumenty(argc, argv, &prikazy, &m, &selekcia);
	triedenie_argumentov(argc, argv, &r_prikazy, &c_prikazy, &m_r, &m_c, &index_r, &index_c, &sel);	// zistim pole prikazov na upravu riadkov a pocet
	
	//vynulovanie si indexov
	m_r = 0;
	m_c = 0;
	
	dalsi_riadok(arr, p, &b, &koniec); //nacitanie prveho riadku a zistenie poctu stlpcov

	while (!koniec) {		
		n_row++;	//inkrementacia poctu riadkov
		m_c = 0;
		vypisanie = 1;	// nastavim povodnu hodnotu vypisovania
			

		if (n_row == 0) {
			n_cols = b;	// v pripade prveho riadku si zapamatam prvotny pocet stlpcov
		}		
	    
		if (b != n_cols) {
        	fprintf(stderr, "\nSome row has more or less values than it's set in first row:b-%i,ncols-%i\n", b, n_cols); //v pripade rozneho poctu stlpcov sa kona chybove hlasenie
            return 1;
        }
		
		b = b + vymaz_c;
		n_cols = n_cols + vymaz_c;	 
		vymaz_c = 0;		
		
		m_r = 0;
		
		if (selekcia == 1)	//ak je prikaz na selekciu, vykonaj ho a uloz vystupnu hodnotu
			s = sel_riadku(arr, sel, n_row, koniec, n_cols); //ziskam hodnotu 0 alebo 1

		if (selekcia == 0 || s == 1) {	//ak nie je zadany prikaz na selekciu alebo udaje o riadku vyhovuju prikazu a parametrom f.

			for (int j = 0; j < m; j++) { //az pokial sa nevykonaju vsetky prikazy
			
				vetvenie(&a, &prikazy, j); //zistim ci robit riadkove alebo stlpcove upravy

				if (a == 1) {	//uz mam nacitany riadok v arr
					r_upravy(&arr, p, &m_r, b, &n_row, &prikazy, j, index_r, &vypisanie, &pomoc, &zamen, &zamena, &arow, &help, &p_opak, &ind3); //konaju sa riadkove upravy
				}
		
				else if (a == 2) {
					c_upravy(&arr, &m_c, &b, &n_cols, &prikazy, j, index_c, &vymaz_c); // konaju sa stlpcove upravy alebo upravy spracovania dat
				}
			}	
		}

		if (vypisanie == 1) {
			vypis(arr, n_cols, ch); //riadok, cislo riadku, pocet stlpcov
			
		}
	
		vyprazdnenie_cyklu(arr, b);
        dalsi_riadok(arr, p, &b, &koniec);
		
		b = b - vymaz_c;

	}	
	
	if (zamena == 1) { //vypis posledneho riadku
		n_row++;
		vypis(zamen, n_cols, ch);
	}

	if (arow > 0) {	//vypis v pripade arow
		for (int j = 0; j < arow; j++) {
			n_row++;
			vyprazdnenie_cyklu(arr, b);
			vypis(arr, n_cols, ch);	
		}
	}

	if (n_row == -1)
	fprintf(stderr, "Input cannot be empty table\n");
	

	return 0;

}


/*Funkcia dalsi_riadok:
 *-----------------------
 *  *arr: vstupno-vystupny parameter ukazujuci na aktualny riadkovy retazec
 *	p: oddelovac
 *	*b: vstupno-vystupny parameter ukazujuci na pocet stlpcov nacitaneho riadku 
 *	*koniec: ukazuje na posledny znak vo funkcii vstup, ci je EOF
 *
 * 	Vystupom funkcie je aktualny retazec, pocet jeho stlpcov a pripadny konecny znak
 */

int dalsi_riadok(char (*arr)[101], char* p, int *b, bool *koniec) { // nacitanie si dalsieho riadku do pola
	
	for(int x = 0; x < 10241; x++){
		for(int y = 0; y < 101; y++) {
			arr[x][y] = '\0';
		}
	}
	
	char ch1[101];
	int i = 0;
    int j = 0;
    int m = 0;
    *b = 0;
    char c;
    c = getc(stdin);

    while(c != EOF && c != '\n') { //nacitavanie po posledne slovo riadku
    
    	if (strchr(p, c) != NULL && c != '\r') { //ignorujem carriage znak
    	    
			for (j = 0; j < i; j++) {
       		    arr[*b][j] = ch1[j];
    		}

 			i = 0;
    		(*b)++;
        }
		
		
    
		if (strchr(p, c) == NULL && c != '\r') {
        	if (ch1[i] != '\r') {
            	ch1[i] = c;
     		    i++;
       		}
       	}
    
        c = getc(stdin);
	}
    
    *koniec = (c == EOF); // urcenie si posledneho znaku a jeho nasledna kontrola vo funkcii vstup, ci je EOF
    
    for (m = 0; m < i; m++) {	//nacitanie slova za poslednym oddelovacom
    	if (ch1[m] != '\r') { //ignorujem znak carriage return kvoli vypisu
           arr[*b][m] = ch1[m];
       }
    }
    
 	return 0;
  
}


/* Funkcia vyprazdnenie_cyklu:
 * ---------------------------
 *  *arr: vstupno-vystupny parameter, po vykonani funkcie je prazdny
 *	b: pocet stlpcov aktualneho riadku
 *
 *	Vracia prazdny retazec
 */


int vyprazdnenie_cyklu(char (*arr)[101], int b) {
	for (int i = 0; i <= b; i++ ) {
		for (int j = 0; j < 101; j++) {
			arr[i][j] = '\0';			
		}
	}

	return 0;

}


/* Funkcia zistim_argumenty:
 *	argc: pocet argumtov
 *	argv: retazec argument
 *	*prikazy: ukazuje na prikazy ktore maju byt konane vo funkcii vstup
 *	*m: ukazuje na pocet prikazov na vykonanie
 *	*selekcia: ukazuje na hodnotu premennej vo vstupe, vieme ci mame alebo nemame konat selekciu riadkov
 *
 *	vracia cisto nazvy prikazov zadanych uzivatelom bez parametrov a ich pocet
 */


int zistim_argumenty(int argc, char* argv[], char* (*prikazy)[10241], int *m, int *selekcia) {

	//deklarujem si pomocne polia s retazcami, podla ktorych zistujem kde a ci vobec sa dany prikaz vyskytuje vo vstupe uzivatela
	int i, j, k, n, x, y;
    char* pole_r [4] = {"irow","drow","arow","drows"};
    char* pole_c [4] = {"icol","dcol","acol","dcols"};
    char* pole_sel [3] = {"rows","beginswith","contains"};
    char* pole_spr [8] = {"cset","tolower","toupper","round","int","copy","swap","move"};


	//PRE RIADKOVE UPRAVY A STLPCOVE UPRAVY
    for (i = 0; i < argc; i++) {
        for (j = 0; j < 4; j++ ) {
            if (strcmp(argv[i], pole_r[j]) == 0) {
                (*prikazy)[*m] = pole_r[j];
				(*m)++;
            }

            else if (strcmp(argv[i], pole_c[j]) == 0) {
                (*prikazy)[*m] = pole_c[j];
				(*m)++;
			}
            
			
            
        }
    }

	//PRE SELEKCIU RIADKOV
    for (k = 0; k < argc; k++){
        for (n = 0; n < 3; n++) {
            if (strcmp(argv[k], pole_sel[n]) == 0) {
                (*prikazy)[*m] = pole_sel[n];
                (*m)++;
				*selekcia = 1;
            }

        }

    }

	//PRE SPRACOVANIE DAT
    for (x = 0; x < argc; x++) {
        for (y = 0; y < 8; y++) {
            if (strcmp(argv[x], pole_spr[y]) == 0) {
				(*prikazy)[*m] = pole_spr[y];
				(*m)++;
			}
        }

    }




	return 0;
}


/* Funkcia triedenie_argumentov
 * -----------------------------
 *	argc: pocet argumentov
 *	argv: retazce argumentov
 *	*r_prikazy: ukazuje na prikazy riadkovych uprav
 *	*c_prikazy: - || -			   stlpcovych uprav
 *	*m_r: ukazuje na index riadkovych uprav
 *	*m_c: - || -		   stlpcovych uprav
 *	*index_r: obsahuje hodnotu parametra riadkovych uprav
 *	*index_c: - || -					 stlpcovych uprav a spracovania dat
 *	*sel: ukazuje na prikaz selekciu riadkov a jeho parametre
 *
 *	Vracia parametre riadkovych, stlpcovych uprav, spracovania dat a selekciu riadkov
 */


int triedenie_argumentov(int argc, char* argv[], char* (*r_prikazy)[10241], char* (*c_prikazy)[10241], int *m_r, int *m_c, char* (*index_r)[10241], char* (*index_c)[10241], char* (*sel)[10241]) {

	char* pole_r [4] = {"irow","drow","arow","drows"};
	char* pole_c [4] = {"icol","dcol","acol","dcols"};
	char* pole_sel [3] = {"rows","beginswith","contains"};
	char* pole_spr [8] = {"cset","tolower","toupper","round","int","copy","swap","move"};  

	int i, j, k, n;

	//Spracujem pripadne udaje pre selekciu riadkov a spracovanie dat
	
	for (n = 1; n < argc; n++) {
		
		if (strcmp(argv[n], pole_sel[0]) == 0) {
			(*sel)[0] = argv[n];
			(*sel)[1] = argv[n + 1];
			(*sel)[2] = argv[n + 2];

		}

		else if (strcmp(argv[n], pole_sel[1]) == 0) {
			(*sel)[0] = argv[n];
			(*sel)[1] = argv[n + 1];
			(*sel)[2] = argv[n + 2];
		}

		else if (strcmp(argv[n], pole_sel[2]) == 0) {
			(*sel)[0] = argv[n];
			(*sel)[1] = argv[n + 1];
			(*sel)[2] = argv[n + 2];
		}

	}

	

	for (k = 1; k < argc; k++) {
		

		if (strcmp(argv[k], pole_spr[0]) == 0) {
			(*c_prikazy)[*m_c] = argv[k];
			(*index_c)[*m_c] = argv [k + 1];
			(*m_c)++;
			(*index_c)[*m_c] = argv [k + 2];
			(*m_c)++;
		}

		if (strcmp(argv[k], pole_spr[1]) == 0) {
        	(*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
            (*m_c)++;
        }

		if (strcmp(argv[k], pole_spr[2]) == 0) {
        	(*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
            (*m_c)++;
        }

		if (strcmp(argv[k], pole_spr[3]) == 0) {
	        (*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
            (*m_c)++;
        }

		if (strcmp(argv[k], pole_spr[4]) == 0) {
        	(*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
            (*m_c)++;
        }		

		if (strcmp(argv[k], pole_spr[5]) == 0) {
            (*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
		    (*m_c)++;
            (*index_c)[*m_c] = argv [k + 2];
			(*m_c)++;
        }

		if (strcmp(argv[k], pole_spr[6]) == 0) {
            (*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
			(*m_c)++;
			(*index_c)[*m_c] = argv [k + 2];
            (*m_c)++;
            }

		if (strcmp(argv[k], pole_spr[7]) == 0) {
            (*c_prikazy)[*m_c] = argv[k];
            (*index_c)[*m_c] = argv [k + 1];
			(*m_c)++;
			(*index_c)[*m_c] = argv [k + 2];
            (*m_c)++;
        }
			
		
	}

	// Udaje pre spracovanie riadkovych a stlpcovych uprav

	for (i = 1; i < argc; i++) {

		for (j = 0; j < 4; j++) {

			if (strcmp(argv[i], pole_r[j]) == 0) {

				if (strcmp(argv[i], pole_r[3]) == 0) {
					(*r_prikazy)[*m_r] = argv[i];
					(*index_r)[*m_r] = argv [i + 1];
					(*m_r)++;
					(*index_r)[*m_r] = argv [i + 2];
					(*m_r)++;					
				}				

				else if (strcmp(argv[i], pole_r[2]) == 0) {
					(*r_prikazy)[*m_r] = argv[i];
					(*m_r)++;
				}

				else if (strcmp(argv[i], pole_r[0]) == 0 || strcmp(argv[i], pole_r[1]) == 0) {
               		(*r_prikazy)[*m_r] = argv[i];
                    (*index_r)[*m_r] = argv [i + 1];
                    (*m_r)++;
					
				}
			}

			if (strcmp(argv[i], pole_c[j]) == 0) {

				if (strcmp(argv[i], pole_c[3]) == 0) {
                	(*c_prikazy)[*m_c] = argv[i];
                    (*index_c)[*m_c] = argv [i + 1];
                    (*m_c)++;
                    (*index_c)[*m_c] = argv [i + 2];
                    (*m_c)++;
                }

                else if (strcmp(argv[i], pole_c[2]) == 0) {
                	(*c_prikazy)[*m_c] = argv[i];
    	            (*m_c)++;
				}

                else if (strcmp(argv[i], pole_c[0]) == 0 || strcmp(argv[i], pole_c[1]) == 0) {
                	(*c_prikazy)[*m_c] = argv[i];
                    (*index_c)[*m_c] = argv [i + 1];
                    (*m_c)++;
				}
			}
		}

	}

	
		

	return 0;
}


/* Funkcia vetvenie:
 *-------------------
 *	*a: ukazovatel na hodnotu, ci vykonat riadkove alebo stlpcove upravy
 *	*prikazy: ukazovatel na prikazy
 *  j: index aktualnej upravy
 *
 * 	Funkcia vracia hodnotu a, podla kt. sa robia bud riadkove alebo stplcove upravy (spracovanie dat)
 */


int vetvenie(int *a, char* (*prikazy)[10241], int j) {
	char* pole_c[4] = {"icol","dcol","acol","dcols"};
	char* pole_r[4] = {"irow","drow","arow","drows"};
	char* pole_spr [8] = {"cset","tolower","toupper","round","int","copy","swap","move"};	

	for (int i = 0; i < 4; i++) {
		if (strcmp((*prikazy)[j], pole_r[i]) == 0)
			*a = 1;
		
		else if (strcmp((*prikazy)[j], pole_c[i]) == 0) 
			*a = 2;

	}

	for (int k = 0; k < 8; k++) {
		if (strcmp((*prikazy)[j], pole_spr[k]) == 0)
			*a = 2;	
	}
		
	return 0;
}


/* Funkcia vypis:
 *-----------------
 *	arr: charovy 2D pole obsahujuce aktualny riadok
 *	n_cols: aktualny pocet stlpcov tabulky
 *	ch: oddelovac
 *
 *	Funkcia vypisuje aktualny riadok
 */

					
int vypis (char arr[10241][101], int n_cols, char ch) {

	for (int k = 0; k <= n_cols; k++) {  //index slova a pocet slov v riadku
    	
		if (k == n_cols) {
	      	printf(("%s\n"), arr[k]);
        }
        else {
        	printf("%s%c", arr[k], ch);
        }
		
         
	}

	return 0;
}		


/* Funkcia r_upravy:
 *--------------------
 *	*arr: ukazuje na retazec aktualneho riadku
 *	p: retazec delim pouzivany pri irow kvoli vypisovaniu
 *	*m_r: ukazuje na idex aktualne spracovavaneho riadku
 *	b: aktualny pocet stlpcov
 *	*n_row: ukazovatel na cislo aktualneho riadku
 *	*prikazy: ukazovatel na aktualny prikaz
 *	j: index aktualneho prikazu
 *	index_r: parametre aktualneho prikazu
 *	*vypisanie: v pripade dcol a dcols urci hodnotu (ne)vypisania
 *	*pomoc: pomocna premenna pre uchovanie retazcu aktualneho riadku
 *	*zamen: pomocna premenna pre uchovanie retazcu predchadzajuceho riadku
 *	*zamena: premenna pre irow kvoli vypisu
 *	*arow: ukazatel na hodnotu, kolko novych riadkov sa ma na konci pripisat
 *	*help: pomocny ukazatel pri drows na zachovanie si prveho parametra drows
 *	*p_opak: pocet opakovani funkcie irow
 *	*ind3: ukazatel na parameter irow kvoli moznemu viacnasobnemu vyskytu irow
 *
 *	Vracia finalne upraveny riadok podla aktualneho riadkoveho prikazu, pripadne aj nove cislo riadku
 */


int r_upravy(char (*arr)[10241][101], char* p, int *m_r  , int b, int *n_row, char* (*prikazy)[10241], int j, char* index_r[10241], int *vypisanie, char (*pomoc)[10241][101], char (*zamen)[10241][101], int *zamena, int *arow, int *help, int *p_opak, int *ind3) {

		
	int ind = 0;
	int ind2 = 0;	

	// DROW------------------------------------------

	if (strcmp((*prikazy)[j], "drow") == 0) {
		ind = atoi(index_r[*m_r]) - 1;	

		if (ind == *n_row) {
			(*prikazy)[j] = "\0";
			*vypisanie = 0;
			(*n_row)--;
			(*m_r)++;
		}
						
	}

	// DROWS---------------------------------------------------------

	if (strcmp ((*prikazy)[j], "drows") == 0) {
		ind = atoi(index_r[*m_r]) - 1;
		ind2 = atoi(index_r[*m_r + 1]) - 1;
			 		
		if (*n_row >= ind && *n_row <= ind2) {
			*vypisanie = -1;
		}
		
		if (*n_row == ind) {
			*help = *n_row;	//zachovam si prvy parameter drows
		}

		else if (*n_row == ind2 + 1) {
			*n_row = *help;
			*vypisanie = 1;
			(*prikazy)[j] = "\0";
			(*m_r) += 2;
		}

	}

	// AROW----------------------------------------------------------
	
	if (strcmp((*prikazy)[j], "arow") == 0) {
		(*prikazy)[j] = "\0";
		(*arow)++;
		(*m_r)++;	
	}	

	// IROW----------------------------------------------------------
        
	if (strcmp((*prikazy)[j], "irow") == 0) {
		int i,k;
	
		if (*p_opak == 0) {
			*ind3 = atoi(index_r[*m_r]) - 1; 
			(*m_r)++; 
			(*p_opak)++;
		}		

		if (*ind3 == *n_row) {
			*zamena = 1;			
			*vypisanie = 0; 

			for (i = 0; i <= b; i++) {

				for (k = 0; k < 101; k++) {
            		(*pomoc)[i][k] = (*arr)[i][k];				
				}				
			}
				  
			vyprazdnenie_cyklu(*arr, b);

			for (i = 0; i <= b; i++){

				for (k = 0; k < 101; k++) {
					(*zamen)[i][k] = (*pomoc)[i][k];
				}		
			}

			vypis(*arr, b, p[0]);	

			return 0;
		}


		if (*zamena == 1) {
			vyprazdnenie_cyklu(*pomoc, b);

			for (i = 0; i <= b; i++) {

				for (k = 0; k < 101; k++) {
            		(*pomoc)[i][k] = (*arr)[i][k]; //predavam hodnotu pola do pomocneho pola
				}
			}

			vyprazdnenie_cyklu(*arr, b);

			for (i = 0; i <= b; i++) {

         		for (k = 0; k < 101; k++) {					
					(*arr)[i][k] = (*zamen)[i][k];
				}	
			}	
		
			vyprazdnenie_cyklu(*zamen, b);

			for (i = 0; i <= b; i++) {

				for (k = 0; k < 101; k++) {
         			(*zamen)[i][k] = (*pomoc)[i][k];
				}
			}

		}

		if (*p_opak == *ind3) {
			*p_opak = 0;
		}

				
	}


	return 0;
}


/* Funkcia c_upravy:
 *------------------
 *	*arr: ukazuje na retazec aktualneho riadku
 *	*m_c: ukazuje na index prikazu
 *	*b: ukazuje na pocet stlpcov
 *	*n_cols: ukazuje na pocet aktualnych stlpcov
 *	*prikazy: ukazuje na aktualne konany prikaz
 *	j: index aktualneho prikazu
 *	index_c: parameter aktualneho prikazu zadany uzivatelom
 *	*vymaz_c: ukazuje kolko stlpcov ma vymazat
 *
 *	Funkcia vracia upraveny riadok podla aktualnej stlpcovej upravy (upravy spracovania dat) a takisto kolko stlpcov ma pripadne vymazat
 */


int c_upravy(char (*arr)[10241][101], int *m_c, int *b, int *n_cols, char* (*prikazy)[10241], int j, char* index_c[10241], int *vymaz_c) {


	int ind = 0;
	int ind2 = 0;
	int i, k;
	int spinac = 0;

	//DCOL----------------------------------------
	if (strcmp((*prikazy)[j], "dcol") == 0) {
		
		ind = atoi(index_c[*m_c]) - 1;
		
		if (ind > *n_cols) {	//v pripade ze parameter je vacsi ako pocet stlpcov ignorujem prikaz
			(*m_c)++;
			return 0;
		}

		for (i = 0; i <= *b; i++) {

        	for (k = 0; k < 101; k++) {

				if (spinac == 1 && i != *b) {
					(*arr)[i][k] = '\0';
					(*arr)[i][k] = (*arr)[i + 1][k];
				}					
				
				if (i == ind && i != *b) {	
             		(*arr)[i][k] = '\0'; 
					(*arr)[i][k] = (*arr)[i + 1][k];
					spinac = 1;
				}
			
				if (ind == *b && i == *b) {
					(*arr)[i][k] = '\0';
				}			
			}	
       }
	
			(*n_cols)--;			
			(*vymaz_c)++;      		
         	(*m_c)++;        
    }

	//DCOLS---------------------------------------
	if (strcmp((*prikazy)[j], "dcols") == 0) {
		ind = atoi(index_c[*m_c]) - 1;
		ind2 = atoi(index_c[*m_c + 1]) - 1;

		if (ind <= *n_cols) {   //v pripade ze parameter je vacsi ako pocet stlpcov ignorujem prikaz
            if (ind2 > *n_cols)
				ind2 = *n_cols;
		}
		

		else if (ind > *n_cols) {
			(*m_c)++;
			(*m_c)++;
            return 0;
        }

		for (i = 0; i <= *b; i++) {
			
			for (k = 0; k < 101; k++) {
				if (ind2 == *b) {

					if (i >= ind && i <= ind2) {
						(*arr)[i][k] = '\0';
						spinac = 1;
					}
				}

				else if (ind == 0) {
					
					if (i >= ind && i <= ind2) {
						(*arr)[i][k] = '\0';
						spinac = 1;
					}

					else if (i < ind || i > ind2) {
						(*arr)[i - (ind2 + 1) ][k] = (*arr)[i][k];
					}
				}

				else if ( ind != 0 && ind2 != *b) {
					
					if (i >= ind && i <= ind2) {
						(*arr)[i][k] = '\0';
					}

					else if (i < ind || i > ind2) {
						(*arr)[i - (ind2)][k] = (*arr)[i][k];
					}
				}
			}	
				
		
			if (i >= ind && i <= ind2) {
				(*n_cols)--;
				(*vymaz_c)++;	
			}

		}
        
	
		(*m_c)++;
		(*m_c)++;	
	}

	//ACOL---------------------------------------------------

	if (strcmp((*prikazy)[j], "acol") == 0) {
		(*n_cols)++;
		(*vymaz_c)--;
	
		for (int k = 0; k < 101; k++) {
			(*arr)[*n_cols][k] = '\0';
		}

		(*m_c)++;
	}

	//ICOL--------------------------------------------------

	if (strcmp((*prikazy)[j], "icol") == 0) {
		char zamen[10241][101];
		char pomoc[10241][101];
		int a = 0;
		
		ind = atoi(index_c[*m_c]) - 1;

		if (ind > *n_cols) {   //v pripade ze parameter je vacsi ako pocet stlpcov ignorujem prikaz
            (*m_c)++;
            return 0;
        }

		vyprazdnenie_cyklu(pomoc, *n_cols);
		(*n_cols)++;
		
		for (i = 0; i <= *n_cols; i++) {

			if (ind == i) {
				a = 1;
			}
				
			if (a == 1) {

				for (int k = 0; k < 101; k++) {
					zamen[i][k] = (*arr)[i][k];
					(*arr)[i][k] = '\0';
					(*arr)[i][k] = pomoc[i][k]; 
					pomoc[i + 1][k] = zamen[i][k];
				}
			}	
		}

 		(*vymaz_c)--;
        (*m_c)++;
	}

	//CSET-------------------------------------------------------
	
	if (strcmp((*prikazy)[j], "cset") == 0) {
		char* retazec = index_c[*m_c + 1];

		ind = atoi(index_c[*m_c]) - 1;

		
		
		for (i = 0; i <= *b; i++) {

        	for (k = 0; k < 101; k++) {

            	if (i == ind) {
                	(*arr)[i][k] = '\0';
           			(*arr)[i][k] = retazec[k];
                }
            }
        }				
	}

	//TOLOWER-----------------------------------------------------

	if (strcmp((*prikazy)[j], "tolower") == 0) {

		ind = atoi(index_c[*m_c]) - 1;

		for (i = 0; i <= *b; i++) {

            for (k = 0; k < 101; k++) {

                if (i == ind) {

					if ((*arr)[i][k] > 64 && (*arr)[i][k] < 91) {
                    	(*arr)[i][k] = (*arr)[i][k] + 32;
					}
                }

            }

        }		

	}

	//TOUPPER----------------------------------------------------

	if (strcmp((*prikazy)[j], "toupper") == 0) {

    	ind = atoi(index_c[*m_c]) - 1;    

        for (i = 0; i <= *b; i++) {

        	for (k = 0; k < 101; k++) {

                if (i == ind) {

                    if ((*arr)[i][k] > 96 && (*arr)[i][k] < 123) {
                        (*arr)[i][k] = (*arr)[i][k] - 32;
                    }
                }
            }
        }
    }

	//ROUND A INT--------------------------------------------------------------------

	if (strcmp((*prikazy)[j], "round") == 0 || strcmp((*prikazy)[j], "int") == 0) {
		int x = 1;
		int p_badchar = 0;
		int p_bod = 0;
		int pos_bod = 0;
		int p_minus = 0;
		int pos_minus = 0;
		int pos_fnum = 0;
		int p_cis = 0;
		ind = atoi(index_c[*m_c]) - 1;

		//zistujem, ci je dany retazec korektne zadane cele cislo

			for (k = 0; k < 101; k++) {
				
				if ( ((*arr)[ind][k] < 48 || (*arr)[ind][k] > 57) && (*arr)[ind][k] != '\0' && (*arr)[ind][k] != '.' && (*arr)[ind][k] != '-')
					p_badchar++;
											
				if ((*arr)[ind][k] == '.') { //kontrolujem pocet a poziciu bodky
					p_bod++;
					pos_bod = k;
				
					if (pos_bod == 0)
                       	p_badchar++;

					if ( ((*arr)[ind][k + 1] < 48 || (*arr)[ind][k + 1] > 57) || ((*arr)[ind][k - 1] < 48 || (*arr)[ind][k - 1] > 57)   )
						p_badchar++;			
				}

				if ((*arr)[ind][k] == '-') { //kontrolujem pripadny pocet a poziciu znaku minusu
					p_minus++;
					pos_minus = k;	
	
					if (pos_minus != 0)
						p_badchar++;	
				}

                if ((*arr)[ind][k] > 47 && (*arr)[ind][k] < 58) {
						
					if (x == 1) {
						pos_fnum = k; //pozicia prveho cisla
						if (pos_fnum > 1)
							p_badchar++;
					}

                    x = 0;

					if ((*arr)[ind][k] == '0') {
							
						if (k == 0) {
							if ((*arr)[ind][k + 1] > 47 && (*arr)[ind][k + 1] < 58)
								p_badchar++;
						}
					
						if (k == 1 && (*arr)[ind][k - 1] == '-') {
                            if ((*arr)[ind][k + 1] > 47 && (*arr)[ind][k + 1] < 58)
                                p_badchar++;
                        }


					}

					p_cis++;
				}	   
                

			}

		
			if (p_cis > 1 && p_bod == 1 && p_badchar == 0 && p_minus <= 1) {

				for (k = 0; k < 101; k++) {
					
					if (k == pos_bod && strcmp((*prikazy)[j], "round") == 0) {
						
						if ((*arr)[ind][k + 1] > '4') {
							((*arr)[ind][k - 1])++;
						}
					}

					if (k >= pos_bod) {
						(*arr)[ind][k] = '\0';
					}
				}
			} 
	}

	// COPY---------------------------------------------------------------------------------

	if (strcmp((*prikazy)[j], "copy") == 0) {
		ind = atoi(index_c[*m_c]) - 1;
		ind2 = atoi(index_c[*m_c + 1]) - 1;
		
		if (ind > *n_cols)
			return 0;
	
		else if (ind2 > *n_cols)
			return 0;


		for (k = 0; k < 101; k++) {
		
			(*arr)[ind2][k] = (*arr)[ind][k];	
		}
	}

	// SWAP---------------------------------------------------------------------------------
	
	if (strcmp((*prikazy)[j], "swap") == 0) {
		char c;

		ind = atoi(index_c[*m_c]) - 1;
		ind2 = atoi(index_c[*m_c + 1]) - 1;

		if (ind > *n_cols)
            return 0;

        else if (ind2 > *n_cols)
            return 0;
		

		for (k = 0; k < 101; k++) {
			c = (*arr)[ind2][k];
            (*arr)[ind2][k] = (*arr)[ind][k];
			(*arr)[ind][k] = c;
        }		
	}

	// MOVE----------------------------------------------------------------------------------

	if (strcmp((*prikazy)[j], "move") == 0) {
		char c;

		ind = atoi(index_c[*m_c]) - 1; //N- ten sa presuva
		ind2 = atoi(index_c[*m_c + 1]) - 1; //M

		if (ind > *n_cols)
            return 0;

        else if (ind2 > *n_cols)
            return 0;
		
		
		if (ind < ind2 && ind + 1 != ind2) {
        	
			for (i = ind; i < ind2 - 1; i++) {
				
				for (k = 0; k < 101; k++) {		
						c = (*arr)[i][k];
            			(*arr)[i][k] = (*arr)[i + 1][k];
            			(*arr)[i + 1][k] = c;
				}
			}
		}

		if (ind > ind2) {

			for (i = ind; i > ind2; i--) {
			
				for (k = 0; k < 101; k++) {
					c = (*arr)[i][k];
					(*arr)[i][k] = (*arr)[i - 1][k];
					(*arr)[i - 1][k] = c;
				}						
			}
		}   

	}   

	return 0;
}


/* Funkcia sel_riadku: sel_riadku
 * -------------------------------
 *	arr: vstupny parameter obsahujuci retazec aktualneho riadku  
 *	sel: vstupny parameter, obsahuje prikaz selekcie riadku a jeho parametre
 *	n_row: vstupny parameter s cislom aktualneho riadku
 *	koniec: vstupny parameter obsahujuci posledny znak EOF
 *	n_cols: vstupny parameter s cislom poctu stlpcov tabulky
 *
 *	Vracia hodnotu 1, ak sa ma na danom riadku vykonat spracovanie dat
 */


int sel_riadku(char arr[10241][101], char* sel[10241], int n_row, bool koniec, int n_cols) {
	int x = 0;
	int y = 0;	

	// ROWS----------------------------------------------------------------
	if (strcmp(sel[0], "rows") == 0) {
		
		if (strcmp(sel[1], "-") != 0 && strcmp(sel[2], "-") != 0) {
			x = atoi(sel[1]) - 1;
			y = atoi(sel[2]) - 1;
			
			if (n_row >= x && n_row <= y) {
				return 1;
			}
		}

		else if (strcmp(sel[1], "-") != 0 && strcmp(sel[2], "-") == 0) {
			x = atoi(sel[1]) - 1;

			if (n_row >= x) {
                return 1;
            }

		}

		else if (strcmp(sel[1], "-") == 0 && strcmp(sel[2], "-") == 0) {
			
			if (koniec == true) {
		
				return 1;		
			
			}
		}	
	}

	
	// BEGINSWITH------------------------------------------------------------
	if (strcmp(sel[0], "beginswith") == 0) {

		x = atoi(sel[1]) - 1;
		char* retazec = sel[2];
		int l = strlen(retazec);

		if (x > n_cols)
			return 0; 
		
		for (int k = 0; k < l; k++) {
			if (arr[x][k] != retazec[k])
				return 0;
		}
		

		return 1;

	}

	//	CONTAINS-------------------------------------------------------------
	if (strcmp(sel[0], "contains") == 0) {

		x = atoi(sel[1]) - 1;
		char* retazec = sel[2];
		int l = strlen(retazec);
		int zhodnost = 0;

		if (x > n_cols)
			return 0;

		for (int k = 0; k < 101; k++) {

			if (arr[x][k] == retazec[0]) {

				for (int m = 0; m < l; m++) {
					if (arr[x][k + m] == retazec[m])
						zhodnost++;
										
					
				}
			}

			if (zhodnost == l) {
				return 1;
			}
		
			zhodnost = 0;

		}

		return 0;		
	}	


	return 0;

}


