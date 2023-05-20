/*
 *
 *
 * Project 2
 *
 *
 *
 */


#include <stdio.h>		//inporting stdio.h to use scanf() and printf() functions
#include <string.h>		//importing string.h to use strcmp() function
#include <stdlib.h>		//importing stdlib.h to use malloc() and free() functions

typedef struct _{               //declare structure to store name, wanted parameter and reference to next node to create a linked list

	char name[48];		//char array to store name		
        int quantity;		//integar to store wanted parameter
	int preci;		//integar to get order of inputs
	struct _* next;		//reference to store next node 
}data;                          //name the structure as "data"

data * arr[27];			//declare global struct reference array to arrange the inputs acording to the alphebtical order(some entires may go uninitialized)
int prec = 0;			//declare global integar to keep account of input order
data * inputs = NULL;		//declare global reference to a node to store head of the linked list
int itemCount = 0;              //declare global integar to store length of linked list

int maxNameLen(int);		//function calculate maximum name length. Intakes integar, number of elements to be printed
int readFile(FILE *,char);	//function to read given file. Intakes pointer to file and wanted parameter as a char.(Meetings-m, Participants-p, Time-'t')
int timeCal(char *);		//function to give time as minutes
int parti(char *,int);		//function to give participants number as integar.
void sort();			//function to sort list  according to wanted paramter.
int StringtoInt(char*);  	//function to turn the string to integar

void plotData(int,int);			//function to print the chart. Intakes integars. The list length and variable "s" to see the scaled graph is needed.
void printBlocks(data*,int,int);   	//function to print the blocks of the chart. Intakes reference node.

void usage(char *);			//prints usage message. Intakes first argument given in the command line.

