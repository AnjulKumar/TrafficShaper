#include<stdio.h>
#include<stdlib.h>
#include "my402list.h"
#include "cs402.h"
/*
typedef struct tagMy402ListElem 
{    
	void *obj;
	struct tagMy402ListElem *next;
	struct tagMy402ListElem *prev;:261
} My402ListElem;
*/
//tagMy402ListElem front;
/*

int  My402ListLength(My402List*);
extern int  My402ListEmpty(My402List*);

extern int  My402ListAppend(My402List*, void*);
extern int  My402ListPrepend(My402List*, void*);
extern void My402ListUnlink(My402List*, My402ListElem*);
extern void My402ListUnlinkAll(My402List*);
extern int  My402ListInsertAfter(My402List*, void*, My402ListElem*);
extern int  My402ListInsertBefore(My402List*, void*, My402ListElem*);

extern My402ListElem *My402ListFirst(My402List*);
extern My402ListElem *My402ListLast(My402List*);
extern My402ListElem *My402ListNext(My402List*, My402ListElem*);
extern My402ListElem *My402ListPrev(My402List*, My402ListElem*);

extern My402ListElem *My402ListFind(My402List*, void*);
*/


//void Traverse(My402List *myList);


int  My402ListLength(My402List* myList)
//Returns the number of elements in the list.
{
	int count = 0;
	My402ListElem *cur;
	if(myList->anchor.next==NULL)
		return 0;
	cur=myList->anchor.next;
	while(cur!=&myList->anchor)
	{
		count++;
		cur=cur->next;
	}
	myList->num_members=count;
	return myList->num_members;
}

int  My402ListEmpty(My402List* myList)
//Returns TRUE if the list is empty. Returns FALSE otherwise.
{
	if(myList->anchor.next==NULL || myList->anchor.prev==NULL)
		return 1;
	else
		return 0;
}


int  My402ListAppend(My402List* myList,void* data)
//Add obj after Last(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
{
	My402ListElem *rear;
	My402ListElem* node=(My402ListElem *) malloc(sizeof(My402ListElem));
	if(node == NULL)
	{
		return 0;
	}
	else
	{
		node->obj= data;
		if (myList->anchor.prev==NULL || myList->anchor.next==NULL)
		{
			//printf("Appending at anchor\n");
			myList->anchor.next = node;			
			myList->anchor.prev=node;
			node->prev=&myList->anchor;
			node->next=&myList->anchor;
		}
		else
		{
			//printf("appending\n");			
			rear=My402ListLast(myList);
			node->next = &myList->anchor;	
			node->prev = rear;
			myList->anchor.prev=node;
			rear->next= node;			
		}
		//printf("Append : %d\n", (int *) data);
		return 1;
	}
	
}


int  My402ListPrepend(My402List* myList, void* data)
//Add obj before First(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise.
{
	My402ListElem *first;
	My402ListElem* node=(My402ListElem*)malloc(sizeof(My402ListElem));
	if(node==NULL)
	{
		return 0;
	}
	else
	{
		//printf("Prepend\n");
		node->obj= data;
		if(myList->anchor.next==NULL || myList->anchor.prev==NULL)
		{
			myList->anchor.next=node;
			myList->anchor.prev=node;
			node->next=&myList->anchor;
			node->prev=&myList->anchor;
		}
		else
		{
			first=My402ListFirst(myList);
			node->next=first;
			node->prev=&myList->anchor;
			first->prev=node;
			myList->anchor.next=node;
		}
		return 1;
	}

}

void My402ListUnlink(My402List* myList, My402ListElem* elem)
//Unlink and delete elem from the list. Please do not delete the object pointed to by elem and do not check if elem is on the list.
{
	My402ListElem* elemn;
	My402ListElem* elemp;
	elemn=elem->next;
	elemp=elem->prev;
	if((My402ListEmpty(myList)))
	{
		return;
	}
	if((My402ListLength(myList))==1)
	{
		elem->next=NULL;
		elem->prev=NULL;
		myList->anchor.next=NULL;
		myList->anchor.prev=NULL;
	}
	/*else if(elem==My402ListLast(myList))
	{
		myList->anchor.next=elem->next;
		elem->next->prev=&myList->anchor;
	}
	else if(elem==My402ListFirst(myList))
	{
		myList->anchor.prev=elem->prev;
		elem->prev->next=&myList->anchor;
	}*/
	else
	{
		elemp->next=elemn;
		elemn->prev=elemp;
		elem->next=NULL;	
		elem->prev=NULL;
	}
	free(elem);	
}
void My402ListUnlinkAll(My402List* myList)
//Unlink and delete all elements from the list and make the list empty. Please do not delete the objects pointed to be the list elements.
{
	My402ListElem* elem, *tempelem;
	if (My402ListEmpty(myList))
		return;
	elem=My402ListFirst(myList);
	while(elem!=&myList->anchor)
	{
		tempelem = elem->next;
		My402ListUnlink(myList,elem);		
		elem=tempelem;
	}
}

int  My402ListInsertBefore(My402List* myList, void* data, My402ListElem* elem)
//Insert obj between elem and elem->prev. If elem is NULL, then this is the same as Prepend(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. Please do not check if elem is on the list.
{
	My402ListElem* node=(My402ListElem*)malloc(sizeof(My402ListElem));
        if(node==NULL)
        {
                return 0;
        }
        else
        {
                if(elem==NULL || myList->anchor.next==NULL)
                {
                       My402ListPrepend(myList,data);
                }
               	else
                {
			node->obj=data;
                        node->next=elem;
			elem->prev->next=node;
			node->prev=elem->prev;
			elem->prev=node;

                }
                return 1;
        }
}

