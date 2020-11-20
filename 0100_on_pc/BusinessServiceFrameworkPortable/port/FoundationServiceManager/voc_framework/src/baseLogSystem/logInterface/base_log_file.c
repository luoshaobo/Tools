/////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file base_log_file.c
//  This file is about how log function implemenation

// @project     GLY_TCAM
// @subsystem   Application
// @author      Zhou You
// @Init date   26-Oct-2018
///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdarg.h>
#include "logInterface.h"
#include "baseLogFile.h"


int listFileAndDel(char *filePath,int bkNum)
{
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(filePath);
    char allFiles[20][100] = {""};
    char absPath[100] = "";
    long int logDateArry[20] = {0};
    long int logDate = 0;
    char dateNum[20] = "";
    unsigned int logCount = 0;
    long int temp = 0;
    int pMin = 0,delCot = 0;
    int i = 0,j = 0,k = 0,m = 0,n = 0;
    char * IsNullStr = NULL;
    int isAvalb = access(filePath,F_OK);
    if(isAvalb != 0)
    {
        mkdir(filePath,0666);
        return 0;
    }
    while(NULL != (ent=readdir(pDir)))
    {
//        printf("others [%s]\n", ent->d_name);
        strcpy(allFiles[i], ent->d_name);
        i++;
    }
    closedir(pDir);
    for(i=0;i < 20;i++)
    {
        IsNullStr = strstr(allFiles[i],".log");
        if(IsNullStr)
        {
            memset(dateNum,0,20);
            j = 0;
            while(allFiles[i][j]!='\0')
            {
                m = 0;
                if(allFiles[i][j]=='~')
                {
                    k = j + 1;
                    while(allFiles[i][k] >= '0' && allFiles[i][k] <= '9')
                    {
                        dateNum[m++] = allFiles[i][k];
                        k++;
                    }
                    logDateArry[n++] = atoi(dateNum);
                }
                j++;
            }
            logCount++;
        }
    }
    if(logCount < bkNum)
        return 0;
    else{
        for(i=0;i<20;i++)
        {
            pMin = i;
            for(j=i;j<19;j++)
            {
                if(logDateArry[j] < logDateArry[pMin])
                    pMin = j;
            }
            temp = logDateArry[i];
            logDateArry[i] = logDateArry[pMin];
            logDateArry[pMin] = temp;
        }
        delCot = n - bkNum;
        for(i=0;i<20;i++)
        {
            if(delCot <= 0)
                break;
            memset(dateNum,0,20);
            if(logDateArry[i]!=0)
            {
                logDate = logDateArry[i];
                sprintf(dateNum,"%ld",logDate);
                for(j=0;j<20;j++)
                {    
                    IsNullStr = strstr(allFiles[j],dateNum);
                    if(IsNullStr)
                    {
                        sprintf(absPath,"%s%s%s",filePath,"/",allFiles[j]);
                        if(remove(absPath)==0)
                        {
                            delCot--;
                            break;
                        }else{
                            return -1;
                        }
                    }
                    
                }    
            }
                
        }
    }
    return 0;
}

int updateLogFile(char *filePath,char *fileNm,int level)
{
    int y,m,d,h,n;
      time_t now;
      struct tm * ptm;
     now = time(NULL);
      ptm = localtime(&now);
      y = ptm->tm_year+1900;
     m = ptm->tm_mon+1;
     d = ptm->tm_mday;
     h = ptm->tm_hour;
      n = ptm->tm_min;
//     s = ptm->tm_sec;
          
    char oldPath[80] = "";
    char newPath[100] = "";
    sprintf(oldPath,"%s%s%s",filePath,"/",fileNm);
    switch(level)
    {
        case 0:
            sprintf(newPath,"%s%s%s%d%s%d%s%d%s%d%s%d%s%ld%s",filePath,"/",
                "ERROR_",y,"_",m,"_",d,"_",h,"_",n,"~",now,"_.log");
            break;
        case 1:    
            sprintf(newPath,"%s%s%s%d%s%d%s%d%s%d%s%d%s%ld%s",filePath,"/",
                    "INFO_",y,"_",m,"_",d,"_",h,"_",n,"~",now,"_.log");
            break;

    }
    if(rename(oldPath,newPath) == 0)
        return 0;
    else
        return -1;
}

