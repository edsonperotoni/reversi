#include<stdio.h>
#include<conio.h>
#include<dos.h>
#include<stdlib.h>
#include<limits.h>
#define Infinito INT_MAX
#define Vazio ' '
typedef enum { JogadorUm, JogadorDois, NenhumJogador }JOGADOR ;
typedef struct lm{
	 int row;
	 int col;
	 struct lm* next;
	 }Lista_Mov;

int top,nivel;

char nivel_j[][19] = {
		{"moleza!"},
		{"iniciante"},
		{"tem que pensar..."},
		{"complicou!"},
		{"experiente"},
		{"mestre!"},
		};

char Tabuleiro_atual[8][8];
char Tabuleiro_anterior[8][8];
int VlrHeur[8][8];


int continua_jogo(JOGADOR);
int avalia_tabuleiro(JOGADOR,char);
Lista_Mov *gera_movimentos(JOGADOR);
void movimenta(int, JOGADOR);
int movimento_valido(int,int,JOGADOR);
int verifica_coord(int,int);
void inicializa_tabuleiro(void);
void mostra_tabuleiro(char,char);
void mostra_pecas(char,char,char,int,int);
void mostra_marcas(char);
void apaga_marcas(JOGADOR);
Lista_Mov * gera_trocas(int,int,JOGADOR);
int existe_trocas(int,int,JOGADOR);
JOGADOR QuemVenceu(char,int*);
JOGADOR outro_jogador(JOGADOR);
void  som(int c);
void music(void);
int minimax(char computer,JOGADOR pl, int prof, int *solucao, int alfa,int beta);
char outro_char(char);

/******************************************************************/
void som(int c)
{
 sound(c);
 delay(150);
 nosound();
}
/******************************************************************/
void music(void)
{
      delay(50);    sound(550); delay(150); nosound();
      delay(20);    sound(250); delay(150); nosound();
      delay(20);    sound(250); delay(150); nosound();
      delay(10);    sound(500); delay(150); nosound();
      delay(20);    sound(250); delay(150); nosound();
      delay(50);    sound(550); delay(150); nosound();
      delay(50);    sound(750); delay(150); nosound();
}

/******************************************************************/
int  avalia_tabuleiro(JOGADOR pl,char computer)
{
    int val=0,diferenca,row,col;
    char player,_other;

    player = (pl == JogadorUm) ? 'X' : 'O';
    _other  = outro_char(player);

    if(!continua_jogo(pl) && !continua_jogo(outro_jogador(pl)) && QuemVenceu(computer,&diferenca) == outro_jogador(pl))
	return -(Infinito-100);  /* adversario vence */

    if(!continua_jogo(pl) && !continua_jogo(outro_jogador(pl)) && QuemVenceu(computer,&diferenca) == pl)
       return (Infinito-100);  /* adversario perde */

    if(!continua_jogo(pl) && continua_jogo(outro_jogador(pl)))
       return -(Infinito-100);  /* jogada faz pl passar a vez */

    if(continua_jogo(pl) && !continua_jogo(outro_jogador(pl)))
       return (Infinito-100);  /* jogada faz adversario passar a vez */

    for (row = 0; row <= 7; row++)
     for ( col = 0; col <= 7; col++)
      {
	if( Tabuleiro_atual[row][col] == player ){
	   val += VlrHeur[row][col];
	 }
	 else
	 if( Tabuleiro_atual[row][col] == _other )
	    val -= VlrHeur[row][col];
      }

    return(val);
}
/******************************************************************/
Lista_Mov *ordena(Lista_Mov* inicio){

Lista_Mov *mov_possiveis = inicio, *aux = inicio;

	   while(mov_possiveis!=NULL){
	       if( (mov_possiveis->row == 0 && mov_possiveis->col==7)
		  ||(mov_possiveis->row == 7 && mov_possiveis->col==0)
		  ||(mov_possiveis->row == 7 && mov_possiveis->col==7))
		  {
		      if( mov_possiveis != inicio ){
		       aux->next = mov_possiveis->next;
		       mov_possiveis->next = inicio;
		       inicio = mov_possiveis;
		       mov_possiveis = aux->next;
			}
			else
			{
			aux= mov_possiveis;
			mov_possiveis = mov_possiveis->next;
			}
		  }
		  else
		  {
		  aux= mov_possiveis;
		  mov_possiveis = mov_possiveis->next;
		  }
	    }
return(inicio);
}

