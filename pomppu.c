#include <stdio.h> 
#include <conio.h>
#include "keyboard.h"
#include "nopeus.c"

void piirra_boxi(int x1,int y1,int x2,int y2,char boxvari);
void grtila(int tila);

void sprite_do(int _x,int _y,int aluex,int aluey,char kumpi,char lahella);
void sprite_read(char kumpi);

FILE *_tied;
char *tulos,anim[2],lautalla,hyppy,aani=1;

char aijavas1[112],aijavas2[112],aijaoik1[112],aijaoik2[112],dia_spr[30],*buf,tiedluku[20],turbo_pc;
//7*16 = 112   ;  5*6 = 30

int y,x,lives,diamonds,nopeus,aika,level,lasku,lauttax[20],lauttay[20],seinax[10],seinay[10],diamondx[10],diamondy[10],enkka;


/*************************************************************'

funktiot:

piirra_boxi = piirtää bufferiin yksivärisen laatikon
parametrit:
x1,y1 = vasemman yläkulman koordinaatit
x2,y2 = oikean alakulman koordinaatit
boxvari = laatikon väri
**
grtila = vaihtaa grafiikka/teksti tilaan/varaa muistia & alustaa bufferin
         /flippaa puskurin näytölle
tila = mitä yllä olevista vaihtoehdoista tekee
**
sprite_do = piirtää yhden spriten
_x,_y = vasemman yläkulman koordinaatit
aluex = kuvan leveys
aluey = kuvan korkeus
kumpi = mikä sprite (alunperin altogrimissa 2 spriteä...)
lahella = skaalaus (yli 1, kokonaislukuja)
**
sprite_read = lukee spriten tiedostosta
kumpi = mikä sprite
***********
muuttujat:

_tied = yleishyödyllinen tiedosto (kaikki data on samassa tiedostossa,
        paitsi hiscore)
**
tulos = tiedoston osoittaja...
anim = ööh.. [0] mitä animaatiota jane käyttää [1] milloin tulee
       seuraava animaatio.
lautalla = onko jane lautan päällä (ei tipu lautan läpi).
hyppy = kuinka paljon hyppyvoimaa jäljellä (kun on painettu hyppy näppiä)
aani = onks äänet päällä??? KUULUUKO???!
aijavas1+2,aijaoik1+2,dia_spr = spritejä nuo kaikki (yksi numero indexissä
                                edustaa yhtä väriä (/väripistettä))
*buf = näyttöbufferi      HUOM!
tiedluku = lukee levelit ja niitten nimet!
turbo_pc = 1= windows3.1 tai win9X,2X me mutta 2= WIN (badbadbad) XP!
**
y,x = janen sijainti
lives = elämiä
diamonds = monta timanttia keräämättä
nopeus = pc:n nopeus
aika = aikaa jälellä ennen kun menee enkuu!
level = mikä leveli
lasku = yleishyödyllinen (h)intti
lauttax/y = lauttojen koordinaatit (20 lauttaa per level)
diamondx/y = timanttejen/kristallejen koordinaatit (10 timanttii per level)
enkka = ennätys
  -- EIPÄ MITTÄÄN YLLÄTTÄVVÄÄ.. NYT VAAN SPACEJABIN KIMPUUN
	--- ( : - O ) NEIN!!!!!!!!!!!!!!
'**************************************************************/


