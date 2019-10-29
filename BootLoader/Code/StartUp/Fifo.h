

#ifndef ___FIFO__H_
#define ___FIFO__H_

void _EI(void);
void _DI(void);
#define printf__(X)     printf("%s = %x\r\n",#X,X)
#define NEW_FIFO(__NAME,__SIZE,__TYPE,__SIZE_t)	\
		__TYPE   FIFO_##__NAME##Buff[(__SIZE)];\
		__TYPE  *FIFO_##__NAME##Tail = FIFO_##__NAME##Buff;\
		__TYPE  *FIFO_##__NAME##Head = FIFO_##__NAME##Buff;\
		__SIZE_t FIFO_##__NAME##_GetOne(__TYPE *pData)\
		{_DI();\
			if (FIFO_##__NAME##Tail != FIFO_##__NAME##Head)\
			{\
				if((FIFO_##__NAME##Head-FIFO_##__NAME##Buff)==__SIZE)	\
			FIFO_##__NAME##Head=FIFO_##__NAME##Buff;\
								*pData = *FIFO_##__NAME##Head++;\
				_EI(); return 0;\
			}\
			_EI(); return 1;\
		}\
		__SIZE_t FIFO_##__NAME##_GetCount(void)\
		{\
			__SIZE_t temp;_DI();\
			if(FIFO_##__NAME##Tail >= FIFO_##__NAME##Head)\
				temp = FIFO_##__NAME##Tail - FIFO_##__NAME##Head;\
			else \
				temp = __SIZE-(FIFO_##__NAME##Head - FIFO_##__NAME##Tail);\
			_EI(); return temp;\
		}\
		__SIZE_t FIFO_##__NAME##_PutOne(__TYPE pData)\
		{\
			if((FIFO_##__NAME##Tail-FIFO_##__NAME##Buff)==(__SIZE))\
					FIFO_##__NAME##Tail=FIFO_##__NAME##Buff;\
				*FIFO_##__NAME##Tail++=pData;\
			return 0;\
		}\
		void FIFO_##__NAME##_Init()\
		{\
			FIFO_##__NAME##Head=FIFO_##__NAME##Buff;\
			FIFO_##__NAME##Tail=FIFO_##__NAME##Buff;\
			return ;\
		}
#define	   FIFO_Init(__NAME)    				FIFO_##__NAME##_Init()
#define		FIFO_GetOne(__NAME,pData)    	FIFO_##__NAME##_GetOne(pData)
#define   FIFO_GetCount(__NAME)         FIFO_##__NAME##_GetCount()
#define   FIFO_PutOne(__NAME,pData) 	FIFO_##__NAME##_PutOne(pData)
#define 	EXTERN_FIFO_PutCount(__NAME,__TYPE,__SIZE_t)				extern __SIZE_t FIFO_##__NAME##_PutOne(__TYPE );
#define		EXTERN_FIFO_GetCount(__NAME,__TYPE,__SIZE_t)		extern		__SIZE_t FIFO_##__NAME##_GetCount(void)
	


#endif