/******************************************************************/
int minimax(char computer,JOGADOR pl, int depth, int *solucao, int alpha, int beta)
{
    Lista_Mov *mov_possiveis;
    int bestmove, child,i,j;
    char pilha[8][8];
    int newval;

    if (!depth)
       return(avalia_tabuleiro(pl,computer));

    mov_possiveis = gera_movimentos(pl);
    mov_possiveis = ordena(mov_possiveis);

    if (mov_possiveis==NULL)
      return(avalia_tabuleiro(pl,computer));

    while(mov_possiveis!=NULL)
    {
	Lista_Mov* laux;

	child = 8*mov_possiveis->row+mov_possiveis->col;
	if(child< 0 || child> 63)
	    getch();
	laux=mov_possiveis;
	mov_possiveis = mov_possiveis->next;
	laux->next=NULL;
	free(laux);

	// guarda tabuleiro no topo da pilha e aloca nova entrada na pilha //
       for ( i = 0; i <=7; i++)
	 for (j = 0; j <=7; j++)
	  pilha[i][j] = Tabuleiro_atual[i][j];

	movimenta(child,pl);
	newval = -minimax(computer,outro_jogador(pl), depth - 1, solucao, -beta, -alpha);

       // desfaz o movimento
       for (i = 0; i <=7; i++)
	 for (j = 0; j <=7; j++)
	   Tabuleiro_atual[i][j] = pilha[i][j];

	if (newval > alpha)
	{
	    alpha = newval;
	    bestmove = child;
	}
       if (alpha >= beta)
	{
	   bestmove = child;
	   while(mov_possiveis!=NULL){
	      laux= mov_possiveis;
	      mov_possiveis = mov_possiveis->next;
	      laux->next = NULL;
	      free(laux);
	    }
       }
    }
    *solucao = bestmove;
    return(alpha);
}

/* Verifica se existe ao menos um movimento valido para o jogador*/
/******************************************************************/
int continua_jogo(JOGADOR pl)
{
    int row,col,continua=0;

    if( pl == JogadorUm )
      for ( row = 0; row <=7; row++)
	for (col = 0; col <=7; col++)
	  if(movimento_valido(row,col,JogadorUm)){
	     continua = 1;
	     row = 10; /*escape*/
	     col = 10; /*escape*/
	  }
    if( pl == JogadorDois )
     for(row = 0; row <=7; row++)
       for (col = 0; col <=7; col++)
	  if(movimento_valido(row,col,JogadorDois)){
	     continua = 1;
	     row = 10; /*escape*/
	     col = 10; /*escape*/
	  }

  return(continua);
}

/******************************************************************/
int  movimento_valido(int row, int col, JOGADOR pl)
{
     if (row>=0 && row<=7 && col>=0 && col<=7
	&& Tabuleiro_atual[row][col] == Vazio
	&& existe_trocas(row,col,pl) )
	 return(1);
 return(0);
}

/******************************************************************/
int  existe_trocas(int row, int col, JOGADOR pl)
{
char player = (pl == JogadorUm) ? 'X' : 'O';
char opp= outro_char(player);
int existe=0,aux=0,coord,newcol,newrow,colinc,rowinc;

      for (colinc = -1; colinc <=1; colinc++)
	for (rowinc = -1; rowinc <=1; rowinc++)
	  {
	     if ( !(colinc==0&&rowinc==0) )
	     {
	       newrow = row + rowinc;
	       newcol = col + colinc;
	       coord = ( newrow>=0 && newrow<=7 && newcol>=0 && newcol<=7 ) ? 1:0 ;
	       while (coord && Tabuleiro_atual[newrow][newcol]==opp)
	       {
		 aux+=1;
		 newrow += rowinc;
		 newcol += colinc;
		 coord = ( newrow>=0 && newrow<=7 && newcol>=0 && newcol<=7 ) ? 1:0;
	       }
	       if(coord && Tabuleiro_atual[newrow][newcol] == player)
	       {
		 if(aux!=0)
		 {
		   existe = 1;
		   rowinc =10;/*escape*/
		   colinc =10;/*escape*/
		 }
	       }
	     }
	   aux=0;
	  }
    return existe;
}