int main(int argc,char ** argv){
	
	for(int i=0;i<27;i++){		//set all entries of global array to NULL
		arr[i] = NULL;
	}

	int flag=-1;		//integar flag to notify if a command -m/-p/-t was given and if so print warning message
	char cmd='m';		//char to get wanted command -m/-p-/-t. Default set to m
	int listLen=0;		//integar to get the length of chart needed
	int s=0;		//integar to see whether scaled chart is needed.
	int filecount=0;	//integar to keep track of how many files are given

	static char prog[20];	//string to pass program name
	strcpy(prog,argv[0]);	//copy program name from arguments to string created

	int strLen=0;		//to assign string lengths
	int newListLen=0;       //to assign to the value given after "-l"
	FILE *fil = NULL;     	//pointer for files

	//-----------------------------------------
//--------------------ERROR HANDLING---------------------------------------------------------------------------------------------------------------------------------
	//------------------------------------------
	
	if(argc==1){		//if no arguments are given exceptt for program file print error message and close program
		printf("No input files were given\n");
                usage(prog);
                return 0;
	}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//ERROR HANDLING (1) - HANDLING FLAGS

	for(int i=1;i<argc;i++){	//for loop to check for invalid flags

		if(argv[i][0]=='-'){	//if invalid flag is given print error message and close program
                        if((strcmp(argv[i],"-m")!=0)&&(strcmp(argv[i],"-t")!=0)&&(strcmp(argv[i],"-p")!=0)&&(strcmp(argv[i],"-l")!=0)&&(strcmp(argv[i],"--scaled")!=0)){
                                printf("Invalid option [%s]\n",argv[i]);
                                usage(prog);
                                return 0;
                        }
                }
						
		if(strcmp(argv[i],"-l")==0){	//if "-l" flag is given

               	        newListLen=0;	//to assign to the value given after "-l"

                       	if(i+1==argc){		//if there aren't enouh arguments, print error message and exit
                           	printf("Not enough options for [-l]\n");
                              	usage(prog);
                             	return 0;
                    	}
			
			if(argv[i+1][0]=='-'){					//if value given after "-l" is negative

				for(int k=1;argv[i+1][k]!='\0';k++){		//going tthrought argument given after"-l"

                               		if((argv[i+1][k]<47)||(argv[i+1][k]>58)){	//if it consists of anything other than digits print error message and exit program

                                	       	printf("Invalid options for [-l]\n");
                                        	usage(prog);
                                        	return 0;
                                	}
                        	}
                           	printf("Invalid option(negative) for [-l]\n");	//print error message for negative value and exit program
                               	usage(prog);
                              	return 0;
                       	}

                    	for(int k=0;argv[i+1][k]!='\0';k++){					//for loop to extract the number given after "-l"
				
				if((argv[i+1][k]<47)||(argv[i+1][k]>58)){			//if anything other than digits are given exit program
                                	printf("Invalid options for [-l]\n");
                                     	usage(prog);
                                 	return 0;
                              	}
                       	}	

			newListLen = StringtoInt(argv[i+1]);		//get given length

                  	if(newListLen==0){					//if it is zero exitt program
                           	return 0;
                    	}

			if(newListLen<0){							//if value is negative
				printf("Invalid options(negative) for [-l]\n");  //print error message for negative value and exit program
                                usage(prog);
                                return 0;
			}
	
                    	listLen=newListLen;		//assign latest extracted "-l" argument to main listCount
          	}
	}
	
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//ERROR HANDLING(2) - LOOKING FOR REQUIRED PARAMETER TO PRINT
	
	for(int i=0;i<argc;i++){		//for loop to iterate over all arguments
		
		if((strcmp(argv[i],"-m")==0)||(strcmp(argv[i],"-t")==0)||(strcmp(argv[i],"-p")==0)){		//if valid parameter flag is given

               		if(flag<0){			//if a parameter flag was not given before assign it to the "cmd" which stores the character
                          	cmd = argv[i][1];
                         	flag=0;

                  	}else if((flag==0)&&((cmd)!=(argv[i][1]))){	//if a parameter flag was given before and if it is not the same paramter flag as before print error message and close program
                       		printf("Cannot plot multiple parameters in same graph.\n");
                		usage(prog);
                		return 0;
                    	}
             	}
	}
	
	//SEEING WHETHER SCALED CHART IS NEEDED
	
	for(int i=0;i<argc;i++){	//go through every argument to see whether "--scaled" argument is given

		if(strcmp(argv[i],"--scaled")==0){		//if so set "s" to 1
                 	s=1;
             	}
	}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//ERROR HANDLING(3) - LOOKING INVALID DATA FILES

	for(int i=1;i<argc;i++){		//going over the arguments(excpet the program) to get files given 

		if((argv[i][0]!='-')&&(strcmp(argv[i-1],"-l")!=0)){	//anything without "-" is considered a file

			strLen = strlen(argv[i]);		//get string length of argument		
	
                        if((argv[i][strLen-1]!='v')||(argv[i][strLen-2]!='s')||(argv[i][strLen-3]!='c')||(argv[i][strLen-4]!='.')){		//if the file type is not csv print error message and exit
                                printf("Only .csv files should be given as inputs.\n");
                                return 0;
                        }
	
                        if((fopen(argv[i],"r")==NULL)){			//if file cannot be opened print error message and exit
                                printf("Cannot open one or more given files\n");
                                return 0;
                        }
			
			fil = fopen(argv[i],"r");
				
			if(readFile(fil,cmd)==-1){		//feed the data to the readFile() function
				printf("File/s contain wrong entries.\n");	//if it return -1 print error message and exit
				return 0;
			}

			fclose(fil);

                        filecount++;	//filecount to keep track of how many files are given
                }
	}
	
	if(listLen==0){			//if no length is specified set to default 10
		listLen=10;
	}

	if(filecount==0){		//if no files are fed print erro message and exit
		printf("No input files were given\n");
		usage(prog);
		return 0;
	}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//IF ARGUMENTS ARE GIVEN CORRECTLY SORT AND PLOT
		
	sort();		//sort the linked list using sort()

	if(listLen>itemCount){	//if given length to be printed is more than the itemcount set the length to be printed to itemcount
		listLen=itemCount;		
	}

        plotData(listLen,s);	//plot the data using plotData()

	return 0;		//end of program
}

void usage(char* prog){		//prints usage message
	printf("usage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n",prog);
}


