#include<stdio.h>
#include<algorithm>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<math.h>
using namespace std;

/*
#define NUM_OF_LINES (1000)
#define NUM_OF_CHILD (50)
#define HEIGHT (1080)
#define WIDTH (1920)
#define NUM_OF_SURVIVE (5)
*/
#define MAX_HEIGHT (2400)
#define MAX_WIDTH (2400)
#define MUTATE_POS (1000)

int LENGTH_OF_LINE = 100;
int NUM_OF_LINES = 2000;
int NUM_OF_CHILD = 200;
int HEIGHT = 1080;
int WIDTH = 1920;
int NUM_OF_SURVIVE = 50;
int NUM_OF_MUTATE = 2;

struct Img
{
	int R[MAX_HEIGHT][MAX_WIDTH];
	int G[MAX_HEIGHT][MAX_WIDTH];
	int B[MAX_HEIGHT][MAX_WIDTH];
};

Img input_img;
//FILE *imgin = fopen("input.bmp","rb");
//FILE *txtin = fopen("savedata.txt","r");

Img temp;

int NUM_OF_GEN;
int buffer[100];

int transImg[MAX_HEIGHT][MAX_WIDTH];

vector<vector<int> > GA;

int random(int number);
void make_Child();
void initialize();
void save();
void load();
long long calcFitness(int num);
void getImage();
//void drawLine(int x1, int y1, int x2, int y2);
void drawLines(int li);
bool compGA(int x, int y);
void trans_image();
void print_transImage();

int main()
{
	srand(time(NULL));
	getImage();
	printf("Get image file finished\n");
	FILE *txtin = fopen("savedata.txt","r");
	if(txtin == NULL)
	{
		printf("Save data not found.\n");
		initialize();
		printf("Initialization Finished\n");
	}
	else
	{
		fclose(txtin);
		load();
		printf("Load save data complete.\n");
		printf("Calculated %d Generation.\n", NUM_OF_GEN);
	}
	print_transImage();
	//make child in main()
	printf("NUM_OF_LINES : %d\n", NUM_OF_LINES);
	printf("NUM_OF_CHILD : %d\n", NUM_OF_CHILD);
	while(1)
	{
		++NUM_OF_GEN;
		printf("Calculating %d Generations...\n", NUM_OF_GEN);
		//choose top (NUM_OF_SURVIVE)
		//make child (NUM_OF_CHILD-NUM_OF_SURVIVE)
		//mutate 2 childs(NUM_OF_MUTATE)
		make_Child();
		printf("Generate Finished\n");
		//save()
		save();
	}
}

void print_transImage()
{
    int i, j;
    FILE *imgout = fopen("transed_Image.bmp", "wb");
	for(i=0; i<54; ++i) putc(buffer[i],imgout);//fprintf(imgout, "%d", buffer[i]);
	for(i=0; i<HEIGHT; ++i)
	{
		for(j=0; j<WIDTH; ++j)
		{
			putc(transImg[i][j],imgout);
			putc(transImg[i][j],imgout);
			putc(transImg[i][j],imgout);
		}
		if((WIDTH*3)%4==3){putc(0,imgout);}
		if((WIDTH*3)%4==2){putc(0,imgout);putc(0,imgout);}
		if((WIDTH*3)%4==1){putc(0,imgout);putc(0,imgout);putc(0,imgout);}
	}
	fclose(imgout);
}

void getImage()
{
	int i, j;
	FILE *imgin = fopen("input.bmp","rb");
	for(i=0; i<54; ++i) buffer[i] = getc(imgin);
	WIDTH = buffer[18] + buffer[19]*256 + buffer[20]*256*256 + buffer[21]*256*256*256;
	HEIGHT= buffer[22] + buffer[23]*256 + buffer[24]*256*256 + buffer[25]*256*256*256;
	printf("Width : %d, Height : %d\n", WIDTH, HEIGHT);
	LENGTH_OF_LINE = min(HEIGHT, WIDTH)/20;
	NUM_OF_LINES = 6*WIDTH*HEIGHT/25/LENGTH_OF_LINE;
	fflush(stdout);
	for(i=0; i<HEIGHT; ++i)
	{
		for(j=0; j<WIDTH; ++j)
		{
			input_img.B[i][j] = getc(imgin);
			input_img.G[i][j] = getc(imgin);
			input_img.R[i][j] = getc(imgin);
		}
		if((WIDTH*3)%4==3){getc(imgin);}
		if((WIDTH*3)%4==2){getc(imgin);getc(imgin);}
		if((WIDTH*3)%4==1){getc(imgin);getc(imgin);getc(imgin);}
	}
	fclose(imgin);
	//printf("Input Image Finished\n");
	//printf("Start Convert Image\n");
	//fflush(stdout);
	trans_image();
	return;
}