/******************************************************************/
Lista_Mov*  gera_movimentos(JOGADOR pl)
{
    int i,j;
    Lista_Mov *lista=NULL,*fim_lista=NULL;

    for (i = 0; i <=7; i++)
      for (j = 0; j <=7; j++)
	if (movimento_valido(i,j,pl))
	  {
		 if(lista == NULL) {
		   lista = (Lista_Mov*)malloc(sizeof(Lista_Mov));
		   if ( lista == NULL ) {
			printf("Erro de alocacao");
			exit (1);
			}
		   lista->row = i;
		   lista->col = j;
		   lista->next = NULL;
		   fim_lista=lista;
		  }
		  else
		  {
		    fim_lista->next = (Lista_Mov*)malloc(sizeof(Lista_Mov));
		    if ( fim_lista->next == NULL ) {
			printf("Erro de alocacao");
			exit (1);
			}
		   fim_lista = fim_lista->next;
		   fim_lista->row = i;
		   fim_lista->col = j;
		   fim_lista->next = NULL;
		  }
	  }
    return(lista);
}

/******************************************************************/
JOGADOR  QuemVenceu(char computer,int* diferenca)
    {
      int comp_count = 0,human_count = 0,col,row ;
      char human = (computer == 'X') ? 'O' : 'X';
      for ( col = 0; col <=7; col++)
	for ( row = 0; row <=7; row++)
	  {
	    if (Tabuleiro_atual[row][col] == computer)
	      comp_count++;
	    else if (Tabuleiro_atual[row][col] == human)
	      human_count++;
	  }
      *diferenca= (comp_count - human_count) > 0 ? (comp_count - human_count) : -(comp_count - human_count);
      if (comp_count > human_count)
	  return(computer == 'X' ? JogadorUm : JogadorDois);
      else if (human_count > comp_count)
	  return(computer == 'X' ? JogadorDois : JogadorUm);
      else
	  return NenhumJogador;
 }

/******************************************************************/
void  movimenta(int move, JOGADOR pl)
{
    char player;
    Lista_Mov *lista_mov, *lista_aux;

    player = (pl == JogadorUm) ? 'X' : 'O';

    Tabuleiro_atual[move/8][move%8] = player;

    lista_mov = gera_trocas(move/8,move%8,pl);
    while(lista_mov != NULL){
      Tabuleiro_atual[lista_mov->row][lista_mov->col] = player;
      lista_aux = lista_mov;
      lista_mov = lista_mov->next;
      lista_aux->next = NULL;
      free(lista_aux);
    }
}

/******************************************************************/
Lista_Mov *  gera_trocas(int row, int col, JOGADOR pl)
{
char player = (pl == JogadorUm) ? 'X' : 'O';
char opp= outro_char(player);
int cnt=0,aux=0,coord,k,newcol,newrow,mov_row[9],mov_col[9],colinc,rowinc;
Lista_Mov * lista= NULL, *lista_aux= NULL;

      for (colinc = -1; colinc < 2; colinc++)
	for (rowinc = -1; rowinc < 2; rowinc++)
	  {
	     if (colinc==0 && rowinc==0) continue;
	      newrow = row + rowinc;
	      newcol = col + colinc;
	      coord = ( newrow>=0 && newrow<=7 && newcol>=0 && newcol<=7 ) ? 1:0 ;
	      while (coord && Tabuleiro_atual[newrow][newcol]==opp)
	       {
		 aux++;
		 mov_row[aux] = newrow;
		 mov_col[aux] = newcol;
		 newrow += rowinc;
		 newcol += colinc;
		 coord = ( newrow>=0 && newrow<=7 && newcol>=0 && newcol<=7 ) ? 1:0;
	       }
	     if(coord)
	      if(Tabuleiro_atual[newrow][newcol] == player){
		cnt += aux;
		for( k=1;k<=aux;k++)
		  if(lista == NULL) {
		   if ( !(lista = (Lista_Mov*)malloc(sizeof(Lista_Mov))) ) {
			printf("Erro de alocacao");
			exit (1);
			}
		   lista->row =mov_row[k];
		   lista->col =mov_col[k];
		   lista->next = NULL;
		   lista_aux=lista;
		  }
		  else
		  {
		    if ( !(lista_aux->next = (Lista_Mov*)malloc(sizeof(Lista_Mov))) ) {
			printf("Erro de alocacao");
			exit (1);
			}
		   lista_aux->next->row =mov_row[k];
		   lista_aux->next->col =mov_col[k];
		   lista_aux->next->next = NULL;
		   lista_aux = lista_aux->next;
		  }
		}
	     aux=0;
	  }
    return lista;
}

