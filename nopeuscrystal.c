

void viive(long nop)
{
long mittari=0;
while(mittari<nop*50000)
mittari++;
}

int saada_nopeus()
{
FILE *huh;
int nopeus=0,saato=0,x,number;
printf("ллллллллллллллллллллл+ SPEEDTEST +ллллллллллллллллллллллллллл\n");
printf("                  Wait 5 to 7 seconds...\n");
x=time(NULL);
saato=0;
number=100;
while(time(NULL)-x<6)
{
viive(1);
saato++;
if(saato==number)
{
putch('|');
number=number+100;
}
}
nopeus=saato/138;



return(nopeus);


}