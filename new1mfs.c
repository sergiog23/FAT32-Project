
//  Created by Sergio Guerrero on 3/20/19.
//  Copyright © 2019 Sergio Guerrero. All rights reserved.
//
/*
 Sergio Guerrero
 1001398730
 */

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <ctype.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments
//int open1 =0; //global variables to check if file is open or closed.
//int close1 =0;
FILE *fp;
struct __attribute__((__packed__)) DirectoryEntry {
    char        DIR_Name[11];
    uint8_t     DIR_Attr;
    uint8_t     Unused1[8];
    uint16_t    DIR_FirstClusterHigh;
    uint8_t     Unused2[4];
    uint16_t    DIR_FirstClusterLow;
    uint32_t    DIR_FileSize;
};
struct DirectoryEntry dir[16];

//BPB(BIOS Parameter Block) boot sector the first sector of the volume
    //FAT 32 LAYOUT
    char    BS_OEMName[8];// doen
    int16_t BPB_BytesPerSec; // done
    int8_t  BPB_SecPerClus;// done
    int16_t BPB_RsvdSecCnt;//dne
    int8_t  BPB_NumFATs;//done
    int16_t BOB_RootEntCnt;//
    char    BS_VolLab[11];
    int32_t BPB_FATSz32;//done
    int32_t BPB_RootClus;//
    //initialize to 0 according to pdf
    int32_t RootDirSectors;
    int32_t FirstDataSector;
    int32_t FirstSectorofCluster;
    //root directory is at the first clusrer
    int root;
    

/*FILE *open_file(char *filename,struct FAT32_Layout *, struct DirectoryEntry *dir
{
    FILE *fp;
    int i;
    if((fp=fopen(filename,"r"))==NULL)
    {
        printf("Error: File system image not found.");
        exit(1);
    }
    //Defaults
    open1=1;
    close1=1;
    RootDirSectors = 0;
    FirstDataSector = 0;
    FirstSectorofCluster =  0;
    
    //BPB_BytesOerSec 2bytes
    fseek(fp,11,0);
    fread(&BPB_BytesPerSec,2,1,fp);
    //BPB_RsvdSecCNT 2bytes
    fseek(fp,14,0);
    fread(&BPB_RsvdSecCnt,2,1,fp);
    //BPB_SecPerClus 1 byte
    fseek(fp,13,0);
    fread(&BPB_SecPerClus,1,1,fp);
    //BPB_NumFATS  1 byte
    fseek(fp, 16,0);
    fread(&BPB_NumFATs,1,1,fp);
    //BPB_RootEntCnt 2 byte
    fseek(fp,16,0);
    fread(&BOB_RootEntCnt,2,1,fp);
    //BPB_Fatsz32 4 bytes
    fseek(fp,36,0);
    fread(&IBPB_FATSz32,4,1,fp);
    //BPB_RootClus
    fseek(fp,44,0);
    fread(&BPB_RootClus, 4, 1, fp);
    //BS_OEMName
    fseek(fp,3,0);
    fread(&BS_OEMName, 8, 1, fp);
    //BS_VolLab
    fseek(fp,71,0);
    fread(&BS_VolLab, 11, 1, fp);
    //printf("\n%d\n",IMG->BPB_RootClus);
    int root =(BPB_NumFATs *BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt *BPB_BytesPerSec);
    //printf("%d\n",IMG->root);
    
    //root directory info = feek into root and loop from i=0 to i<16
    //fread 32 bytes into directory entry array
    fseek(fp,root,0);
    for(i=0;i<16;i++)
    {
        fread(&DirEnt[i], 32, 1,fp);
    }
    
    
    return fp;
}*/