/******************************************************************/
JOGADOR outro_jogador(JOGADOR pl)
{
    return(pl == JogadorUm ? JogadorDois: JogadorUm);
}

/******************************************************************/
char  outro_char(char player)
{
    return(player == 'X' ? 'O' : 'X');
}

/******************************************************************/
void  inicializa_tabuleiro(void)
{
int i,j;
    for ( i = 0; i <=7; i++)
      for ( j = 0; j <=7; j++)
	Tabuleiro_atual[i][j] = Vazio;

    for ( i = 0; i <=7; i++)
      for ( j = 0; j <=7; j++)
	Tabuleiro_anterior[i][j] = Vazio;

    Tabuleiro_anterior[3][3] = 'X';    Tabuleiro_anterior[3][4] = 'O';
    Tabuleiro_anterior[4][3] = 'O';    Tabuleiro_anterior[4][4] = 'X';

    Tabuleiro_atual[3][3] = 'X';    Tabuleiro_atual[3][4] = 'O';
    Tabuleiro_atual[4][3] = 'O';    Tabuleiro_atual[4][4] = 'X';

 //valores heuristicos atribuidos ao tabuleiro
 VlrHeur[0][0] =100;VlrHeur[0][1] =  5; VlrHeur[0][2] = 17; VlrHeur[0][3] =13;  VlrHeur[0][4] =13; VlrHeur[0][5] = 17; VlrHeur[0][6] =  5; VlrHeur[0][7] =100;
 VlrHeur[1][0] = 5; VlrHeur[1][1] =-33; VlrHeur[1][2] = -5; VlrHeur[1][3] =-6;  VlrHeur[1][4] =-6; VlrHeur[1][5] = -5; VlrHeur[1][6] =-33; VlrHeur[1][7] = 5;
 VlrHeur[2][0] =17; VlrHeur[2][1] = -5; VlrHeur[2][2] =  5; VlrHeur[2][3] = 1;  VlrHeur[2][4] = 1; VlrHeur[2][5] =  5; VlrHeur[2][6] = -5; VlrHeur[2][7] =17;
 VlrHeur[3][0] =13; VlrHeur[3][1] = -6; VlrHeur[3][2] =  1; VlrHeur[3][3] = 0;  VlrHeur[3][4] = 0; VlrHeur[3][5] =  1; VlrHeur[3][6] = -6; VlrHeur[3][7] =13;
 VlrHeur[4][0] =13; VlrHeur[4][1] = -6; VlrHeur[4][2] =  1; VlrHeur[4][3] = 0;  VlrHeur[4][4] = 0; VlrHeur[4][5] =  1; VlrHeur[4][6] = -6; VlrHeur[4][7] =13;
 VlrHeur[5][0] =17; VlrHeur[5][1] = -5; VlrHeur[5][2] =  5; VlrHeur[5][3] = 1;  VlrHeur[5][4] = 1; VlrHeur[5][5] =  5; VlrHeur[5][6] = -5; VlrHeur[5][7] =17;
 VlrHeur[6][0] = 5; VlrHeur[6][1] =-33; VlrHeur[6][2] = -5; VlrHeur[6][3] =-6;  VlrHeur[6][4] =-6; VlrHeur[6][5] = -5; VlrHeur[6][6] =-33; VlrHeur[6][7] = 5;
 VlrHeur[7][0] =100;VlrHeur[7][1] = 5;  VlrHeur[7][2] = 17; VlrHeur[7][3] =13;  VlrHeur[7][4] =13; VlrHeur[7][5] = 17; VlrHeur[7][6] =  5; VlrHeur[7][7] =100;

}