int filePreTreat(char *filePath,char *fileNm,unsigned int maxSz,int bkNum,int logSz)
{
    FILE * fpL = NULL;
    int fileSz = -1;
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",filePath,"/",fileNm);

    int isAvalb = access(filePath,F_OK);
    if(isAvalb != 0)
    {
        mkdir(filePath,0777);
        return 0;
    }
    isAvalb = access(filePath,F_OK);
    if(isAvalb != 0)
    {
        fpL = fopen(absPath,"wb");
        fclose(fpL);
        return 0;
    }
    fpL = fopen(absPath,"r");
    if(NULL == fpL)
        return -1;
    fseek(fpL,0L,SEEK_END);
    fileSz = ftell(fpL);
    fclose(fpL);
    if((logSz + fileSz )< maxSz)
        return 0;
    else     
        return -1;
}

extern logCtlHandle logInitHandle(const char *fileNm,const char *filePh,int maxSz,int bkNum)
{
    LOGDEFINE * ptrLog = (LOGDEFINE *)malloc(sizeof(LOGDEFINE));
    strcpy(ptrLog->fileName,fileNm);
    strcpy(ptrLog->filePath,filePh);
    ptrLog->maxSize = maxSz;
    ptrLog->keepNum = bkNum;
    pthread_mutex_init(&ptrLog->mutexRW,NULL);
    return (logCtlHandle)ptrLog;
}
void logFormatCh(const char ch,logCtlHandle lgHandle,int level)
{
    LOGDEFINE * ptrLog = (LOGDEFINE *)lgHandle;
    char bufDec[5] = "";
    int ret = filePreTreat(ptrLog->filePath,ptrLog->fileName,ptrLog->maxSize,ptrLog->keepNum,1);
    if(ret == -1)
    {
        if(listFileAndDel(ptrLog->filePath,ptrLog->keepNum)==0)
            updateLogFile(ptrLog->filePath,ptrLog->fileName,level);
    }
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",ptrLog->filePath,"/",ptrLog->fileName);
    FILE *fp = fopen(absPath,"a+");
    sprintf(bufDec,"%c",ch);
    fwrite(bufDec,1,1,fp);
    fclose(fp);
}

void logFormatInt(const int dec,logCtlHandle lgHandle,int level)
{
       LOGDEFINE * ptrLog = (LOGDEFINE *)lgHandle;
    char bufDec[50] = "";
    sprintf(bufDec,"%d",dec);
    int logLength = strlen(bufDec);
    int ret = filePreTreat(ptrLog->filePath,ptrLog->fileName,ptrLog->maxSize,ptrLog->keepNum,logLength);
    if(ret == -1)
    {
        if(listFileAndDel(ptrLog->filePath,ptrLog->keepNum)==0)
            updateLogFile(ptrLog->filePath,ptrLog->fileName,level);
    }
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",ptrLog->filePath,"/",ptrLog->fileName);
    FILE *fp = fopen(absPath,"a+");
    fwrite(bufDec,1,logLength,fp);
    fclose(fp);
}

void logFormatStr(const char *ptr,logCtlHandle lgHandle,int level)
{
       LOGDEFINE * ptrLog = (LOGDEFINE *)lgHandle;
    int logLength = strlen(ptr);
    int ret = filePreTreat(ptrLog->filePath,ptrLog->fileName,ptrLog->maxSize,ptrLog->keepNum,logLength);
    if(ret == -1)
    {
        if(listFileAndDel(ptrLog->filePath,ptrLog->keepNum)==0)
            updateLogFile(ptrLog->filePath,ptrLog->fileName,level);
    }
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",ptrLog->filePath,"/",ptrLog->fileName);
    FILE *fp = fopen(absPath,"a+");
    fwrite(ptr,1,logLength,fp);
    fclose(fp);

}

