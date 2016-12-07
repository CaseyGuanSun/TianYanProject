/* ******************************************************
** Author       : CaseyGuan
** Last modified: 2016-11-24 23:20
** Email        : caseyguan@posteritytech.com
** Filename     : QueryTask.h
** Description  : 
** ******************************************************/
#ifndef _QUERY_TASK_HEADER_
#define _QUERY_TASK_HEADER_

struct QUERY_TASK_NODE
{
    int         timestamp;
    int         taskId;
    string      tiebaUserName;
    string      mobileWithAsterisk;
    string      mobilePartA;
    string      mobilePartB;
    string      areaCode; 
}QUERY_TASK_NODE;

#endif

