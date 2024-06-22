#include "catalog.h"
#include "query.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
   if(relation.empty())
	return BADCATPARM;

   Status status;
   RelDesc relDesc;
   status = relCat->getInfo(relation, relDesc);
   if(status != OK) return status;
 
   int count = relDesc.attrCnt;
   if(count != attrCnt)
	return BADCATPARM; //check with pradap
   
   AttrDesc attrDesc[attrCnt];
   for (int i = 0; i<attrCnt; i++){
	status = attrCat->getInfo(attrList[i].relName,
				  attrList[i].attrName,
				  attrDesc[i]);
	if(status != OK) return status;
   }

   int recLen = 0;
   for (int i=0; i<attrCnt; i++){
	if(attrList[i].attrValue == NULL) //check if any attribute is null
		return BADCATPARM;
	recLen = recLen + attrDesc[i].attrLen; //calculate the record length to be inserted in the result table
	
   }

   char outputData[recLen];
   Record outputRec;

   outputRec.data   = (void*) outputData;
   outputRec.length = recLen;
   char * value;     
   for (int i=0; i<attrCnt; i++){
	if(attrDesc[i].attrType == INTEGER){
		int temp = atoi((char*)attrList[i].attrValue);
		int *val  = &temp;
		value = (char*)val;
	}
	else if(attrDesc[i].attrType == FLOAT){
		float ftemp = (float)atof((char*)attrList[i].attrValue);
		float *fval = &ftemp;
		value = (char*)fval;
	}
	else{
		value = (char*)attrList[i].attrValue;
	}
	
	memcpy(outputData+attrDesc[i].attrOffset,value, attrDesc[i].attrLen);	
   }
   InsertFileScan ifs(relation, status);
   if(status != OK) return status;
   RID outRid;
   status = ifs.insertRecord(outputRec, outRid);
   if(status != OK) return status;

return OK;

}