int LBAToOffset(int32_t sector)
{
    return((sector-2) * BPB_BytesPerSec) +(BPB_BytesPerSec * BPB_RsvdSecCnt)+(BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec);
}
int16_t NextLB(uint32_t sector)
{
    uint32_t FATAddress=(BPB_BytesPerSec * BPB_RsvdSecCnt) + (sector *4);
    int16_t val;
    fseek(fp, FATAddress,0);
    fread(&val,2,1,fp);
    return val;
    
}
char* formatString(char *input){
    
  //  char *toFat;
    char copyUser[strlen(input)];
    strcpy(copyUser,input);
    
    char IMG_Name[11] = "FOO     TXT";
    
    // char input = "foo.txt";
    
    static char expanded_name[12];
    
   // strcpy(expanded_name,copyUser);
    
    memset( expanded_name, ' ', 12 );
    
    char *token = strtok( copyUser, "." );
    
    strncpy(expanded_name, token, strlen( token ) );
    
    token = strtok( NULL, "." );
    
    if( token )
    {
        strncpy( (char*)(expanded_name+8), token, strlen(token ) );
    }
    
    expanded_name[11] = '\0';
    
    int i;
    for( i = 0; i < 11; i++ )
    {
        expanded_name[i] = toupper( expanded_name[i] );
    }
    
  /*  if( strncmp( expanded_name, IMG_Name, 11 ) == 0 )
    {
        printf("They matched\n");
    }*/
    return expanded_name;
}
int main()
{
    
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    FILE *fp=NULL;
    
    while( 1 )
    {
        // Print out the mfs prompt
        printf ("mfs> ");
        
        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
        
        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        
        int   token_count = 0;
        
        // Pointer to point to the token
        // parsed by strsep
        char *arg_ptr;
        
        char *working_str  = strdup( cmd_str );
        
        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;
        
        // Tokenize the input stringswith whitespace used as the delimiter
        while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
               (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
            if( strlen( token[token_count] ) == 0 )
            {
                token[token_count] = NULL;
            }
            token_count++;
        }
     if(token[0]==NULL) continue;    // Now print the tokenized input as a debug check
        // \TODO Remove this code and replace with your shell functionality
        if (strcmp((token[0]),"quit")==0)
        {
            exit(EXIT_SUCCESS);
        }
        else if (strcmp((token[0]),"exit")==0)
        {
            exit(EXIT_SUCCESS);
        }
        
      //  int token_index  = 0;
        
       if (strcmp((token[0]),"open")==0)
        {
            
            /*if(open1==1)
            {
                printf("Error:File system image is already opened.\n");
            }
            if(close1==0)
            fp=open_file(token[1], fat32, dirE);
            */

          
        int i;
        if((fp=fopen(token[1],"r"))==NULL)
     {
         printf("Error: File system image not found.");
        exit(1);
     }
    //Defaults
    //open1=1;
        //close1=1;
         RootDirSectors = 0;
        FirstDataSector = 0;
        FirstSectorofCluster =  0;
    
        //BPB_BytesOerSec 2bytes
        fseek(fp,11,0);
        fread(&BPB_BytesPerSec,2,1,fp);
        //BPB_RsvdSecCNT 2bytes
        fseek(fp,14,0);
        fread(&BPB_RsvdSecCnt,2,1,fp);
        //BPB_SecPerClus 1 byte
        fseek(fp,13,0);
        fread(&BPB_SecPerClus,1,1,fp);
        //BPB_NumFATS  1 byte
        fseek(fp, 16,0);
        fread(&BPB_NumFATs,1,1,fp);
        //BPB_RootEntCnt 2 byte
        fseek(fp,16,0);
        fread(&BOB_RootEntCnt,2,1,fp);
        //BPB_Fatsz32 4 bytes
        fseek(fp,36,0);
        fread(&BPB_FATSz32,4,1,fp);
        //BPB_RootClus
        fseek(fp,44,0);
        fread(&BPB_RootClus, 4, 1, fp);
        //BS_OEMName
        fseek(fp,3,0);
        fread(&BS_OEMName, 8, 1, fp);
        //BS_VolLab
        fseek(fp,71,0);
        fread(&BS_VolLab, 11, 1, fp);
        //printf("\n%d\n",IMG->BPB_RootClus);
        int root =(BPB_NumFATs *BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt *BPB_BytesPerSec);
     
    
    //root directory info = feek into root and loop from i=0 to i<16
    //fread 32 bytes into directory entry array
    fseek(fp,root,0);
    for(i=0;i<16;i++)
    {
        fread(&dir[i], 32, 1,fp);
    }
        }
        if (strcmp((token[0]),"close")==0)
        {
          //  if(open1==0)
            
                printf("Error: File system not open.\n");
            
           /* if(open1==1)
            {
                //fclose(fp);
                //open1=0;
              //  close1=0;
                
            }*/
            
        }
        if (strcmp((token[0]),"stat")==0)
        {
            int i=0,idx;
            char* converted=NULL;
            converted=formatString(token[1]);
            for(i=0;i<16;i++)
            {
                //yes=strncmp(converted,dir[i].DIR_Name,11);
                char name[12];
                strncpy(name,dir[i].DIR_Name,11);
                if((dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20 || dir[i].DIR_Name[0] == '.') )
                {
                    if(strcasecmp(name,converted)==0) idx=i;
                }          
            }
                
                        printf("Attribute       Size     Cluster\n");
                        printf("%d              %d       %d\n",dir[idx].DIR_Attr,dir[idx].DIR_FileSize,dir[idx].DIR_FirstClusterLow);
        
            
        }
        if (strcmp((token[0]),"info")==0)
        {
            printf("BPB_BytesPerSec:HEX: %2x  DEC: %2d\n",BPB_BytesPerSec,BPB_BytesPerSec);
            printf("BPB_SecPerClus: HEX: %x    DEC: %d\n",BPB_SecPerClus,BPB_SecPerClus);
            printf("BPB_RsvdSecCnt: HEX: %2x   DEC: %2d\n",BPB_RsvdSecCnt,BPB_RsvdSecCnt);
            printf("BPB_NumFATS:    HEX: %x    DEC: %d\n",BPB_NumFATs,BPB_NumFATs);
            printf("BPB_FATSz32:    HEX: %2x  DEC: %2d\n",BPB_FATSz32,BPB_FATSz32);


            
        }
        if (strcmp((token[0]),"ls")==0)
        {
            int i;
            char dirname[12];

           // fseek(fp,0x100400,0);
            //fread(&dir[0],16,(sizeof(struct DirectoryEntry)),fp);

          for(i=0;i<16;i++){
              if( (dir[i].DIR_Attr == 0x1 || dir[i].DIR_Attr == 0x10 ||  dir[i].DIR_Attr == 0x20))
              {
              strncpy(dirname,dir[i].DIR_Name,11);
              if(dirname[0]!=0)
              printf("%s\n",dirname);
              }
          }   
            
        }

        if (strcmp((token[0]),"cd")==0)
        {
        char *converted=NULL;
        char dirname[12];
        int i,p;
        converted=formatString(token[1]);
        //int cluster=dir[1].DIR_FirstClusterLow;
        int cluster=6099;
        //int clusterFolderB=

        int addr=LBAToOffset(cluster);
        
     
       for(i=0;i<16;i++){
             //if strncmp
              if( (dir[i].DIR_Attr == 0x1 || dir[i].DIR_Attr == 0x10 ||  dir[i].DIR_Attr == 0x20))
              {
              strncpy(dirname,dir[i].DIR_Name,11);
                 // printf("\n%s\n",dirname);
                  fseek(fp,addr,0);
                  fread(&dir[0],(sizeof(struct DirectoryEntry)),16,fp);
              //if(dirname[0]!=0)
                //  p=1;
              //printf("%s\n",dirname);
              }}
        
        }
        if (strcmp((token[0]),"get")==0)
        {
            
            
            
        }
        if (strcmp((token[0]),"put")==0)
        {
            
            
        }
        if (strcmp((token[0]),"read")==0)
        {
            
            
        }
        
        
        
        /*
        for( token_index = 0; token_index < token_count; token_index ++ )
        {
            printf("token[%d] = %s\n", token_index, token[token_index] );
         }*/
        
        free( working_root );
        
    
  //
}
    //treturn 0;
}