/******************************************************************/
void  mostra_pecas(char tomove,char jogou_antes,char computer,int posicao_atual, int posicao_anterior)
{
int i,j,coluna,linha;
char computador[]="Computador",humano[]="Humano";
char nome_jogo[]= "R E V E R S O";

    gotoxy(1,1);clreol();
    gotoxy(34,1); textcolor(YELLOW); cprintf("%s",nome_jogo);
    gotoxy(57,1);textcolor(LIGHTBLUE);cprintf("N¡vel:%s",nivel_j[nivel]);


    gotoxy(1,2);clreol();
    gotoxy(7,2);textcolor(WHITE);cprintf("%s","Jogada atual:");
    gotoxy(20,2);
    textcolor(BROWN);
    cprintf("%c",tomove);
    gotoxy(22,2); textcolor(WHITE);cprintf("%s",tomove==computer?computador:humano);
    gotoxy(33,2);
    textcolor(BROWN);
    cprintf("%d,%d",posicao_atual/8,posicao_atual%8);


    gotoxy(45,2);textcolor(WHITE);cprintf("%s","Jogada anterior:");
    gotoxy(61,2);
    textcolor(BROWN);
    cprintf("%c",jogou_antes);
    gotoxy(63,2); textcolor(WHITE);cprintf("%s",jogou_antes==computer?computador:humano);
    if(posicao_anterior!=-1){
     gotoxy(74,2);
     textcolor(BROWN);
     cprintf("%d,%d",posicao_anterior/8,posicao_anterior%8);
    }
  linha=6;
  coluna= 7;
  gotoxy(coluna,linha);
  for ( i = 0; i <=7; i++ ){
      for ( j = 0; j <=7 ; j++ ){
	  if(8*i+j == posicao_atual)
	     textcolor(BROWN);
	  else
	  if(Tabuleiro_atual[i][j] == 'X')
	     textcolor(LIGHTBLUE);
	  else
	     textcolor(RED);
	  cprintf("%c", Tabuleiro_atual[i][j]);
	  coluna+=4;
	  gotoxy(coluna,linha);
      }
      coluna=46;
      gotoxy(coluna,linha);
      for ( j = 0; j <=7 ; j++ ){
	  if(8*i+j == posicao_anterior)
	     textcolor(BROWN);
	  else
	  if(Tabuleiro_anterior[i][j] == 'X')
	     textcolor(LIGHTBLUE);
	  else
	     textcolor(RED);
	  cprintf("%c", Tabuleiro_anterior[i][j]);
	  coluna+=4;
	  gotoxy(coluna,linha);
      }
      linha+=2;
      coluna= 7;
      gotoxy(coluna,linha);
   }
   /* Aqui o jogador X ou O ja jogou, entao devemos
      mostrar as posicoes possiveis para o outro jogador
   */
    mostra_marcas(tomove=='X'? 'O':'X');
    textcolor(WHITE);
}

