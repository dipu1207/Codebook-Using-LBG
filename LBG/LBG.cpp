// vector_quantization.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdio.h>
#include<math.h>

// create a array of size 6340 by 12 and read the value of universe.txt in it
long double universeArray[6340][12];
long double codebook[8][12];
long double copyCodebook[8][12];
int regions[8][6340];//here we store indices of universe vectors according to k nearest neighbours
long double tokhuraWeight[12] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0}; //tokhura weights
long double oldDistortion=0.0;
int universe_size=6340;
int codebook_size=1;
long double epsilon=0.03;
long double delta=0.000001;
long double final_distortion;
int   region_size[8];//this array will store the no of elements we have in each row of regions array
FILE *outprint=fopen("distortions.txt","w");
FILE *code=fopen("codebook.txt","w");
//read universe
void readUniverse()
{
	char readvalue[20];
	long double charToDouble=0;
	
	//make a file pointer to read universe.txt
	FILE *readUniverse=fopen("universe.txt","r");
	if(!readUniverse)
	{	
		printf("cant read universe.txt\n");
	    return; 
  	}
     
	for(int i=0;i<universe_size;i++)
	    {
	      for(int j=0;j<12;j++)
		  {
		    fscanf(readUniverse,"%s",readvalue);
			charToDouble=atof(readvalue);
			universeArray[i][j]=charToDouble;
		  }
	    }
  
}

//fill initial codebook of size 8
void fillInitialcodebook()
{
	
	
	    long double tempArray[12];//to store centroid of universe array
	
		for(int row=0;row<12;row++)// set temp array to 0
		{tempArray[row]=0.0;}
	  
		  // fill temp array with universe
		for(int row=0;row<universe_size;row++)
	     {
	         for(int col=0;col<12;col++)
				tempArray[col]+=universeArray[row][col];
		}
		 //take avg of temp array and fill in codebook
		for(int avg=0;avg<12;avg++)
		{	
			tempArray[avg]=(long double)(tempArray[avg]/universe_size);
		    codebook[0][avg]=tempArray[avg];
		}

}


//function to calculate tokhura distance
long double calculateTokhura(long double tempUniverse[],long double tempCodebook[])
{
    long double distance=0.0;
	for(int i=0;i<12;i++)
		distance+=tokhuraWeight[i]*((tempUniverse[i] - tempCodebook[i])*(tempUniverse[i]- tempCodebook[i]));
		
	distance/=12;
	return distance;
}

void assign_regions()
{ 
   //initially set size_regions to 0 means the size of each row is 0
	for (int i=0;i<codebook_size;i++)
	    region_size[i]=0;
	
	long double tempUniverse[12];
	long double tempCodebook[12];
	
	
	int colno;
	
	for(int itrUni=0;itrUni< universe_size;itrUni++)//run of each vector of universe
	{
		
		long double currdistance=0.0;
		long double mindistance=16789.0; // random large no
		int index=-1;      // to store which region has been assigned to vector at index i of universe
		
		//here i call a function to calculate tokhura distane of each vector of universe with all vectors of codebook and the codevector with min distance will be the region
		//and i will increment count for that region array
		//first take universe vector at index i in a temp array
	    for(int itrUniverse=0;itrUniverse<12;itrUniverse++)
			tempUniverse[itrUniverse]=universeArray[itrUni][itrUniverse];

		//now i will run a loop 8 times and store one codebook vector each time and find the tokhura distance with the universe vactor
		for(int itrCode=0;itrCode<codebook_size;itrCode++)
		{  
			
		   for(int itrCodebook=0;itrCodebook<12;itrCodebook++)
			  tempCodebook[itrCodebook]=codebook[itrCode][itrCodebook];

		   //now call function to calculate tokhura distance between these two temporary variables and return the value in a variable
		   currdistance=calculateTokhura(tempUniverse,tempCodebook);
		   
		   if(mindistance>currdistance)
		   { 
			   mindistance=currdistance;
			   index=itrCode;   //we will need this to update the regions
		   }
		}

	
	
	//now here we  will put index of universe vector in the region which has row no as index
		colno=region_size[index];
		regions[index][colno]=itrUni;
		//increase size of region by 1 where this universe vector belongs
	     region_size[index]++;
	}//all vectors of universe are assigned

}
long double totalDistortion()
{
	long double avgDistortion=0.0;
	//here we will take all vectors of a region and find centroid and then update code book
	for(int itrCodebook=0;itrCodebook<codebook_size;itrCodebook++)
	{
		//store codebook vector in array
		long double codevector[12];
		for(int i=0;i<12;i++)
			{ 
				codevector[i]=codebook[itrCodebook][i];
		    }
	   //now read all vector whose index is in array region[itrCodebook];
		for(int col=0;col<region_size[itrCodebook];col++)
		{
			long double regionvector[12];
			for(int i=0;i<12;i++)
				regionvector[i]=universeArray[col][i];

			avgDistortion+=calculateTokhura(regionvector,codevector);

		}
		
	}
	
	
    avgDistortion/=universe_size;	
	return avgDistortion;
}