void main()
{


 printf("DO YOU WANT PC-SPEAKER SOUND? 1: yes 2: no\n");
hyppy=getch();
if(hyppy=='1')
{
printf("SOUNDS ON\n");
aani=1;
}
else
{
printf("NO SOUND\n");
aani=0;
}
clrscr();
printf("DO YOU HAVE WINDOWS XP OR BETTER?\n1: YES\n2: NO\n");

hyppy=getch();
if(hyppy=='1')
{
printf("WIN XP\n");
turbo_pc=2;
}
else
{
printf("-\n");
turbo_pc=1;
}


srand((int)time(NULL));
nopeus=saada_nopeus();
grtila('A');		//Alustetaan bufferi
grtila(1);
setkeyhandler();

 clrscr();
 printf("HI!\nWelcome to Crystal Jane\n\nStory:\nJane has been ten years in a prison\n");
 printf("of a cruel stoneman.\nOne day stoneman says to free Jane\nif she collects all");
 printf(" the lifecrystals\non that planet...\n\nPRESS ENTER\n");
 while(!keybuffer[SxENTER]){}
 viive(nopeus*5*turbo_pc);
piirra_boxi(0,0,320,200,0);
grtila('F');
 clrscr();
 printf("You'll be able to control Jane with\n<- and -> arrows\n");
 printf("and Jane jumps if you press CONTROL.\nJane can also go back to the beginning\nof a level by pressing ENTER.\nPRESS P ANYTIME TO PAUSE GAME.\nYour mission is to get");
 printf(" the lifecrystals\non 15 levels.\n\nPRESS ENTER\n");
 while(!keybuffer[SxENTER]){}
 viive(nopeus*5*turbo_pc);
piirra_boxi(0,0,320,200,0);
grtila('F');
 clrscr();
 printf("You can see your lives in the\nupper left corner\nand time running next to it...\nBest of luck for you!\n\nPRESS ENTER\n");
 while(!keybuffer[SxENTER]){}
 viive(nopeus*5*turbo_pc);
 clrscr();

alku:
printf("LOADING SPRITES...\n");

_tied=fopen("hiscore.jan","r");
  enkka=fgetc(_tied);
fclose(_tied);

_tied=fopen("gdat.dat","r");

if(_tied==NULL)
{
fclose(_tied);
grtila(1);
printf("\nERROR WHILE LOADING GAME DATA (gdat.dat)\n");
resetkeyhandler();
gets(aijaoik1);
exit(1);
}

sprite_read('<');
sprite_read('{');
sprite_read('>');
sprite_read('}');
sprite_read('D');


lives=8;
level=0;
anim[0]='>';
anim[1]=0;
diamonds=0;

piirra_boxi(0,0,320,200,0);
 sprite_do(100,80,7,16,'>',2);
 sprite_do(140,94,5,6,'D',2);
grtila('F');
clrscr();
printf("Crystal Jane (c) Upr00ted tree software\n        MENU\n        1: PLAY\n        2: QUIT\n\n\n\n\n        HISCORE: %d\n",enkka);
while(1)
{
if(keybuffer[Sx2])
{
grtila(0);
printf("+********************+\n* THANKS FOR PLAYING *\n*    CRYSTAL JANE    *\n+********************+\n");
printf("Crystal Jane - (c) Upr00ted tree software\nAll comments to: joonas1000@jippii.fi\n");
exit(1);
}

if(keybuffer[Sx1])
break;
}


while(lives>0&&!keybuffer[SxESC])
{
	if(diamonds==0)
	{
	piirra_boxi(0,0,320,200,0);
	grtila('F');
	clrscr();
	 if(level!=15){
	printf("\n\n\n\n         GET READY FOR ACTION!\n\n\n                 LEVEL:\n");

	 tulos=fgets(tiedluku,20,_tied);
	 tulos=fgets(tiedluku,20,_tied);

	 printf("         %d: %s\n\n\n\nPress enter...\n",level+1,tiedluku);
	while(!keybuffer[SxENTER]){}
	 }



	 if(level==15){
	 anim[1]=0;

	 sprite_read('<');
	 fclose(_tied);
	 for(x=0;x<200;x=x+3)
	 {
	 piirra_boxi(0,0,320,200,0);
	 anim[1]++;
	 if(anim[1]==7)
	 anim[1]=0;

	 if(anim[1]<3)
	 anim[0]='>';
	 else
	 anim[0]='}';
	 sprite_do(x,80,7,16,anim[0],2);
	 sprite_do(250,48,7,16,'<',4);

	 grtila('F');
	 viive(nopeus*turbo_pc);
	 }
	 clrscr();
	 piirra_boxi(0,0,320,200,0);
	 sprite_do(x,80,7,16,anim[0],2);
	 sprite_do(250,48,7,16,'<',4);
	 grtila('F');
	 printf("JANE: NOW I HAVE COLLECTED\nALL THE CRYSTALS!\n[ENTER]\n");
	 while(!keybuffer[SxENTER]){}
	 viive(nopeus*5*turbo_pc);
	 clrscr();
	 piirra_boxi(0,0,320,200,0);
	 sprite_do(250,48,7,16,'<',4);
	 sprite_do(x,80,7,16,anim[0],2);
	 grtila('F');
	 printf("STONEMAN: YES. I SEE.\nNOW YOU'LL BE FREE TO GO...\n[ENTER]\n");
	 while(!keybuffer[SxENTER]){}
	 viive(nopeus*5*turbo_pc);
	 clrscr();
	 piirra_boxi(0,0,320,200,0);
	 sprite_do(250,48,7,16,'<',4);
	 sprite_do(x,80,7,16,anim[0],2);
	 grtila('F');
	 printf("CONGRATULATIONS!!!\nYOU HAVE COMPLETED CRYSTAL JANE!\n\nLIVES BONUS: %d\n",lives*10);

	 if(lives*10>enkka)
	 {
	 _tied=fopen("hiscore.jan","w");
	  fputc(lives*10,_tied);
	 fclose(_tied);
	 printf("NEW HISCORE!\n");
	 }
	 else
	 printf("HISCORE: %d\n",enkka);

	 while(!keybuffer[SxENTER]){}
	 viive(nopeus*5*turbo_pc);
	 goto alku;}





	 aika=0;
	 level++;
	 hyppy=-1;
	 lautalla='K';
	 x=15;
	 y=160;

	 diamonds=10;


	 for(lasku=0;lasku<20;lasku++)		//LAUTAT TIEDOSTOSTA
	 {
	 tulos=fgets(tiedluku,10,_tied);
	 lauttax[lasku]=atoi(tiedluku);
	 tulos=fgets(tiedluku,10,_tied);
	 lauttay[lasku]=atoi(tiedluku);
	 }
	 for(lasku=0;lasku<10;lasku++)		//SEINÄT TIEDOSTOSTA
	 {
	 tulos=fgets(tiedluku,10,_tied);
	 seinax[lasku]=atoi(tiedluku);
	 tulos=fgets(tiedluku,10,_tied);
	 seinay[lasku]=atoi(tiedluku);
	 }
	 for(lasku=0;lasku<10;lasku++)		//TIMANTIT TIEDOSTOSTA
	 {
	 tulos=fgets(tiedluku,10,_tied);
	 diamondx[lasku]=atoi(tiedluku);
	 tulos=fgets(tiedluku,10,_tied);
	 diamondy[lasku]=atoi(tiedluku);
	 }


	}
	piirra_boxi(0,0,320,200,DARKGRAY);

	if(hyppy>0)
	{
	y=y-5;
	lautalla='E';
	sound((20+hyppy*5)*aani);
	}
	hyppy--;

	 if(hyppy<0)
	 {
	  lautalla='E';
	  for(lasku=0;lasku<20;lasku++)
	  {
	   if(lauttax[lasku]+50>x&&lauttax[lasku]-14<x&&lauttay[lasku]+1<y+32&&lauttay[lasku]+4>y+32)
	   {
	   lautalla='K';
	   hyppy=-1;
	   }

		


	  }
 	 if(lautalla=='E'&&y<159)
  	 {
  	 y=y+3;
	 hyppy=-1;
  	 }
	 else
	 {
  	 lautalla='K';
	 hyppy=-1;
	 }
	}
	  for(lasku=0;lasku<10;lasku++)
	  {
		 if(diamondx[lasku]+10>x&&diamondx[lasku]<x+14&&diamondy[lasku]<y+32&&diamondy[lasku]+10>y)   //OTA TIMANTTI
		 {
		  diamonds--;
		  diamondx[lasku]=500;
		  diamondy[lasku]=500;
		 }
	  }

	if(keybuffer[SxP])
	{
	 clrscr();
	 printf("\n\n\nGAME PAUSED\nPRESS SPACE TO CONTINUE\n");
	 while(!keybuffer[SxSPACE]){}
	}

	if(keybuffer[SxENTER])			//ALOITA ALUSTA
	{x=15;y=160;lautalla='K';hyppy=-1;}

	if(keybuffer[SxRIGHT]&&x<305)		//LIIKU OIKEAAN
	{
	 anim[1]++;

	 if(anim[0]=='<'||anim[0]=='{')
	  anim[0]='>';

		if(anim[1]==3)
		{
		 if(anim[0]=='}')
		  {
		  anim[0]='>';
		  sound(1*aani);
		  }
		 else
		  {
		  anim[0]='}';
		  sound(8*aani);
		  }

		 anim[1]=0;
		}
	  x=x+3;
	  for(lasku=0;lasku<10;lasku++)			//OSUUKO SEINÄÄN
	  {
		if(seinax[lasku]>x&&seinax[lasku]-11<x&&seinay[lasku]<y+30&&seinay[lasku]+30>y)
		 x=x-3;
	  }

	}

	if(keybuffer[SxLEFT]&&x>5)		//LIIKU VASEMPAAN
	{
	 anim[1]++;

	 if(anim[0]=='>'||anim[0]=='}')
	  anim[0]='<';

		if(anim[1]==3)
		{
		 if(anim[0]=='{')
		  {
		  anim[0]='<';
		  sound(1*aani);
		  }
		 else
		  {
		  anim[0]='{';
		  sound(8*aani);
		  }

		 anim[1]=0;
		}
	  x=x-3;
	  for(lasku=0;lasku<10;lasku++)			//OSUUKO SEINÄÄN
	  {
		if(seinax[lasku]+5>x&&seinax[lasku]<x&&seinay[lasku]<y+30&&seinay[lasku]+30>y)
		 x=x+3;
	  }

	}

	if(keybuffer[SxCTRL]&&lautalla=='K')	//HYPPÄÄ!
	 hyppy=6;


	if(keybuffer[SxF5]&&keybuffer[SxJ]&&lautalla=='K')	//KOODIT
	 hyppy=12;
	if(keybuffer[SxF5]&&keybuffer[SxD]&&y<160)	//KOODIT
	 lautalla='E';
	if(keybuffer[SxF5]&&keybuffer[SxF]&&hyppy<1)	//KOODIT
	 hyppy=6;

	//*LAUTAT + MUUT*//

		 piirra_boxi(0,190,320,200,YELLOW);
	for(lasku=0;lasku<20;lasku++)
	{
		 piirra_boxi(lauttax[lasku]-1,lauttay[lasku],lauttax[lasku]+50,lauttay[lasku]+5,YELLOW);
		if(lasku<10)
		 piirra_boxi(seinax[lasku],seinay[lasku],seinax[lasku]+5,seinay[lasku]+30,YELLOW);
	}

	for(lasku=0;lasku<10;lasku++)
	{
		if(diamondx[lasku]<500)
		 sprite_do(diamondx[lasku],diamondy[lasku],5,6,'D',2);
	}

	sprite_do(x,y-2,7,16,anim[0],2);

	for(lasku=0;lasku<lives;lasku++)	//PIIRRÄ ELÄMÄT
	 sprite_do(lasku*10,1,5,6,'D',1);

	piirra_boxi(125,1,125+(175-aika/2),6,LIGHTRED);		//PIIRRÄ AIKA

	aika++;
	 if(aika>350)
	 {
	  lives--;
	  aika=0;
	 }
	 if(aika>300)
	 {
	  sound(10*aani);
	 }

	grtila('F');
	viive(nopeus*turbo_pc);
	sound(0);

	}
 piirra_boxi(0,0,320,200,0);
 grtila('F');
 clrscr();
 printf("\n\n         GAME OVER\nYOU REACHED LEVEL: %d / 15\n\n\n\n\n    PRESS SPACE",level);
fclose(_tied);
 while(!keybuffer[SxSPACE]){}
goto alku;
}