int readFile(FILE * fileP, char cmd){	//function to read given file. Intakes pointer to file and wanted parameter as a char.(Meetings-m, Participants-p, Time-'t')

	int i=0;                        //declare integar i to go over read[] which includes one line of the file
       	int flag=-1;                    //declare integar flag=-1 to notify whether the read name is already in the item[] array
       	int quantity=0;                 //get wanted parameter
	int index =0;			//declare integar to keep track of index in global array
	char read[96];          	//declare array to get one line of the file
	char tempName[48];              //declare array tempName[] to extract  name from the file

        while(fgets(read,96,fileP)!=NULL){     //while loop to go through file line after line	
				
                i=0;                  	//set integar i to 0 
        	flag=-1;            	//set integar flag=-1 to notify whether the read name is already in the item[] array             
		tempName[0] = '\0';   	//set array tempName[] to NULL to extract  name from the file
		quantity=0;		//set qauntity to get wanted parameter
		index = 0;		//set index to 0

                for(;read[i]!=',';i++){         //for loop to get name
                        tempName[i]= read[i];   //store each character in tempName[]
                }
		
                tempName[i]='\0';               //make tempName[] a string by adding terminating character at the end

		if(i==0){			//if name is empty return -1 to print error message
			return -1;
		}

		if(parti(read,i)==-1){		//if participants are given incorrectly return -1 to print error message
			return -1;
		}
		
		if(cmd=='m'){			//if command is '-m' add one to quantity		
			quantity=1;
		}else if(cmd=='p'){		//if command is '-p' run parti() function to extract number of participants
			quantity = parti(read,i);
		}else{
			quantity = timeCal(read);	//if command is '-t' run time() funciton to get extract time in minutes
		}	

		if(quantity==-1){		//if -1 was returned from either of functions return -1 to print error message
                   	return -1;
               	}

		if((tempName[0]>64)&&(tempName[0]<91)){			//if first letter is capital set index to according to the letter by deducting ascii of 'A'
			index = tempName[0] - 'A';	
		}else if((96<tempName[0])&&(123>tempName[0])){		//if fisrt letter is in lowercase set index accodring to the letter by deducting ascii of 'a'
			index = tempName[0] - 'a';
		}else{							//if starting with different character make index 26
			index = 26;
		}

		if(arr[index]==NULL){					//if array slot is empty

			data * current = (data*)malloc(sizeof(data));	//make a node 
			arr[index] = current;				//assign it to the relevant array position
			strcpy(current->name,tempName);			//set name
			current->quantity = quantity;			//set amount
			current->preci = prec;				//set precedence(order)
			prec++;						//increase value of precedence
			itemCount++;

			current->next = NULL;				//set next node pointer to NULL

		}else{										//if a linked list has been created for that letter					 			
			data * current, * prev=NULL;						//declare node pointers

			for(current=arr[index];current!=NULL;current=current->next){		//go through linked list
				if(strcmp(tempName,current->name)==0){				//if the name already exists add the quantity and break
					current->quantity = current->quantity+quantity;
					flag=1;
					break;
				}
				prev = current;		//prev used for follow up node
			}

			if(flag<0){						//if i=name was not found create a new node

				data * temp= (data*)malloc(sizeof(data));	//reserve memeory dynamically
				strcpy(temp->name,tempName);			//set name
				temp->quantity = quantity;			//set quantity
				temp->preci = prec;				//set order
				prec++;						//increase precedence value
				temp->next = NULL;				//next pointer set to NULL
				prev->next = temp;				//follow up nodese to current node
				itemCount++;
			}	
		}
       	}
	return 0;		//end of function
}