void logFormatFloat(const float flt,logCtlHandle lgHandle,int level)
{
       LOGDEFINE * ptrLog = (LOGDEFINE *)lgHandle;
    char bufDec[50] = "";
    sprintf(bufDec,"%f",flt);
    int logLength = strlen(bufDec);
    int ret = filePreTreat(ptrLog->filePath,ptrLog->fileName,ptrLog->maxSize,ptrLog->keepNum,logLength);
    if(ret == -1)
    {
        if(listFileAndDel(ptrLog->filePath,ptrLog->keepNum)==0)
            updateLogFile(ptrLog->filePath,ptrLog->fileName,level);
    }
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",ptrLog->filePath,"/",ptrLog->fileName);
    FILE *fp = fopen(absPath,"a+");
    fwrite(bufDec,1,logLength,fp);
    fclose(fp);
}

extern void BASE_INFO_LOG(logCtlHandle lgHandle,const char *format,...)
{
#if 1
    LOGDEFINE * ptrLog = (LOGDEFINE *)lgHandle;
    pthread_mutex_lock(&ptrLog->mutexRW);
    va_list ap;
    va_start(ap,format);

    while(*format)
    {
        if(*format != '%')
        {
            putchar(*format);
            format++;
        }
        else
        {
            format++;
            switch(*format)
            {
                case 'c':
                {
                    char valch = va_arg(ap,int);
                    logFormatCh(valch,lgHandle,1);
                    format++;
                        break;
                }
                case 'd':
                {
                    int valint = va_arg(ap,int);
                    logFormatInt(valint,lgHandle,1);
                    format++;
                    break;
                }
                case 's':
                {
                    char *valstr = va_arg(ap,char *);
                    logFormatStr(valstr,lgHandle,1);
                    format++;
                    break;
                }
                case 'f':
                {
                    float valflt = va_arg(ap,double);
                    logFormatFloat(valflt,lgHandle,1);
                    format++;
                    break;
                }
                default:
                    logFormatCh(*format,lgHandle,1);
                    format++;
            }       
        }
    }
    va_end(ap);
    
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",ptrLog->filePath,"/",ptrLog->fileName);
    FILE *fp = fopen(absPath,"a+");
    fwrite("\n",1,1,fp);
    fclose(fp);

    pthread_mutex_unlock(&ptrLog->mutexRW);
#endif
}


extern void BASE_ERROR_LOG(logCtlHandle lgHandle,const char *format,...)
{
#if 1
    LOGDEFINE * ptrLog = (LOGDEFINE *)lgHandle;
    pthread_mutex_lock(&ptrLog->mutexRW);
    va_list ap;
    va_start(ap,format);
    while(*format)
    {
        if(*format != '%')
        {
            putchar(*format);
            format++;
        }
        else
        {
            format++;
            switch(*format)
            {
                case 'c':
                {
                    char valch = va_arg(ap,int);
                    logFormatCh(valch,lgHandle,0);
                    format++;
                        break;
                }
                case 'd':
                {
                    int valint = va_arg(ap,int);
                    logFormatInt(valint,lgHandle,0);
                    format++;
                    break;
                }
                case 's':
                {
                    char *valstr = va_arg(ap,char *);
                    logFormatStr(valstr,lgHandle,0);
                    format++;
                    break;
                }
                case 'f':
                {
                    float valflt = va_arg(ap,double);
                    logFormatFloat(valflt,lgHandle,0);
                    format++;
                    break;
                }
                default:
                    logFormatCh(*format,lgHandle,0);
                    format++;
            }
            va_end(ap);
        }
    }

    
    char absPath[100] = "";
    sprintf(absPath,"%s%s%s",ptrLog->filePath,"/",ptrLog->fileName);
    FILE *fp = fopen(absPath,"a+");
    fwrite("\n",1,1,fp);
    fclose(fp);

    pthread_mutex_unlock(&ptrLog->mutexRW);
#endif    
}


#if 0


int main()
{
    logCtlHandle logHandle = logInitHandle(FILE_NAME,FILE_PATH,MAX_SIZE,KEEP_NUM);
    logCtlHandle logErr = logInitHandle(ERROR_FILE,FILE_PATH,MAX_SIZE,KEEP_NUM);
    BASE_INFO_LOG(logHandle,"%s%c%d%f","HELLO WORLD\n",'C',100,200.0);
    BASE_ERROR_LOG(logErr,"%s%c%d%f","HELLO WORLD\n",'C',100,200.0);

    char ch = 'A';
    char *str = "holle world";
    int dec = 1234;
    float flt = 1234.3456789; 

    return 0;
}

#endif