void grtila(int tila)
{
if(tila==1)
textmode(0x13);
if(tila==0)
textmode(0x3);
if(tila=='A')
{

buf=(char *)malloc(64000); // Alustetaan puskuri
if(buf==NULL)
{
printf("Not enought conventional memory!\n");
resetkeyhandler();
exit(1);
}

}
if(tila=='F')
_dosmemputl(buf, 64000/4, 0xA0000); //Flipataan puskuri näyttömuistiin 
}


void piirra_boxi(int x1,int y1,int x2,int y2,char boxvari)
{
int y_buf;

if(y1<0)
y1=0;
if(y2>200)
y2=200;
if(x1<0)
x1=0;
if(x2>320)
x2=320;


do
{
for(y_buf=y1;y_buf<y2;y_buf++)
{
buf[y_buf*320+x1]=boxvari;
}

x1++;
}while(x1<x2);

}

void sprite_do(int _x,int _y,int aluex,int aluey,char kumpi,char lahella)
{
char _123x,_123y=0;
int laskuri=0;

while(aluey*lahella>_123y)
{
for(_123x=0;_123x<aluex*lahella;_123x=_123x+lahella)
{
if(kumpi=='>'&&aijaoik1[laskuri]!='n')
	piirra_boxi(_x+_123x,_y+_123y,_x+_123x+lahella,_y+_123y+lahella,aijaoik1[laskuri]);
if(kumpi=='}'&&aijaoik2[laskuri]!='n')
	piirra_boxi(_x+_123x,_y+_123y,_x+_123x+lahella,_y+_123y+lahella,aijaoik2[laskuri]);
if(kumpi=='<'&&aijavas1[laskuri]!='n')
	piirra_boxi(_x+_123x,_y+_123y,_x+_123x+lahella,_y+_123y+lahella,aijavas1[laskuri]);
if(kumpi=='{'&&aijavas2[laskuri]!='n')
	piirra_boxi(_x+_123x,_y+_123y,_x+_123x+lahella,_y+_123y+lahella,aijavas2[laskuri]);
if(kumpi=='D'&&dia_spr[laskuri]!='n')
	piirra_boxi(_x+_123x,_y+_123y,_x+_123x+lahella,_y+_123y+lahella,dia_spr[laskuri]);



laskuri++;
}
_123y=_123y+lahella;


}



}
void sprite_read(char kumpi)
{
char luetkos;
int laskuri=0;




luetkos=fgetc(_tied);

if(luetkos==' ')
luetkos='';

while(luetkos!='E')
{
if(luetkos!=10)
{
if(luetkos==' ')
luetkos='';
if(kumpi=='<')
aijavas1[laskuri]=luetkos-47;
if(kumpi=='{')
aijavas2[laskuri]=luetkos-47;
if(kumpi=='>')
aijaoik1[laskuri]=luetkos-47;
if(kumpi=='}')
aijaoik2[laskuri]=luetkos-47;
if(kumpi=='D')
dia_spr[laskuri]=luetkos-47;


laskuri++;
}
luetkos=fgetc(_tied);



}


}