int  My402ListInsertAfter(My402List* myList, void* data, My402ListElem* elem)
//Insert obj between elem and elem->next. If elem is NULL, then this is the same as Append(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. Please do not check if elem is on the list.
{
	My402ListElem* node=(My402ListElem*)malloc(sizeof(My402ListElem));
        if(node==NULL)
        {
                return 0;
        }
        else
        {
                if(elem==NULL || myList->anchor.next==NULL)
                {
                        My402ListAppend(myList,data);
                }
                else
                {
			node->obj=data;
                        node->next=elem->next;
			elem->next->prev=node;
      			elem->next=node;
			node->prev=elem;			
                }
                return 1;
        }

}


My402ListElem *My402ListFirst(My402List* myList)
//Returns the first list element or NULL if the list is empty.
{
	if(myList->anchor.next==NULL && myList->anchor.prev==NULL)
		return NULL;
	else
		return myList->anchor.next;
}
My402ListElem *My402ListLast(My402List* myList)
//Returns the last list element or NULL if the list is empty.
{
	if(myList->anchor.next==NULL)
		return NULL;
	else
	{
		return myList->anchor.prev;
	}
}


My402ListElem *My402ListNext(My402List* myList, My402ListElem* elem)
//Returns elem->next or NULL if elem is the last item on the list. Please do not check if elem is on the list.
{
	if(elem->next==&myList->anchor || myList->anchor.next==NULL)
	{
		return NULL;
	}
	else
	{
		return (elem->next);
	}
}


My402ListElem *My402ListPrev(My402List* myList, My402ListElem* elem)

//Returns elem->prev or NULL if elem is the first item on the list. Please do not check if elem is on the list.
{
	if(elem->prev==&myList->anchor || myList->anchor.next==NULL)
	{
		return NULL;
	}
	else
	{
		return (elem->prev);
	}
}


My402ListElem *My402ListFind(My402List* myList, void* data)
//Returns the list element elem such that elem->obj == obj. Returns NULL if no such element can be found.
{
	My402ListElem *elem;
	elem=My402ListFirst(myList);
	
	if(My402ListEmpty(myList))
		return NULL;	
	while(elem != &myList->anchor)
	{
		if(elem->obj==data)
			return elem;
		elem= elem->next;
	}
	return NULL;
}

int My402ListInit (My402List* myList)
//Initialize the list into an empty list. Returns TRUE if all is well and returns FALSE if there is an error initializing the list.
{
	
	myList->num_members=0;
	myList->anchor.next=NULL;
	myList->anchor.prev=NULL;
	myList->anchor.obj=NULL;
	return 1;
}
/*
int main()
{
	My402List *myList;
	myList = (My402List *)malloc(sizeof(My402List));
	
	printf("Hello\n");	 	
	printf("Hello\n");	 	
	int n = My402ListInit(myList);
	Traverse(myList);
	if(My402ListEmpty(myList))
		printf("list is empty\n");	
	
	My402ListAppend(myList, (void*)2);
	//printf("count:%d\n", My402ListLength(myList));
	//My402ListUnlinkAll(myList);
	//printf("count:%d\n", My402ListLength(myList));
	My402ListAppend(myList, (void*)3);
 	My402ListAppend(myList, (void*)4);
	Traverse(myList);
	My402ListPrepend(myList, (void*)20);
	My402ListPrepend(myList, (void*)30);
	My402ListPrepend(myList, (void*)40);
	Traverse(myList);
	My402ListElem *elem = (My402ListElem*)malloc(sizeof(My402ListElem)); 
//	elem = My402ListFind(myList,20)
//	puts(&elem->obj);
	My402ListInsertBefore(myList,(void*)55,My402ListFind(myList,20));
	My402ListInsertBefore(myList,(void*)65,My402ListFind(myList,30));
	My402ListInsertBefore(myList,(void*)75,My402ListFind(myList,40));
	Traverse(myList);
	My402ListInsertAfter(myList, (void*)11,My402ListFind(myList,20));
	My402ListInsertAfter(myList, (void*)11,My402ListFind(myList,30));
	My402ListInsertAfter(myList, (void*)11,My402ListFind(myList,40));
	My402ListElem * node = My402ListFirst(myList);
	//printf("%d\n", (int)node->obj);
	//node = My402ListLast(myList);
	//printf("%d\n", (int)node->obj);
	Traverse(myList);
	int cnt=0;
	cnt=My402ListLength(myList);
	printf("count111: %d\n", cnt);
	My402ListUnlink(myList, My402ListFind(myList,20));
	My402ListUnlink(myList, My402ListFind(myList,30));
	My402ListElem *dnode;
	printf("DEBUG: NewTrav\n");
	Traverse(myList);	
	cnt=My402ListLength(myList);
	printf("count122: %d\n", cnt);
//	dnode=My402ListPrev(myList, My402ListFind(myList,10));
//	printf("prev: %d",(int)dnode->obj);
	dnode = My402ListFirst(myList);	
	printf("First: %d\n", (int)dnode->obj);
	dnode = My402ListLast(myList);
	printf("Last: %d\n", (int)dnode->obj);
	Traverse(myList);
	dnode = My402ListFind(myList,75);
	printf("find: %d\n", (int)dnode->obj);
	if(dnode)printf("Don't print\n");
	My402ListUnlinkAll(myList);
	cnt=My402ListLength(myList);
	printf("count144: %d\n", cnt);
	return 0;	
}
void Traverse(My402List *myList)
{
	My402ListElem *elem=NULL;
	printf("TEST\n");
	for (elem=My402ListFirst(myList);elem != NULL;elem=My402ListNext(myList, elem)) 
	{
		printf("%d\n",(int)elem->obj);
        // access foo here 
	}
}*/