void sort(){					//function to sort list  according to wanted paramter
	
	//first join all linked lists
	
	data * current = NULL;			//create pointer to go throughtt the linked lists
	int i =0;				//integar to go through the main array

	for(i=0;i<26;i++){			//for loop to go through the array

                if((arr[i]!=NULL)&&inputs==NULL){	//if list exists
			
			inputs = arr[i];		//get head node
			for(current=inputs;current->next!=NULL;current=current->next){	//get to the end of the linked list
			}

                }else if(arr[i]!=NULL){
			current->next = arr[i];
			for(current=arr[i];current->next!=NULL;current=current->next){
                        }
		}
	}
	
	//SORTING
	
	int done=0;	//declare integar to see whether sorting is done
      	data * prev = NULL;	//additional node pointer to follow up on the current node

	while(!done){		//while loop top loop till sorting is done
			
		prev = NULL;	//set prev to NULL
		done = 1;	//set done to 1

		for(current = inputs;current->next!=NULL;current=current->next){		//for loop to iterate over the linked list
		
			if(((current->quantity)<(current->next->quantity))||((current->quantity==current->next->quantity)&&(current->preci>current->next->preci))){	//if there is a need to be a shift of nodes

				done = 0;		//make done 0 so to run while loop again

				if(prev==NULL){		//the start of the list

					inputs = current->next;		//set input to the next node
					current->next = inputs->next;	//switch the nodes
					inputs->next = current;		

					current = inputs;		

				}else{	
					prev->next = current->next;		//switch the nodes
					current->next = prev->next->next;
					prev->next->next = current;
					current = prev->next;
				}
			}	
			prev = current;			//follow up node
		}
	}
}

int parti(char * read,int i){

        int ini = i;                    //save first i value
        int l=0;                        //to manipulate size for array participants

        for(i++;read[i]!=',';i++){      //2) for loop to get length needed for array to store string
                l++;
        }

        char * tempParti = (char*)malloc(sizeof(char)*(l+1));   //dynamically declare array

        l=0;    //set l to zero

        for(ini++;read[ini]!=',';ini++){        //2) for loop to get no. of participants
                tempParti[l]=read[ini];         //store each character i//function to turn the string to integarn tempParti[]
                l++;
        }

        tempParti[l] = '\0';    //make it a string

        if(strlen(tempParti)==0){       //if it is zero return -1
                return -1;
        }

        int part = StringtoInt(tempParti);      //declare intagre to store integare value of participants
        free(tempParti);                        //free the memory

        return part;                            //return integar
}

int timeCal(char * read){

        int t=0;                        //decalre integar to get go throught the string array
        int tenth=1;                    //declare integar tenth=1 to multiply by 10 so to use when converting number from string to integar
        int dots = 0;                   //declare integar to get see whether there are minutes or hour part of the string
        int comma=0;                    //declare integar to skip name, participants of the line

        for(int i=0;read[i]!='\0';i++){ //for loop to get length for array to store time
                if((comma==2)){
                        t++;
                }else if(read[i]==','){
                        comma++;
                }
        }

        char * tempTime = (char*)malloc(sizeof(char)*(t+1));    //declare array dynamically to store the time string

        t=0;            //set to to go over the time string
        comma=0;        //set comma to zero

        for(int i=0;read[i]!='\0';i++){         //for loop to go over  the sentence and get time string
                if((comma==2)){
                        tempTime[t]=read[i];
                        t++;
                }else if(read[i]==','){
                        comma++;
                }
        }

        tempTime[t] = '\0';             //make the array a string
        int mins=0;                     //declar eintegar to get minutes

        for(int i=strlen(tempTime);i>=0;i--){           //go over the time string

                if(tempTime[i]==':'){                   //whether to see hour or minute part is needed
                        dots++;
                        tenth = 1;
                }
                else if(dots==2){                                       //hour part of the time string.use tenth to add to minutes
                        mins += tenth*((tempTime[i]-'0')*60);
                        tenth = tenth*10;
                }else if(dots==1){                                      //minute part of the time string
                        mins += (tenth * (tempTime[i]-'0'));
                        tenth = tenth*10;
                }
        }

        free(tempTime);                 //free the array
        return mins;                    //return the munber of minutes
}

int maxNameLen(int listLen){				//function calculate maximum name length. Intakes integar, number of elements to be printed

	int maxLength = 0;				//declare variable to store maximum length
	data * current = inputs;			//declare node pointer to go over the linked list
	int length;					//temporary inetager to store each name length and compare with the maximum length

	for(int l=0;l<listLen;l++){			//for loop to go through the linked list but only till the required plot length
	
		length = strlen(current->name);		//get name length

		if(length>maxLength){				//if name length is longer than the current set maximum length replace it
			maxLength = length;
		}
		current = current->next;			//move to the next node using "current"
	}
	return maxLength;					//return maximum name length
}
	