void initialize()
{
	vector<int> tmp(NUM_OF_LINES*3);
	for(int i=0; i<NUM_OF_CHILD; ++i)
	{
		for(int j=0; j<NUM_OF_LINES*3; ++j)
		{
		    if(j%3 == 0) tmp[j] = random(HEIGHT);
			else if(j%3 == 1) tmp[j] = random(WIDTH);
			else if(j%3 == 2) tmp[j] = random(360);
		}
		GA.push_back(tmp);
	}
	NUM_OF_GEN = 0;
	save();
}
void save()
{
	FILE *txtout = fopen("savedata.txt","w");
	fprintf(txtout, "%d %d %d %d %d\n", NUM_OF_GEN, HEIGHT, WIDTH, NUM_OF_CHILD, NUM_OF_LINES);
	for(int i=0; i<NUM_OF_CHILD; ++i)
	{
		for(int j=0; j<NUM_OF_LINES*3; ++j)
		{
			fprintf(txtout, "%d ", GA[i][j]);
		}
		fprintf(txtout, "\n");
	}
	fclose(txtout);
	return;
}
void load()
{
	vector<int> tmp;
	GA.clear(); tmp.clear();
	FILE *txtin = fopen("savedata.txt","r");
	fscanf(txtin, "%d %d %d %d %d", &NUM_OF_GEN, &HEIGHT, &WIDTH, &NUM_OF_CHILD, &NUM_OF_LINES);
	for(int i=0; i<NUM_OF_CHILD; ++i)
	{
		GA.push_back(tmp);
		for(int j=0; j<NUM_OF_LINES*3; ++j)
		{
			int x;
			fscanf(txtin, "%d", &x);
			GA[i].push_back(x);
		}
	}
	fclose(txtin);
	return;
}
void drawLines(int num)
{
	int i, j, x1, x2, y1, y2, dx, dy, x, y, t;
	for(i=0; i<HEIGHT; ++i)
	{
		for(j=0; j<WIDTH; ++j)
		{
			temp.R[i][j] = temp.G[i][j] = temp.B[i][j] = 255;
		}
	}
	for(i=0; i<NUM_OF_LINES; ++i)
	{
		//drawing a line
		x1 = GA[num][3*i+0];
		y1 = GA[num][3*i+1];
		x2 = (int)(x1 + LENGTH_OF_LINE*cos(GA[num][3*i+2]));
		y2 = (int)(y1 + LENGTH_OF_LINE*sin(GA[num][3*i+2]));
		if(x1>x2)
        {
            t = x1; x1 = x2; x2 = t;
            t = y1; y1 = y2; y2 = t;
        }
        dx = (x2-x1);
        dy = (y2-y1);
        if(dx!=0)
        {
            for(x = x1; x <= x2; ++x)
            {
                y = y1 + dy*(x-x1)/dx;
                temp.R[x][y] -= 32; temp.R[x][y] = temp.R[x][y]<0?0:temp.R[x][y];
                temp.G[x][y] -= 32; temp.G[x][y] = temp.G[x][y]<0?0:temp.G[x][y];
                temp.B[x][y] -= 32; temp.B[x][y] = temp.B[x][y]<0?0:temp.B[x][y];
            }
        }
        if(y1>y2)
        {
            t = x1; x1 = x2; x2 = t;
            t = y1; y1 = y2; y2 = t;
        }
        dx = (x2-x1);
        dy = (y2-y1);
        if(dy!=0)
        {
            for(y = y1; y <= y2; ++y)
            {
                x = x1 + dx*(y-y1)/dy;
                temp.R[x][y] -= 32; temp.R[x][y] = temp.R[x][y]<0?0:temp.R[x][y];
                temp.G[x][y] -= 32; temp.G[x][y] = temp.G[x][y]<0?0:temp.G[x][y];
                temp.B[x][y] -= 32; temp.B[x][y] = temp.B[x][y]<0?0:temp.B[x][y];
            }
        }
	}
	return;
}
void make_Child()
{
    srand(time(NULL));
	int i, j;
	vector<int> tt(NUM_OF_LINES*3);
	//choose top 5
	//sort(GA.begin(), GA.end(), compGA);
	printf("Sorting... (%3d/%3d)", 0, NUM_OF_CHILD);
	for(i=NUM_OF_CHILD/2; i>=1; --i)
    {
        j = i;
        while(j<=NUM_OF_CHILD)
        {
            if(j*2+1<=NUM_OF_CHILD)
            {
                if(compGA(j*2 - 1, j*2+1 - 1))
                {
                    if(compGA(j - 1, j*2+1 - 1))
                    {
                        tt = GA[j - 1];
                        GA[j - 1] = GA[j*2+1 - 1];
                        GA[j*2+1 - 1] = tt;
                        j = j*2+1;
                    }
                    else break;
                }
                else
                {
                    if(compGA(j - 1, j*2 - 1))
                    {
                        tt = GA[j - 1];
                        GA[j - 1] = GA[j*2 - 1];
                        GA[j*2 - 1] = tt;
                        j = j*2;
                    }
                    else break;
                }
            }
            else if(j*2<=NUM_OF_CHILD)
            {
                if(compGA(j - 1, j*2 - 1))
                {
                    tt = GA[j - 1];
                    GA[j - 1] = GA[j*2 - 1];
                    GA[j*2 - 1] = tt;
                    j = j*2;
                }
                else break;
            }
            else break;
        }
    }
	for(i=NUM_OF_CHILD; i>=1; --i)
    {
        tt = GA[0];
        GA[0] = GA[i-1];
        GA[i-1] = tt;
        j = 1;
        while(j<=i-1)
        {
            if(j*2+1<=i-1)
            {
                if(compGA(j*2 - 1, j*2+1 - 1))
                {
                    if(compGA(j - 1, j*2+1 - 1))
                    {
                        tt = GA[j - 1];
                        GA[j - 1] = GA[j*2+1 - 1];
                        GA[j*2+1 - 1] = tt;
                        j = j*2+1;
                    }
                    else break;
                }
                else
                {
                    if(compGA(j - 1, j*2 - 1))
                    {
                        tt = GA[j - 1];
                        GA[j - 1] = GA[j*2 - 1];
                        GA[j*2 - 1] = tt;
                        j = j*2;
                    }
                    else break;
                }
            }
            else if(j*2<=i-1)
            {
                if(compGA(j - 1, j*2 - 1))
                {
                    tt = GA[j - 1];
                    GA[j - 1] = GA[j*2 - 1];
                    GA[j*2 - 1] = tt;
                    j = j*2;
                }
                else break;
            }
            else break;
        }
        printf("\b\b\b\b\b\b\b\b");
        printf("%3d/%3d)", NUM_OF_CHILD-i+1, NUM_OF_CHILD);
    }
    printf("\nSorting Complete\n");
	//draw the top one ("output_g{0}.bmp".(NUM_OF_GEN))
	drawLines(0);
	char filename[20];
	sprintf(filename, "output_g%d.bmp", NUM_OF_GEN);
	FILE *imgout = fopen(filename, "wb");
	for(i=0; i<54; ++i) putc(buffer[i],imgout);//fprintf(imgout, "%d", buffer[i]);
	for(i=0; i<HEIGHT; ++i)
	{
		for(j=0; j<WIDTH; ++j)
		{
			putc(temp.B[i][j],imgout);
			putc(temp.G[i][j],imgout);
			putc(temp.R[i][j],imgout);
		}
		if((WIDTH*3)%4==3){putc(0,imgout);}
		if((WIDTH*3)%4==2){putc(0,imgout);putc(0,imgout);}
		if((WIDTH*3)%4==1){putc(0,imgout);putc(0,imgout);putc(0,imgout);}
	}
	fclose(imgout);
	//make childs
	for(i=NUM_OF_SURVIVE; i<NUM_OF_CHILD; ++i)
    {
        int par1, par2;
        par1 = random(NUM_OF_SURVIVE);
        par2 = random(NUM_OF_SURVIVE);
        while(par2 == par1) par2 = random(NUM_OF_SURVIVE);
        //make new DNA
        int p = random(NUM_OF_LINES)*3;
        for(j=0; j<p; ++j) GA[i][j] = GA[par1][j];
        for(j=p; j<NUM_OF_LINES*3; ++j) GA[i][j] = GA[par2][j];
    }
	//mutate 2 childs
    for(i=NUM_OF_SURVIVE; i<NUM_OF_CHILD; ++i)
    {
        for(j=0; j<NUM_OF_LINES*3; ++j)
        {
            if(random(MUTATE_POS) == 0)
            {
                if(j%3 == 0) GA[i][j] = random(HEIGHT);
                if(j%3 == 1) GA[i][j] = random(WIDTH);
                if(j%3 == 2) GA[i][j] = random(360);
            }
        }
    }
	return;
}
int random(int number){ return rand()%number; }
bool compGA(int x, int y)
{
	return calcFitness(x) > calcFitness(y);
}
long long calcFitness(int num)//bigger value is more great
{
	drawLines(num);
	int i, j, tmp;
	long long result=0;
	for(i=0; i<HEIGHT; ++i)
	{
		for(j=0; j<WIDTH; ++j)
		{
		    tmp += abs(temp.R[i][j]-transImg[i][j])+abs(temp.G[i][j]-transImg[i][j])+abs(temp.B[i][j]-transImg[i][j]);
		    result -= (long long)tmp*(long long)tmp;
			//result /= 3;
		}
	}
	return result;
}
void trans_image()
{
	//trans img (input_img)
	int i, j;
	for(i=0; i<HEIGHT; ++i)
	{
		for(j=0; j<WIDTH; ++j)
		{
			int c=0;
			if(i>0)
			{
				++c;
				transImg[i][j]+=abs(input_img.R[i][j]-input_img.R[i-1][j]);
				transImg[i][j]+=abs(input_img.G[i][j]-input_img.G[i-1][j]);
				transImg[i][j]+=abs(input_img.B[i][j]-input_img.B[i-1][j]);
			}
			if(j>0)
			{
				++c;
				transImg[i][j]+=abs(input_img.R[i][j]-input_img.R[i][j-1]);
				transImg[i][j]+=abs(input_img.G[i][j]-input_img.G[i][j-1]);
				transImg[i][j]+=abs(input_img.B[i][j]-input_img.B[i][j-1]);
			}
			if(i<HEIGHT-1)
			{
				++c;
				transImg[i][j]+=abs(input_img.R[i][j]-input_img.R[i+1][j]);
				transImg[i][j]+=abs(input_img.G[i][j]-input_img.G[i+1][j]);
				transImg[i][j]+=abs(input_img.B[i][j]-input_img.B[i+1][j]);
			}
			if(j<WIDTH-1)
			{
				++c;
				transImg[i][j]+=abs(input_img.R[i][j]-input_img.R[i][j+1]);
				transImg[i][j]+=abs(input_img.G[i][j]-input_img.G[i][j+1]);
				transImg[i][j]+=abs(input_img.B[i][j]-input_img.B[i][j+1]);
			}
			transImg[i][j]/=3; transImg[i][j]/=c;
			transImg[i][j] = 255-transImg[i][j];
			transImg[i][j]=transImg[i][j]*transImg[i][j]/255*transImg[i][j]/255*transImg[i][j]/255*transImg[i][j]/255*transImg[i][j]/255*transImg[i][j]/255;
		}
	}
	return;
}
