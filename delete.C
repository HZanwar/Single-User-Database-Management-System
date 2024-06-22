#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
   if(relation.empty())
	return BADCATPARM;

   Status status;
   int offset;
   int length;
   Datatype attrType;
   Operator myop;
   const char* value;
    
   AttrDesc attrDesc;

   if(attrName.empty()){
	   offset = 0;
	   length = 0;
	   attrType = STRING;
	   myop     = EQ;
	   value    = NULL;

   }
   else{
	   status = attrCat->getInfo(relation, attrName, attrDesc);
	   if(status != OK)
		return status;

	   if(attrDesc.attrType != type)
		return ATTRTYPEMISMATCH;

	   offset = attrDesc.attrOffset;
	   length = attrDesc.attrLen;
 	   attrType   = (Datatype) attrDesc.attrType;  
	   if(attrType == INTEGER){
		int temp = atoi(attrValue);
		int *val = &temp; 
		value = (char*)val;
	   }
	   else if(attrType == FLOAT){
		cout<<"Reached inside float"<<endl;
		float ftemp  =(double) atof(attrValue);
		cout<<ftemp<<endl;
		float *fval  = &ftemp;
		value = (char*)fval;
           }
	   else
		value = attrValue;	
	   myop   = op;
   }

   HeapFileScan hfs(relation, status);
   if(status != OK) return status;

   status = hfs.startScan(attrDesc.attrOffset, 
			  attrDesc.attrLen,
			  attrType,
			  value,
			  myop);

   if(status != OK) return status;

   RID rid;
   while(hfs.scanNext(rid) == OK){
	status = hfs.deleteRecord();
	if(status != OK) return status;
   }
   return OK;

}