void plotData(int listLen,int s){		//function to print the chart. Intakes integars. The list length and variable "s" to see the scaled graph is needed.
	
	int total = 0;				//declare integar to get total amount of wanted paramter
	int maxLength = maxNameLen(listLen);	//get maximum name length
	data * current;				//declare node pointer to go over the linked list

	for(current=inputs;current!=NULL;current=current->next){	//go over the linked list
		total += current->quantity;				//get total by adding all the values in the nodes
	}

	int tempMax = inputs->quantity;		//get the value of the top node to get its the digits length
	int numLength = 0;			//declare integar to get legnth of digits

       	for(int k=0;tempMax!=0;k++){		//for loop to iterate over the tempMax to get its digit length
             	tempMax=tempMax/10;
               	numLength++;
       	}
	
	current = inputs;		//start from the top node to start plotting
	float blocks;			//declare float to calculate wanted amount of block to be printed
	
	printf("\n");			//newline to start plot

	for(int i=0;i<listLen;i++){	//for loop to iterate over the node till required length
			
		//assign number of blocks needed according to scaled or normal
		if(s){		//if scaled graph is needed

                	blocks = ((float)current->quantity)*((float)(80-maxLength-3-numLength))/(float)inputs->quantity;

		}else{		//if normal graph is needed		

                	blocks = ((float)(current->quantity))*((float)(80-maxLength-3-numLength))/((float)total);
        	}
		
           	printBlocks(current,blocks,maxLength);		//use printBlocks() function to print the number of blocks
		current = current->next;			//move tot he next node
       	}
	
	//bottom part of the  plot
	for(int i=0;i<=maxLength+1;i++){			//use blanks in a for loop to move to wanted place. That is maximum name length plus two spaces.
                printf(" ");
        }
	
	printf("\u2514");			//print vertical line

	for(int i=0;i<80-maxLength-3;i++){		//for loop to print horizontal lines at the bottom
		printf("\u2500");
	}
	printf("\n");					//end plotting by printing new line
}


void printBlocks(data * current,int blocks,int maxLength){	//function to print the chart. Intakes reference node.
	
	for(int l=1;l<=3;l++){				//for loop which runs thrice to print the blocks. The middle l=2 prints the name as well.
		
		if(l==1){					
			for(int i=0;i<=maxLength+1;i++){		//use blanks in a for loop to move to wanted place. That is maximum name length plus two spaces.
         	       		printf(" ");
        		}

			printf("\u2502");               //print the vertical line

			for(int k=0;k<blocks;k++){      //print the blocks in one row
                        	printf("\u2591");
                	}

		}else if(l==2){                               //in the middle of loop the name needs to be printed, the blocks followed by the quantity
                        printf(" %s ",current->name);                    //print name
                      		
			for(int i=0;i<maxLength-(strlen(current->name));i++){
                                printf(" ");
                        }
			
			printf("\u2502");               //print the vertical line

			for(int k=0;k<blocks;k++){      //print the blocks in one row
                        	printf("\u2591");
                	}

                        printf("%d",current->quantity);                 //print the quantity
                }else if(l==3){
			for(int i=0;i<=maxLength+1;i++){		//use blanks in a for loop to move to wanted place. That is maximum name length plus two spaces.
                                printf(" ");
                        }
			printf("\u2502");               //print the vertical line

			for(int k=0;k<blocks;k++){      //print the blocks in one row
                       		printf("\u2591");
                	}
		}

		printf("\n");			//move to new line
	}
						
	for(int i=0;i<=maxLength+1;i++){	//use blanks in a for loop to move to wanted place. That is maximum name length plus two spaces.
             	printf(" ");
       	}

	printf("\u2502");			//print another single vertical between the data

	printf("\n");		

}

int StringtoInt(char * str){		//function to turn the string to integar
		
	int intValue = 0;		//to return the value
	int strLen = 0;			//to get string length		
	int tenth = 1;			//use tenth to go throughtt the basees of ten

	strLen = strlen(str)-1;		//get string value. reduce one to get index

	for(int i=strLen;i>=0;i--){	// going over the string
		intValue += tenth*(str[i]-'0');		//add numbr according to base of ten
		tenth = tenth*10;			//multiplyin ny ten 
	}

	return intValue;				//return integar
}