/******************************************************************/
void  mostra_tabuleiro(char tomove,char computer)
{
int i,j,coluna,linha;

char computador[]="Computador",humano[]="Humano";
char nome_jogo[]= "R E V E R S O";

    textbackground(BLACK); textcolor(WHITE);clrscr();
    gotoxy(34,1);textcolor(YELLOW);cprintf("%s",nome_jogo);
    gotoxy(57,1);textcolor(LIGHTBLUE);cprintf("N¡vel:%s",nivel_j[nivel]);

    gotoxy(7,2);textcolor(WHITE);cprintf("%s","Jogada atual:");
    gotoxy(21,2);
    textcolor(BROWN);
    cprintf("%c",tomove);
    gotoxy(23,2);textcolor(WHITE);cprintf("%s",tomove==computer?computador:humano);

    gotoxy(48,2);textcolor(WHITE);cprintf("%s","Jogada anterior:");
    gotoxy(65,2);
    if(tomove==computer && computer=='X'){
       textcolor(BROWN);
       cprintf("%c",'O');
    }
    else
    if(tomove==computer && computer=='O'){
       textcolor(BROWN);
       cprintf("%c",'X');
    }
    else
     if(tomove!=computer && computer=='X'){
       textcolor(BROWN);
       cprintf("%c",'X');
     }
     else
     if(tomove!=computer && computer=='O'){
       textcolor(BROWN);
       cprintf("%c",'O');
     }
    gotoxy(67,2);textcolor(WHITE);cprintf("%s",tomove==computer?humano:computador);

    mostra_marcas(tomove);
    textcolor(WHITE);
    gotoxy(5,4);
    cprintf("  0   1   2   3   4   5   6   7          0   1   2   3   4   5   6   7");

    textcolor(BROWN);
    gotoxy(5,5);
    cprintf("ÉÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍ»      ÉÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍËÍÍÍ»");

    linha=6;
    for ( i = 0; i <=7; i++ ){
	 coluna=5;
	 gotoxy(3,linha);
	 textcolor(WHITE);
	 cprintf("%d",i);
      for ( j = 1; j <=9 ; j++ ){
	  gotoxy(coluna,linha);
	  textcolor(BROWN);
	  cprintf("º");
	  coluna+=4;
      }
	gotoxy(42,linha);
	textcolor(WHITE);
	cprintf("%d",i);

	coluna = 44;
      for ( j = 1; j <=9 ; j++ ){
	  gotoxy(coluna,linha);
	  textcolor(BROWN);
	  cprintf("º");
	  coluna+=4;
      }
      linha+=1;
      if(i!=7)
      {
	gotoxy(5,linha);
	textcolor(BROWN);
	cprintf("ÌÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍ¹      ÌÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍÎÍÍÍ¹");
       }
      else
      {
       gotoxy(5,linha);
       textcolor(BROWN);
       cprintf("ÈÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍ¼      ÈÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍÊÍÍÍ¼");
      }
      linha+=1;
   }

    gotoxy(19,12); textcolor(LIGHTBLUE);
    cprintf("%c", Tabuleiro_atual[3][3]);
    gotoxy(23,12); textcolor(RED);
    cprintf("%c", Tabuleiro_atual[3][4]);
    gotoxy(19,14); textcolor(RED);
    cprintf("%c", Tabuleiro_atual[4][3]);
    gotoxy(23,14);textcolor(LIGHTBLUE);
    cprintf("%c", Tabuleiro_atual[4][4]);

    gotoxy(58,12); textcolor(LIGHTBLUE);
    cprintf("%c", Tabuleiro_anterior[3][3]);
    gotoxy(62,12); textcolor(RED);
    cprintf("%c", Tabuleiro_anterior[3][4]);
    gotoxy(58,14); textcolor(RED);
    cprintf("%c", Tabuleiro_anterior[4][3]);
    gotoxy(62,14); textcolor(LIGHTBLUE);
    cprintf("%c", Tabuleiro_anterior[4][4]);
    textcolor(WHITE);

}

/******************************************************************/
void  mostra_marcas(char tomove){
    Lista_Mov * ListaMovJog, *laux;
    ListaMovJog=gera_movimentos(tomove=='X'?JogadorUm:JogadorDois);
    while(ListaMovJog!=NULL)
      {
       laux=ListaMovJog;
       gotoxy(7+4*ListaMovJog->col,6+2*ListaMovJog->row);
       if(tomove == 'X')
	textcolor(LIGHTBLUE);
       else
	textcolor(LIGHTRED);
       cprintf("%c",'.');
       ListaMovJog = ListaMovJog->next;
       laux->next = NULL;
       free(laux);
      }
      textcolor(WHITE);
}

/******************************************************************/
void  apaga_marcas(JOGADOR pl){
    Lista_Mov * ListaMovJog, *laux;
    ListaMovJog=gera_movimentos(pl);
    while(ListaMovJog!=NULL)
      {
       laux=ListaMovJog;
       gotoxy(7+4*ListaMovJog->col,6+2*ListaMovJog->row);
       textcolor(BLACK);
       cprintf("%c",' ');
       ListaMovJog = ListaMovJog->next;
       laux->next = NULL;
       free(laux);
      }
      textcolor(WHITE);
}

/******************************************************************/
char pergunta(int col, int lin, char string[50] ){
    char answer[35];
    do{
       gotoxy(col,lin);
       cprintf("%s",string);
       gets(answer);
     }
     while(answer[0] != 's' && answer[0] != 'S' && answer[0] != 'n' && answer[0] != 'N');

  if(answer[0] == 's' || answer[0] == 'S')
      return('s');

 return('n');
}