void update_codebook()
{
	 
  // here we take take centroid of each region and replace the code book
   long double centroid[12];
   int row;
   for(int i=0;i<codebook_size;i++) // runs for each region
   { 
	   //empty the centroid for each region
	   for(int j=0;j<12;j++)
	   {
		   centroid[j]=0.0;
	   }
	  
	   //this loop takes all vectors in one region
	   for(int k=0;k<region_size[i];k++)
	   {
		   row=regions[i][k];//this gives row of universe array
	      for(int fill=0;fill<12;fill++)
		  {
			  centroid[fill]+=universeArray[row][fill];
		  }
	   }
	   
       
	   //here we have computed centroid for one region now we take average
	   for(int p=0;p<12;p++)
		  { 
			  centroid[p]/=region_size[i];
	          codebook[i][p]=centroid[p];
	       }
      
   }
}

void clear_regions()
{
  for(int i=0;i<8;i++)
  { 
	  for(int j=0;j<universe_size;j++)
		  regions[i][j]=0;
    }
}


void Kmeans()
{  
	//make a count variable to check at which iteration my new and old distortion values are what
	int count=0;
	//here first old distortion value is set to 0 as we have not calculated the distortion yet
	oldDistortion=0.0;
	//now we assign the vectors to specific regions
    assign_regions();
	//find distortion
	long double currDistortion=totalDistortion();
	//write in a file the values of old and new distortion for count i
	
	fprintf(outprint,"\n\n\nfor codebook of size\t %d \n",codebook_size);
	 fprintf(outprint,"for count %d:->  old distortion is:  %lf  and new distortion is:  %lf  and difference  is:-> %lf\n",count,oldDistortion,currDistortion,abs(currDistortion-oldDistortion));
	//run until currDistortion - oldDistortion is less than delta
	
	while(abs(currDistortion-oldDistortion)>delta)
	{
		count++;
		//update codebook
		
		update_codebook();
		//clear the regions array to fill new values
	    clear_regions();
		 //now call assign regions to assign the regions of universe vectors according to new codebook
		assign_regions();
		 //set old distortion = curr distortion 
	   oldDistortion=currDistortion;
		 //now find new distortion
        currDistortion=totalDistortion();

		//write the values of old and new distortion in file 
		 fprintf(outprint,"for count %d:->  old distortion is:  %lf  and new distortion is:  %lf  and difference  is:-> %lf\n",count,oldDistortion,currDistortion,abs(currDistortion-oldDistortion));
	
	   }
	final_distortion=currDistortion;
	 fprintf(outprint,"\nfinal  distortion  D is :->  %lf  \n",currDistortion);
	
}

//function to double size of code book and fill it
void split()
{
	//first clear copyCodebook array
	for(int p=0;p<8;p++)
	{
		for(int q=0;q<12;q++)
			copyCodebook[p][q]=0.0;
	   }
	//fill codebook in copyCodebook
	for(int i=0;i<codebook_size;i++)
	{
	  for(int j=0;j<12;j++)
	  {
	     copyCodebook[i][j]=codebook[i][j];
	   }
	}
}

void fillCodebook()
{
	long double temp1[12],temp2[12];
	int row=0;
	for(int i=0;i<codebook_size;i++)
	{
	    for(int j=0;j<12;j++)
		{
		  temp1[j]=copyCodebook[i][j]*(1+epsilon);
		  temp2[j]=copyCodebook[i][j]*(1-epsilon);
		
		}
		//now fill the code book
		for(int k=0;k<12;k++)
		{
			codebook[row][k]=temp1[k];
		}
		row++;
		for(int k=0;k<12;k++)
		{
			codebook[row][k]=temp2[k];
		}
		row++;
	}
}
void LBG()
{
 
	
	
	while(codebook_size<=8)
	{
		fprintf(code,"for codebook size %d  ",codebook_size);
		
		//call k means
		Kmeans();
		
		//write codebook in a file
		 fprintf(code,"\n");
		for(int i=0;i<codebook_size;i++)
		{
		  for(int j=0;j<12;j++)
			   {
			      fprintf(code,"%lf ",codebook[i][j]);
			   }
			   fprintf(code,"\n");
			}
		 fprintf(code,"\n");
		
		
		if(codebook_size>=8)
		{
			 return;
		}
		
		
		//call split function 
	 split();
	
	//now we put new vactor in codebook from copyCodebook
	fillCodebook();
	codebook_size*=2;
	}
	return;
}

int _tmain(int argc, _TCHAR* argv[])
{

	//call function to read universe vector in an array
	readUniverse();

	//fill initial codebook of size 8 with  avg of jump no. of vectors 
	fillInitialcodebook();
	
	//now call function LBG
	  LBG();
	
	printf("final distortion using LBG is \t %lf\n",final_distortion);

	 printf("final codebook of size 8 using LBG\n");
	for(int i=0;i<codebook_size;i++)
	{
	 for(int j=0;j<12;j++)
	   printf("%lf ",codebook[i][j]);
	 printf("\n");
	
	}
	 
	
	return 0;
}

