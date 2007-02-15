#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"

#define 	RINGSIZE		4096					// size of ring buffer //
#define 	MAXMATCH		18						// upper limit for match_length //
#define 	THRESHOLD		2   					// encode string into position and length //
#define 	NIL				RINGSIZE				// index for root of binary search trees //

unsigned char	*ringbuffer;
int				*lson,*rson,*dad;
int 			match_position, match_length;

//------------------------------------------------------------------//
// char InitCompress(void)											//
// This function is called to setup and allocate buffers required	//
// to peform compression.  If the function is unable to allocate	//
// all of the buffers required for compression, a FALSE value is	//
// returned to the caller and all previously allocated buffers are	//
// released back to the heap.										//
//------------------------------------------------------------------//
char InitCompress(void)
{
	ringbuffer=(unsigned char*)malloc(RINGSIZE+MAXMATCH-1);
	if(ringbuffer==(unsigned char*)NULL) return FALSE;

	dad=(int*)malloc(sizeof(int)*(RINGSIZE+1));
	if(dad==(int*)NULL){
		free(ringbuffer);
		return FALSE;
	}

	lson=(int*)malloc(sizeof(int)*(RINGSIZE+1));
	if(lson==(int*)NULL){
		free(ringbuffer);
		free(dad);
		return FALSE;
	}

	rson=(int*)malloc(sizeof(int)*(RINGSIZE+257));
	if(rson==(int*)NULL){
		free(ringbuffer);
		free(dad);
		free(lson);
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------//
// void EndCompress(void)											//
// This function is called to end a compresssion session and frees	//
// the allocated memory back to the heap.							//
//------------------------------------------------------------------//
void EndCompress(void)
{
	if(ringbuffer!=(unsigned char*)NULL){
		free(ringbuffer);
		ringbuffer = NULL;
	}
	if(dad!=(int*)NULL) free(dad);
	if(lson!=(int*)NULL) free(lson);
	if(rson!=(int*)NULL) free(rson);
}

//------------------------------------------------------------------//
// char InitDecompress(void)										//
// This function is called to setup and allocate buffers required	//
// to peform decompression.  If the function is unable to allocate	//
// all of the buffers required for decompression, a FALSE value is	//
// returned to the caller and all previously allocated buffers are	//
// released back to the heap.										//
//------------------------------------------------------------------//
char InitDecompress(void)
{
	ringbuffer=(unsigned char*)malloc(RINGSIZE+MAXMATCH-1);
	if(ringbuffer==(unsigned char*)NULL) return FALSE;
	return TRUE;
}

//------------------------------------------------------------------//
// void EndDecompress(void)											//
// This function is called to clean up after decompression.  It is	//
// responsible for freeing up the memory allocated to the ring		//
// buffer.															//
//------------------------------------------------------------------//
void EndDecompress(void)
{
	if(ringbuffer!=(unsigned char*)NULL)
		free(ringbuffer);
}

//------------------------------------------------------------------//
// void InitTree(void)												//
// This function initializes the trees used for strings during the	//
// compression. 													//
//------------------------------------------------------------------//
void InitTree(void)  // initialize trees //
{
	int  i;

	for(i=RINGSIZE+1;i<=RINGSIZE+256;i++) rson[i]=NIL;
	for(i=0;i<RINGSIZE;i++) dad[i]=NIL;
}


/*------------------------------------------------------------------*/
/* void InsertNode(int r)											*/
/* This function inserts a string of length MAXMATCH into one of the*/
/* trees and returns the longest-match position and length via the 	*/
/* global variables 'match_position' and 'match_length'.  If 		*/
/* match_length equals MAXMATCH, it will remove the old node in		*/
/* favor of the new one. 'r' is set to the offset into the			*/
/* ringbuffer.														*/
/*------------------------------------------------------------------*/
void InsertNode(int r)
{
	int  			i, p, cmp;
	unsigned char  	*key;

	cmp=1;
	key=&ringbuffer[r];
	p=RINGSIZE+1+key[0];
	rson[r]=lson[r]=NIL;
	match_length=0;
	for(;;){
		if(cmp>=0){
			if(rson[p]!=NIL){
				p=rson[p];
			}else{
				rson[p]=r;
				dad[r]=p;
				return;
			}
		}else{
			if(lson[p]!=NIL){
				p=lson[p];
			}else{
				lson[p]=r;
				dad[r]=p;
				return;
			}
		}
		for(i=1;i<MAXMATCH;i++){
			if((cmp=key[i]-ringbuffer[p+i])!=0) break;
		}
		if(i>match_length){
			match_position=p;
			if((match_length=i)>=MAXMATCH) break;
		}
	}
	dad[r]=dad[p];
	lson[r]=lson[p];
	rson[r]=rson[p];
	dad[lson[p]]=r;
	dad[rson[p]]=r;
	if(rson[dad[p]]==p){
		rson[dad[p]]=r;
	}else{
		lson[dad[p]]=r;
	}
	dad[p]=NIL;  					/* remove p */
}

/*------------------------------------------------------------------*/
/* void DeleteNode(int p)											*/
/* This function will delete node 'p' from tree.					*/
/*------------------------------------------------------------------*/
void DeleteNode(int p)
{
	int  q;

	if(dad[p]==NIL) return;  /* not in tree */

	if(rson[p]==NIL){
		q=lson[p];
	}else{
		if(lson[p]==NIL){
			q=rson[p];
		}else{
			q=lson[p];
			if(rson[q]!=NIL){
				do{
					q=rson[q];
				}while(rson[q]!=NIL);
				rson[dad[q]]=lson[q];
				dad[lson[q]]=dad[q];
				lson[q]=lson[p];
				dad[lson[p]]=q;
			}
			rson[q]=rson[p];
			dad[rson[p]]=q;
		}
	}
	dad[q]=dad[p];
	if(rson[dad[p]]==p){
		rson[dad[p]]=q;
	}else{
		lson[dad[p]]=q;
	}
	dad[p]=NIL;
}

/*------------------------------------------------------------------*/
/* int EncodeLZSS(unsigned char* source, unsigned char* dest,		*/
/*				  int size);										*/
/* This functions performs the actual encoding of the compressed	*/
/* data and returns the size of the compressed data.				*/
/*------------------------------------------------------------------*/
int EncodeLZSS(unsigned char* source, unsigned char* dest, int size)
{
	int  			i,len,r,s,last_match_length,code_buf_ptr;
	int				codesize=0;
	unsigned char 	code_buf[17],mask;
	unsigned char	*sourcePtr,*destPtr;
	unsigned char	c;

	sourcePtr=source;
	destPtr=dest;

	if(size==0) return 0;

	InitTree();

	code_buf[0]=0;
	code_buf_ptr=mask=1;
	s=0;
	r=RINGSIZE-MAXMATCH;
	for(i=s;i<r;i++) ringbuffer[i]=' ';
	for(len=0;(len<MAXMATCH) && ((sourcePtr-source)<size);len++){
		c=*sourcePtr++;
		ringbuffer[r + len] = c;
	}
	for(i=1;i<=MAXMATCH;i++) InsertNode(r-i);

	InsertNode(r);
	do{
		if(match_length>len) match_length=len;
		if(match_length<=THRESHOLD){
			match_length=1;
			code_buf[0]|=mask;
			code_buf[code_buf_ptr++]=ringbuffer[r];
		}else{
			code_buf[code_buf_ptr++]=(unsigned char)match_position;
			code_buf[code_buf_ptr++]=(unsigned char)(((match_position>>4)&0xf0)
													  | (match_length - (THRESHOLD + 1)));
		}
		if((mask<<=1)==0){
			for(i=0;i<code_buf_ptr;i++){
				*destPtr++=code_buf[i];
			}
			codesize+=code_buf_ptr;
			code_buf[0]=0;
			code_buf_ptr=mask=1;
		}
		last_match_length=match_length;
		for(i=0;(i<last_match_length) && ((sourcePtr-source)<size);i++){
			c=*sourcePtr++;
			DeleteNode(s);
			ringbuffer[s]=c;
			if(s<MAXMATCH-1) ringbuffer[s+RINGSIZE]=c;
			s=(s+1)&(RINGSIZE-1);
			r=(r+1)&(RINGSIZE-1);
			InsertNode(r);
		}
		while(i++<last_match_length){
			DeleteNode(s);
			s=(s+1)&(RINGSIZE-1);
			r=(r+1)&(RINGSIZE-1);
			if(--len) InsertNode(r);
		}
	}while(len>0);

	if(code_buf_ptr>1){
		for(i=0;i<code_buf_ptr;i++) *destPtr++=code_buf[i];
		codesize+=code_buf_ptr;
	}
	return(codesize);
}