/******************************************************************/
void main(void)
{
    int depth=0,row,col,posicao,posicao_anterior=-1,Aviso=0,diferenca,i,j;
    char answer[8],resposta,computer, human,tomove,quem_jogou_antes=' ';
    JOGADOR pl;
   do{
      textbackground(BLACK); textcolor(WHITE);clrscr();
      gotoxy(27,1);  cprintf("REVERSO por Edson Perotoni");
      gotoxy(14,2);  cprintf("Para disciplina de Inteligˆncia Artificial");
      gotoxy(14,3);  cprintf("Professor Felipe Muller - INFORMµTICA - UFSM");
      gotoxy(19,5);  cprintf("Qual o n¡vel de dificuldade do jogo? ");

      gotoxy(19,6); textcolor(LIGHTBLUE);  cprintf("1 - moleza!");
      gotoxy(19,7); textcolor(LIGHTBLUE);  cprintf("2 - iniciante");
      gotoxy(19,8); textcolor(LIGHTBLUE);  cprintf("3 - tem que pensar...");
      gotoxy(19,9); textcolor(LIGHTBLUE);  cprintf("4 - complicou!");
      gotoxy(19,10); textcolor(LIGHTBLUE); cprintf("5 - experiente");
      gotoxy(19,11); textcolor(LIGHTBLUE); cprintf("6 - mestre!");

      gotoxy(19,12); textcolor(WHITE);cprintf("==>");
    do{
     gotoxy(22,12); textcolor(WHITE);
      gets(answer);
      depth = atoi(answer);
      nivel = depth -1;
      if(depth==1)
	depth=0;
      som(200);
    }
 while(depth!=0 && depth!=2 && depth!=3 && depth!=4 && depth!=5 && depth!=6);
     gotoxy(56,5);  cprintf("%d",depth==0?1:depth);

     if (pergunta(14,12,"Vocˆ quer avisos sonoros(s/n)?") == 's')
       Aviso =1;
     else
       Aviso =0;

   if (pergunta(14,14,"Vocˆ quer mover primeiro (s/n)? ") == 's')
    {
	tomove = human = 'X';
	computer = 'O';
    }
    else
    {
	tomove = computer = 'X';
	human = 'O';
    }

     gotoxy(19,16); cprintf("INSTRU€åES:");
     gotoxy(14,17); cprintf("Para escolher a posi‡Æo do tabuleiro");
     gotoxy(14,18); cprintf("digite um n£mero inteiro que ‚ composi‡Æo da linha e da coluna");
     gotoxy(14,19); cprintf("EXEMPLO=> linha: 3 coluna: 7 ====> digite 37");
     gotoxy(14,20); cprintf("N£mero negativo abandona a partida");
     gotoxy(14,20); cprintf("BOA SORTE! Pressione uma tecla para iniciar...");
     getch();

     inicializa_tabuleiro();
     mostra_tabuleiro(tomove,computer);

    while (continua_jogo(JogadorUm) || continua_jogo(JogadorDois))
    {
	if (tomove == human)
	{
	    if( !continua_jogo(human == 'O' ? JogadorDois : JogadorUm) )
	    {
		 gotoxy(5,22);clreol();
		 gotoxy(5,23);clreol();
		 gotoxy(5,22);
		 textcolor(WHITE);
		 if(Aviso){
		    som(200);
		    som(250);
		 }
		 cprintf("Voce tem que passar uma jogada! Pressione ...");
		 getch();
		 /* mostra marcas do outro jogador*/
		  apaga_marcas(human=='X'?JogadorUm:JogadorDois);
		  mostra_marcas(computer);
	     }
	    else
	    {
	      do{
		 int movimento;
		 gotoxy(5,22);clreol();
		 gotoxy(5,23);clreol();
		 gotoxy(5,22); textcolor(WHITE);
		 if(Aviso)
		    som(200);
		 cprintf("Qual o seu movimento? ");
		 gets(answer);
		 movimento = atoi(answer);
		 if(movimento<0){
		     if(Aviso){
			som(500);
			som(700);
			som(500);
		     }
		      gotoxy(5,22);clreol();
		      gotoxy(5,22);textcolor(WHITE);
		      if (pergunta(5,22,"Vocˆ quer abandonar a partida(s/n)? ") == 's')
			 goto label;
		      else
		       {
			 row = -1; col = -1; // valores invalidos
			}
		 }
		 else{
		   row = movimento/10;
		   col = movimento%10;
		   gotoxy(1,22);clreol();
		   gotoxy(1,23);clreol();
		   posicao = 8*row+col;
		 }
	       }while ( !(movimento_valido(row,col,human == 'O' ? JogadorDois : JogadorUm)) );
	       for (i = 0; i <=7; i++)
		for (j = 0; j <=7; j++)
			 Tabuleiro_anterior[i][j] =  Tabuleiro_atual[i][j];

	       movimenta(posicao, human == 'X' ? JogadorUm:JogadorDois);
	       mostra_pecas(human,quem_jogou_antes,computer,posicao,posicao_anterior);
	       posicao_anterior = posicao;
	       quem_jogou_antes = human;
	     }
	     tomove = computer;
	}
	else if (tomove == computer)
	{
	   if( !continua_jogo(computer == 'X' ? JogadorUm : JogadorDois) )
	    {
		    gotoxy(5,22);clreol();
		    gotoxy(5,23);clreol();
		    gotoxy(5,22);textcolor(WHITE);
		    if(Aviso){
		      som(200);
		      som(250);
		    }
		    cprintf("Eu devo passar uma jogada! Pressione ...");
		    getch();
		    /* mostra marcas do outro jogador */
		     apaga_marcas(computer=='X'?JogadorUm:JogadorDois);
		     mostra_marcas(human);

	     }
	     else
	     {
		gotoxy(5,22);clreol();
		gotoxy(5,23);clreol();
		if(depth>0){
		  gotoxy(5,22);textcolor(WHITE);
		  cprintf("Estou pensando ...");
		  minimax(computer,computer == 'X' ? JogadorUm : JogadorDois,
				  2+depth, &posicao,-Infinito,Infinito);

		}
		else
		{
		    int i=0,n;
		    Lista_Mov *laux, *mov_possiveis =  gera_movimentos(computer == 'X' ? JogadorUm : JogadorDois);
		    laux= mov_possiveis;
		    while(laux!=NULL){
			 laux = laux->next;
			 i+=1;
		    }
		    n = random(i);
		    while(n){
		      laux= mov_possiveis;
		      mov_possiveis = mov_possiveis->next;
		      laux->next = NULL;
		      free(laux);
		      n-=1;
		    }
		    posicao = 8*mov_possiveis->row +mov_possiveis->col;
		    while(mov_possiveis!=NULL){
		      laux= mov_possiveis;
		      mov_possiveis = mov_possiveis->next;
		      laux->next = NULL;
		      free(laux);
		    }
		}
		for (i = 0; i <=7; i++)
		   for (j = 0; j <=7; j++)
			 Tabuleiro_anterior[i][j] =  Tabuleiro_atual[i][j];

		movimenta(posicao, computer == 'X' ? JogadorUm : JogadorDois);
		mostra_pecas(computer,quem_jogou_antes,computer,posicao,posicao_anterior);
		posicao_anterior = posicao;
		quem_jogou_antes = computer;
	     }
	     tomove = human;
	}

    }
   pl =  QuemVenceu(computer,&diferenca);
   if(Aviso)
	   music();
   gotoxy(5,22);clreol();
   gotoxy(5,22);textcolor(WHITE);
   if (pl == JogadorUm)
     if (computer=='X')
	cprintf("S¢ lamento, mas EU venci com %d pe‡as a mais!",diferenca);
     else
	cprintf("Raios! VOCÒ venceu com %d pe‡as a mais!",diferenca);
   else
   if (pl == JogadorDois)
     if (human=='O')
	cprintf("Raios! VOCÒ venceu com %d pe‡as a mais!",diferenca);
     else
	cprintf("S¢ lamento, mas EU venci com %d pe‡as a mais!",diferenca);
  else
	cprintf("Humano, n¢s EMPATAMOS!");
label:
     gotoxy(5,23);clreol();
     gotoxy(5,23);textcolor(WHITE);
     resposta=pergunta(5,23,"Vocˆ quer jogar novamente(s/n)?");
  }
  while(resposta == 's');

  textbackground(BLACK);
  textcolor(WHITE);
  clrscr();
}
/******************************************************************/