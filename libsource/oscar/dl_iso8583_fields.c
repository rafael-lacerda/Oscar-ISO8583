/******************************************************************************/
/*                                                                            */
/* Copyright (C) 2005-2007 Oscar Sanderson                                    */
/*                                                                            */
/* This software is provided 'as-is', without any express or implied          */
/* warranty.  In no event will the author(s) be held liable for any damages   */
/* arising from the use of this software.                                     */
/*                                                                            */
/* Permission is granted to anyone to use this software for any purpose,      */
/* including commercial applications, and to alter it and redistribute it     */
/* freely, subject to the following restrictions:                             */
/*                                                                            */
/* 1. The origin of this software must not be misrepresented; you must not    */
/*    claim that you wrote the original software. If you use this software    */
/*    in a product, an acknowledgment in the product documentation would be   */
/*    appreciated but is not required.                                        */
/*                                                                            */
/* 2. Altered source versions must be plainly marked as such, and must not be */
/*    misrepresented as being the original software.                          */
/*                                                                            */
/* 3. This notice may not be removed or altered from any source distribution. */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* ISO8583 field packers / unpackers                                          */
/*                                                                            */
/******************************************************************************/

#include "oscar/dl_iso8583_fields.h"

DL_UINT16 DEBUG = 0;

/******************************************************************************/
//
// FIELD HANDLER PROTOTYPES
//

DL_ERR _pack_iso_ASCHEX ( DL_UINT16                    iField,
						  const DL_ISO8583_MSG        *iMsg,
						  const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						  DL_UINT8                   **ioPtr );

// unpacks ISO Numeric (bcd format)
// NB if iSize is odd then we have a padding char on left
//    (but don't include when unpacking)
DL_ERR _unpack_iso_ASCHEX ( DL_UINT16                    iField,
						    DL_ISO8583_MSG              *ioMsg,
						    const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						    DL_UINT8                   **ioPtr );

DL_ERR _pack_iso_ASCII ( DL_UINT16                    iField,
						 const DL_ISO8583_MSG        *iMsg,
						 const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						 DL_UINT8                   **ioPtr,
						 DL_UINT16                    isEbcdic );

DL_ERR _unpack_iso_ASCII ( DL_UINT16                    iField,
						   DL_ISO8583_MSG              *ioMsg,
						   const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						   DL_UINT8                   **ioPtr,
						   DL_UINT16                    isEbcdic );

DL_ERR _pack_iso_BINARY ( DL_UINT16                    iField,
						  const DL_ISO8583_MSG        *iMsg,
						  const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						  DL_UINT8                   **ioPtr,
						  DL_UINT16                    isEbcdic );

DL_ERR _unpack_iso_BINARY ( DL_UINT16                    iField,
						    DL_ISO8583_MSG              *ioMsg,
						    const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						    DL_UINT8                   **ioPtr,
							DL_UINT16                    isEbcdic );

DL_ERR _pack_iso_BITMAP ( DL_UINT16                    iField,
						  const DL_ISO8583_MSG        *iMsg,
						  const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						  DL_UINT8                   **ioPtr );

DL_ERR _unpack_iso_BITMAP ( DL_UINT16                    iField,
						    DL_ISO8583_MSG              *ioMsg,
						    const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						    DL_UINT8                   **ioPtr );

DL_ERR _hexstr_to_bytes (	const char *hexStr,
                     		DL_UINT8 *output,
                     		DL_UINT16 *outputLen);

/******************************************************************************/
//
// LENGTH HANDLER PROTOTYPES
//

// outputs the variable length element
// iVarLenType - e.g. kDL_ISO8583_LLVAR
static DL_ERR VarLen_Put ( DL_UINT8    iVarLenType,
						   DL_UINT32   iActLen,
						   DL_UINT32  *ioReqLen,
						   DL_UINT8  **ioPtr,
						   DL_UINT16   isEbcdic );

// determines variable length element
static DL_ERR VarLen_Get ( const DL_UINT8 **ioPtr,
			               DL_UINT8         iVarLenDigits,
				           DL_UINT16        iMaxValue,
				           DL_UINT16       *oLen,
						   DL_UINT16   isEbcdic );

/******************************************************************************/
//
// TYPES
//

struct DL_ISO8583_TYPE_S
{
	DL_ERR    (*unpackFunc)(DL_UINT16,DL_ISO8583_MSG*,const DL_ISO8583_FIELD_DEF*,DL_UINT8**,DL_UINT16);
	DL_ERR    (*packFunc  )(DL_UINT16,const DL_ISO8583_MSG*,const DL_ISO8583_FIELD_DEF*,DL_UINT8**,DL_UINT16);
};
typedef struct DL_ISO8583_TYPE_S DL_ISO8583_TYPE;

/******************************************************************************/
//
// VARIABLES
//

static DL_ISO8583_TYPE fieldTypeArr[] = {
/* ISO_N    */ {_unpack_iso_ASCHEX,_pack_iso_ASCHEX},
/* ISO_NS   */ {_unpack_iso_BINARY,_pack_iso_BINARY},
/* ISO_XN   */ {_unpack_iso_ASCHEX,_pack_iso_ASCHEX},
/* ISO_A    */ {_unpack_iso_ASCII ,_pack_iso_ASCII },
/* ISO_AN   */ {_unpack_iso_ASCII ,_pack_iso_ASCII },
/* ISO_ANS  */ {_unpack_iso_ASCII ,_pack_iso_ASCII },
/* ISO_ANSB */ {_unpack_iso_ASCII ,_pack_iso_ASCII },
/* ISO_ANP  */ {_unpack_iso_ASCII ,_pack_iso_ASCII },
/* ISO_B    */ {_unpack_iso_BINARY,_pack_iso_BINARY},
/* ISO_Z    */ {_unpack_iso_BINARY,_pack_iso_BINARY},
/* ISO_BMAP */ {_unpack_iso_BITMAP,_pack_iso_BITMAP} };

/******************************************************************************/
//
// MACROS
//

// gets the field type details
#define GetFieldType(fieldType)\
 (&fieldTypeArr[fieldType])

/******************************************************************************/

DL_ERR _DL_ISO8583_FIELD_Pack ( DL_UINT16                  iField,
								const DL_ISO8583_MSG      *iMsg,
								const DL_ISO8583_HANDLER  *iHandler,
								DL_UINT8                 **ioPtr,
								DL_UINT16                  isEbcdic )
{
	DL_ERR                err          = kDL_ERR_NONE;
	DL_ISO8583_FIELD_DEF *fieldDefPtr  = DL_ISO8583_GetFieldDef(iField,iHandler);
	DL_ISO8583_TYPE      *fieldTypePtr = GetFieldType(fieldDefPtr->fieldType);

	err = fieldTypePtr->packFunc(iField,iMsg,fieldDefPtr,ioPtr,isEbcdic);

	return err;
}

/******************************************************************************/

DL_ERR _DL_ISO8583_FIELD_Unpack ( DL_UINT16                  iField,
								  DL_ISO8583_MSG            *ioMsg,
								  const DL_ISO8583_HANDLER  *iHandler,
								  DL_UINT8                 **ioPtr,
								  DL_UINT16					 isEbcdic )
{
	DL_ERR                err          = kDL_ERR_NONE;
	DL_ISO8583_FIELD_DEF *fieldDefPtr  = DL_ISO8583_GetFieldDef(iField,iHandler);
	DL_ISO8583_TYPE      *fieldTypePtr = GetFieldType(fieldDefPtr->fieldType);

	err = fieldTypePtr->unpackFunc(iField,ioMsg,fieldDefPtr,ioPtr,isEbcdic);

	return err;
}

/******************************************************************************/

DL_ERR _pack_iso_ASCHEX ( DL_UINT16                    iField,
						  const DL_ISO8583_MSG        *iMsg,
						  const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						  DL_UINT8                   **ioPtr )
{
	DL_ERR                err           = kDL_ERR_NONE;
	DL_UINT8             *tmpPtr        = *ioPtr;
	DL_ISO8583_MSG_FIELD *fieldPtr      = ((DL_ISO8583_MSG*)iMsg)->field + iField;
	DL_UINT32             actLen        = fieldPtr->len;
	DL_UINT8             *dataPtr       = fieldPtr->ptr;
	DL_UINT32             reqLen        = iFieldDefPtr->len;
	DL_UINT32             wholeActBytes = 0;
	DL_UINT32             wholeReqBytes = 0;
	DL_UINT32             i;

	/* variable length handling */
	err = VarLen_Put(iFieldDefPtr->varLen,actLen,&reqLen,&tmpPtr,0);

	if ( !err )
	{
		if ( actLen > reqLen ) /* too long */
		{
			err = kDL_ERR_OTHER;
		}
		else
		{
			/* determine numbers of bytes for required / actual lengths      */
			/* NB 'required bytes' are rounded up, 'actual' are rounded down */
			wholeActBytes = actLen / 2;
			wholeReqBytes = (reqLen + 1) / 2;

			/* output left padding (00h) bytes - where required */
			/* NB less one if the actual length has an odd number of digits */
			i = wholeReqBytes - wholeActBytes;
			if ( actLen % 2 )
				i--;
			DL_MEM_memset(tmpPtr,0,i);
			tmpPtr += i;

			/* handle partial digit - if required */
			if ( actLen % 2 ) /* have partial digit */
			{
				*tmpPtr++ = (DL_UINT8)DL_ASCHEX_2_NIBBLE(dataPtr[0]);
				dataPtr++;
			}

			/* handle complete digit pairs */
			for ( i=0 ; i<wholeActBytes ; i++,dataPtr+=2 )
			{
				*tmpPtr++ = (DL_UINT8)((DL_ASCHEX_2_NIBBLE(dataPtr[0]) << 4) |
										DL_ASCHEX_2_NIBBLE(dataPtr[1])         );
			} /* end-for */
		}
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

// unpacks ISO Numeric (bcd format)
// NB if iSize is odd then we have a padding char on left
//    (but don't include when unpacking)
// NB doesn't remove any leading padding (0 nibbles)
DL_ERR _unpack_iso_ASCHEX ( DL_UINT16                    iField,
						    DL_ISO8583_MSG              *ioMsg,
						    const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						    DL_UINT8                   **ioPtr )
{
	DL_ERR     err        = kDL_ERR_NONE;
	DL_UINT8  *tmpPtr     = *ioPtr;
	DL_UINT16  size       = 0;
	DL_UINT8  *tmpDataPtr = NULL;

	/* variable length handling */
	err = VarLen_Get(&tmpPtr,iFieldDefPtr->varLen,iFieldDefPtr->len,&size,0);

	/* allocate field */
	if ( !err )
		err = _DL_ISO8583_MSG_AllocField(iField,size,ioMsg,&tmpDataPtr);

	if ( !err )
	{
		DL_UINT8 ch;

		/* if size is 'odd' then ignore the leading nibble, as this is a pad character */
		if ( size % 2 ) /* odd */
		{
			ch = *tmpPtr & 0x0f;
			*tmpDataPtr++ = DL_NIBBLE_2_ASCHEX(ch);
			tmpPtr++;
			size -= 1;
		}

		size /= 2;
		while ( size-- > 0 )
		{
			ch = (*tmpPtr >> 4) & 0xf;
			*tmpDataPtr++ = DL_NIBBLE_2_ASCHEX(ch);
			ch = *tmpPtr & 0xf;
			*tmpDataPtr++ = DL_NIBBLE_2_ASCHEX(ch);
			tmpPtr++;
		}
		
		*tmpDataPtr = kDL_ASCII_NULL; /* null terminate */
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

DL_ERR _pack_iso_ASCII ( DL_UINT16                    iField,
						 const DL_ISO8583_MSG        *iMsg,
						 const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						 DL_UINT8                   **ioPtr,
						 DL_UINT16					  isEbcdic )
{
	DL_ERR                err      = kDL_ERR_NONE;
	DL_UINT8             *tmpPtr   = *ioPtr;
	DL_ISO8583_MSG_FIELD *fieldPtr = ((DL_ISO8583_MSG*)iMsg)->field + iField;
	DL_UINT32             actLen   = fieldPtr->len;
	DL_UINT8             *dataPtr  = fieldPtr->ptr;
	DL_UINT8			dataPtrEbcdic[actLen];
	DL_UINT32             reqLen   = iFieldDefPtr->len;

	/* variable length handling */
	err = VarLen_Put(iFieldDefPtr->varLen,actLen,&reqLen,&tmpPtr,isEbcdic);

	if (isEbcdic) {
		for (int i = 0; i < actLen; i++) {
			dataPtrEbcdic[i] = fieldPtr->ptr[i];
			dataPtrEbcdic[i] = ASCIItoEBCDIC(dataPtrEbcdic[i]);
		}
	}

	if ( !err )
	{
		if ( actLen > reqLen ) /* too long */
		{
			err = kDL_ERR_OTHER;
		}
		else if ( actLen == reqLen ) /* exact size */
		{
			/* copy up to 'required' amount */
			if (isEbcdic)
				DL_MEM_memcpy(tmpPtr,dataPtrEbcdic,reqLen);
			else
				DL_MEM_memcpy(tmpPtr,dataPtr,reqLen);
			tmpPtr += reqLen;
		}
		else /* shorter - so need to right pad (space) */
		{	
			if (isEbcdic) {
				/* copy what data we have (actual length) */
				DL_MEM_memcpy(tmpPtr,dataPtrEbcdic,actLen);
				/* right pad as required */
				DL_MEM_memset(tmpPtr+actLen,(int)kDL_ASCII_SP,reqLen-actLen);
			}
			else 
			{
				/* copy what data we have (actual length) */
				DL_MEM_memcpy(tmpPtr,dataPtr,actLen);
				/* right pad as required */
				DL_MEM_memset(tmpPtr+actLen,(int)kDL_ASCII_SP,reqLen-actLen);
			}
			tmpPtr += reqLen;
		}
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

// NB doesn't remove any trailing padding (spaces)
DL_ERR _unpack_iso_ASCII ( DL_UINT16                    iField,
						   DL_ISO8583_MSG              *ioMsg,
						   const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						   DL_UINT8                   **ioPtr,
						   DL_UINT16					isEbcdic )
{
	DL_ERR     err        = kDL_ERR_NONE;
	DL_UINT8  *tmpPtr     = *ioPtr;
	DL_UINT16  size       = 0;
	DL_UINT8  *tmpDataPtr = NULL;
	//DL_UINT32    actLen   = iFieldDefPtr->len;

	/* variable length handling */
	err = VarLen_Get(&tmpPtr,iFieldDefPtr->varLen,iFieldDefPtr->len,&size,isEbcdic);

	if ( !err ) 
	{
		if (isEbcdic) {
			for (int i = 0; i < size; i++) {
				tmpPtr[i] = EBCDICtoASCII(tmpPtr[i]);
			}
		}
	}

	/* allocate field */
	if ( !err )
		err = _DL_ISO8583_MSG_AllocField(iField,size,ioMsg,&tmpDataPtr);

	if ( !err )
	{
		DL_MEM_memcpy(tmpDataPtr,tmpPtr,size); 
		tmpPtr     += size;
		tmpDataPtr += size;

		*tmpDataPtr = kDL_ASCII_NULL; /* null terminate */
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

DL_ERR _pack_iso_BINARY ( DL_UINT16                    iField,
						  const DL_ISO8583_MSG        *iMsg,
						  const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						  DL_UINT8                   **ioPtr,
						  DL_UINT16					   isEbcdic )
{
	DL_ERR                err      = kDL_ERR_NONE;
	DL_UINT8             *tmpPtr   = *ioPtr;
	DL_ISO8583_MSG_FIELD *fieldPtr = ((DL_ISO8583_MSG*)iMsg)->field + iField;
	DL_UINT32             actLen   = fieldPtr->len;
	DL_UINT8             *dataPtr  = fieldPtr->ptr;
	DL_UINT8 			dataPtrEbcdic[actLen];
	DL_UINT32             reqLen   = iFieldDefPtr->len;
	DL_UINT8              dataPtrBytes[(actLen/2)];
	DL_UINT32			 *actLenPtr = &actLen;

	
	if (iField == 0) {
		if (isEbcdic)	{
			for (int i = 0; i < actLen; i++) {
				dataPtrEbcdic[i] = fieldPtr->ptr[i];
				dataPtrEbcdic[i] = ASCIItoEBCDIC(dataPtrEbcdic[i]);
			}
		}
	/* correct binary fields that shoud be recorded as bytes */
	}  else  {
		// printf("Before: ");
		// fwrite(dataPtr, actLen, 1, stdout);
		// printf("\n\n");

		err = _hexstr_to_bytes(dataPtr,dataPtrBytes,actLenPtr);
		dataPtr = dataPtrBytes;

		// printf("Later: ");
		// fwrite(dataPtr, actLen, 1, stdout);
		// printf("\n\n");
		if ( err ) return err;
	}
	
	/* variable length handling */
	err = VarLen_Put(iFieldDefPtr->varLen,actLen,&reqLen,&tmpPtr,isEbcdic);

	if ( !err )
	{
		if ( actLen > reqLen ) /* too long */
		{
			err = kDL_ERR_OTHER;
		}
		else if ( actLen == reqLen ) /* exact size */
		{
			if (isEbcdic && iField == 0) {
				/* copy up to 'required' amount */
				DL_MEM_memcpy(tmpPtr,dataPtrEbcdic,reqLen);
				tmpPtr += reqLen;
			} else {
				/* copy up to 'required' amount */
				DL_MEM_memcpy(tmpPtr,dataPtr,reqLen);
				tmpPtr += reqLen;
			}
		}
		else /* shorter - so need to right pad (space) */
		{
			if (isEbcdic)
			{			
				/* copy what data we have (actual length) */
				DL_MEM_memcpy(tmpPtr,dataPtrEbcdic,actLen);
				/* right pad as required */
				DL_MEM_memset(tmpPtr+actLen,(int)0,reqLen-actLen);
			}
			else 
			{			
				/* copy what data we have (actual length) */
				DL_MEM_memcpy(tmpPtr,dataPtr,actLen);
				/* right pad as required */
				DL_MEM_memset(tmpPtr+actLen,(int)0,reqLen-actLen);
			}
			tmpPtr += reqLen;
		}
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

// NB doesn't remove any trailing padding (0's)
DL_ERR _unpack_iso_BINARY ( DL_UINT16                    iField,
						    DL_ISO8583_MSG              *ioMsg,
						    const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						    DL_UINT8                   **ioPtr,
							DL_UINT16					 isEbcdic )
{
	DL_ERR     err        = kDL_ERR_NONE;
	DL_UINT8  *tmpPtr     = *ioPtr;
	DL_UINT16  size       = 0;
	DL_UINT8  *tmpDataPtr = NULL;
	

	/* variable length handling */
	err = VarLen_Get(&tmpPtr,iFieldDefPtr->varLen,iFieldDefPtr->len,&size,isEbcdic);
	// printf("Size is: %u\n",size);

	if (iField == 0) {
		if (isEbcdic)	{
			for (int i = 0; i < size; i++) {
				tmpPtr[i] = EBCDICtoASCII(tmpPtr[i]);
			}
		}
			/* allocate field */
		if ( !err )
			err = _DL_ISO8583_MSG_AllocField(iField,size,ioMsg,&tmpDataPtr);

		if ( !err )
		{
			DL_MEM_memcpy(tmpDataPtr,tmpPtr,size); 
			tmpPtr     += size;
			tmpDataPtr += size;

			*tmpDataPtr = kDL_ASCII_NULL; /* null terminate */
		}

	} else {

		DL_UINT16  tmPsize = size*2;

		/* allocate field */
		if ( !err )
			err = _DL_ISO8583_MSG_AllocField(iField,tmPsize,ioMsg,&tmpDataPtr);

		if ( !err ) 
		{
			if (DEBUG == 2)
			{
				printf("Before: ");
				fwrite(tmpPtr, size, 1, stdout);
				printf("\n\n");
			}
			
			err = _bcd_to_asc(tmpPtr,tmpDataPtr,tmPsize);
			tmpPtr     += size;
			tmpDataPtr     += tmPsize;
			*tmpDataPtr = kDL_ASCII_NULL; /* null terminate */

			if (DEBUG == 2)
			{
				printf("Later: ");
				fwrite(tmpDataPtr, tmPsize, 1, stdout);
				printf("\n\n");
			}
		}
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

DL_ERR _pack_iso_BITMAP ( DL_UINT16                    iField,
						  const DL_ISO8583_MSG        *iMsg,
						  const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
						  DL_UINT8                   **ioPtr )
{
	DL_ERR     err         = kDL_ERR_NONE;
	DL_UINT8  *tmpPtr      = *ioPtr;
	DL_UINT16  curFieldIdx = iField;
	int        i;

	/* for each possible bitmap segment */
	for ( i=0 ; i<((kDL_ISO8583_MAX_FIELD_IDX-iField+1)+63)/64 ; i++ )
	{
		DL_UINT32 ms=0,
				  ls=0;
		int       j;

		/* move to next field */
		if(i == 0)
			curFieldIdx++;

		for ( j=0 ; j<31 + i ; j++,curFieldIdx++ )
		{
			ms <<= 1;
			if ( (curFieldIdx <= kDL_ISO8583_MAX_FIELD_IDX) &&
				 (NULL != iMsg->field[curFieldIdx].ptr) )
				ms++;
		}

		for ( j=0 ; j<32 ; j++,curFieldIdx++ )
		{
			ls <<= 1;
			if ( (curFieldIdx <= kDL_ISO8583_MAX_FIELD_IDX) &&
				 (NULL != iMsg->field[curFieldIdx].ptr) )
				ls++;
		}

		/* output bitmap segment (if required) */
		if ( 0 == i )
		{
			/* NB 1st segment is always output */
			DL_UINT32_TO_BYTES(ms,tmpPtr);
			DL_UINT32_TO_BYTES(ls,tmpPtr+4);
			tmpPtr += 8;
		}
		else
		{
			if ( ms || ls )
			{
				/* set continuation bit of previous segment */
				*(tmpPtr-8) |= 0x80;

				DL_UINT32_TO_BYTES(ms,tmpPtr);
				DL_UINT32_TO_BYTES(ls,tmpPtr+4);
				tmpPtr += 8;
			}
			else
			{
				/* no fields present, so don't output */
				break;
			}
		}
	} /* end-for(i) */

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

DL_ERR _unpack_iso_BITMAP ( DL_UINT16                    iField,
					        DL_ISO8583_MSG              *ioMsg,
					        const DL_ISO8583_FIELD_DEF  *iFieldDefPtr,
					        DL_UINT8                   **ioPtr )
{
	DL_ERR     err    = kDL_ERR_NONE;
	DL_UINT8  *tmpPtr = *ioPtr;
	DL_UINT32 i = 0;

	{
		DL_UINT16 curFieldIdx = iField;

		/* for each bitmap segment (8 bytes) */
		while(i<(((kDL_ISO8583_MAX_FIELD_IDX-iField+1)+63)/64))
		{
			DL_UINT32 ms,ls;
			int       j;

			/* get bitmap segment (8 bytes) */
			ms = DL_BYTES_TO_UINT32(tmpPtr);
			ls = DL_BYTES_TO_UINT32(tmpPtr+4);
			tmpPtr += 8;

			/* move to next field */
			if(i == 0)
			curFieldIdx++;

			/* ms part */
			for ( j=30 + i ; j>=0 ; j--,curFieldIdx++ )
			{
				if ( DL_BIT_TEST(ms,j) )
				{
					if ( curFieldIdx > kDL_ISO8583_MAX_FIELD_IDX )
						return kDL_ERR_OTHER;

					/* set length to non-zero value to indicate field presence */
					ioMsg->field[curFieldIdx].len = 1;
				}
			} /* end-for(j) */

			/* ls part */
			for ( j=31 ; j>=0 ; j--,curFieldIdx++ )
			{
				if ( DL_BIT_TEST(ls,j) )
				{
					if ( curFieldIdx > kDL_ISO8583_MAX_FIELD_IDX )
						return kDL_ERR_OTHER;

					/* set length to non-zero value to indicate field presence */
					ioMsg->field[curFieldIdx].len = 1;
				}
			} /* end-for(j) */

			/* stop if no more bitmap segments */
			if( i == 0)
			{
			if ( 0 == DL_BIT_TEST(ms,31) )
				break;
			}
			i++;
		}
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

// returns the bcd encoded value - based on decValue (0..99)
#define output_bcd_byte(decValue)\
 ((DL_UINT8)((((decValue)/10)<<4)|((decValue)%10)))

// outputs the variable length element
// iVarLenType - e.g. kDL_ISO8583_LLVAR
static DL_ERR VarLen_Put ( DL_UINT8    iVarLenType,
						   DL_UINT32   iActLen,
						   DL_UINT32  *ioReqLen,
						   DL_UINT8  **ioPtr,
						   DL_UINT16   isEbcdic )
{
	DL_ERR    err    = kDL_ERR_NONE;
	DL_UINT8 *tmpPtr = *ioPtr;
	DL_UINT8 dataPtr[4];
	//printf("%x%x%x\n",*(tmpPtr-1),*tmpPtr,*(tmpPtr+1));
	switch ( iVarLenType )
	{
		case kDL_ISO8583_FIXED:
			/* do nothing */
			break;
		case kDL_ISO8583_LLVAR:
			// printf("%lu\n",iActLen/10);
			iActLen  %= 100;
			dataPtr[0] = (iActLen/10 + '0') ;
			dataPtr[1] = (iActLen%10 + '0') ;
			// printf("%lu\n",iActLen%10);
			// printf("Aqui %x\n",dataPtr);
			//printf("%u\n",tmpPtr);
			//*tmpPtr++    = output_bcd_byte(iActLen);

			if (isEbcdic) {
				for (int i = 0; i < kDL_ISO8583_LLVAR; i++) {
					dataPtr[i] = ASCIItoEBCDIC(dataPtr[i]);
				}
			}

			DL_MEM_memcpy(tmpPtr,&dataPtr, 2);
			tmpPtr += 2;
			*ioReqLen  = iActLen;
			// *tmpPtr++    = output_bcd_byte(iActLen);
			break;
		case kDL_ISO8583_LLLVAR:
			iActLen   %= 1000;
			dataPtr[0] = (iActLen/100 + '0') ;
			dataPtr[1] = ((iActLen%100)/10 + '0') ;
			dataPtr[2] = (((iActLen%100)%10) + '0') ;

			if (isEbcdic) {
				for (int i = 0; i < kDL_ISO8583_LLLVAR; i++) {
					dataPtr[i] = ASCIItoEBCDIC(dataPtr[i]);
				}
			}

			DL_MEM_memcpy(tmpPtr,&dataPtr, 3);
			tmpPtr += 3;
			*ioReqLen  = iActLen;
			// *tmpPtr++    = output_bcd_byte(iActLen/100);
			// *tmpPtr++    = output_bcd_byte(iActLen%100);
			break;
		case kDL_ISO8583_LLLLVAR:
			iActLen   %= 10000;
			*ioReqLen  = iActLen;
			*tmpPtr++    = output_bcd_byte(iActLen/100);
			*tmpPtr++    = output_bcd_byte(iActLen%100);
			break;
		default:
			/* [ERROR] unsupported length type */
			err = kDL_ERR_OTHER;
	} /* end-switch */

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/

// determines variable length element
static DL_ERR VarLen_Get ( const DL_UINT8 **ioPtr,
			               DL_UINT8         iVarLenDigits,
				           DL_UINT16        iMaxValue,
				           DL_UINT16       *oLen,
						   DL_UINT16   		isEbcdic )
{
	DL_ERR    err    = kDL_ERR_NONE;
	DL_UINT8 *tmpPtr = (DL_UINT8*)*ioPtr;
	DL_UINT8 dataPtr[4];

	/* init outputs */
	*oLen = iMaxValue;

	//printf("%x\n",*tmpPtr);

	if ( kDL_ISO8583_FIXED != iVarLenDigits )
	{	
		*oLen = 0;

		if (DEBUG)
		{
			printf("\tLen Bytes to read: ");
			for (int i = 0; i < iVarLenDigits; i++) 
			{
				printf("%02X ", *(tmpPtr+i));
			}
			printf("\n");
		}

		if ( iVarLenDigits == 2 ) {

			if (isEbcdic) {
				for (int i = 0; i < iVarLenDigits; i++) {
					tmpPtr[i] = EBCDICtoASCII(tmpPtr[i]);
				}
			}

			dataPtr[0] = (*tmpPtr - '0');
			dataPtr[1] = (*(tmpPtr+1) - '0');

			*oLen = dataPtr[0]*10 + dataPtr[1];
			//printf("%d %d\n", dataPtr[0],dataPtr[1]);
			tmpPtr += 2;

		} else if ( iVarLenDigits == 3 ) {

			if (isEbcdic) {
				for (int i = 0; i < iVarLenDigits; i++) {
					tmpPtr[i] = EBCDICtoASCII(tmpPtr[i]);
				}
			}

			dataPtr[0] = (*tmpPtr - '0');
			dataPtr[1] = (*(tmpPtr+1) - '0');
			dataPtr[2] = (*(tmpPtr+2) - '0');

			//*oLen = (*tmpPtr - '0')*100 + (*(tmpPtr+1) - '0')*10 + (*(tmpPtr+2) - '0');
			*oLen = dataPtr[0]*100 + dataPtr[1]*10 + dataPtr[2];
			tmpPtr += 3;

		}

		// if ( iVarLenDigits % 2 )
		// 	iVarLenDigits++;

		// while ( iVarLenDigits > 0 )
		// {
		// 	*oLen = (*oLen * 100) +
		// 			((((int)(*tmpPtr) >> 4) & 0xf) * 10) +
		// 			((int)(*tmpPtr) & 0xf);
		// 	iVarLenDigits -= 2;
		// 	tmpPtr++;
		// } /* end-while */

		// printf("Read size Before: %d bytes.\n", *oLen);
		/* limit if exceeds max */
		*oLen = MIN(iMaxValue,*oLen);
	}

	if (DEBUG)
	{	
		printf("\tRead size is %d bytes - ", *oLen);
		for (int i = 0; i < *oLen; i++) {
			printf("%02X ", *(tmpPtr+i));
		}
		printf("\n\n");
	}

	*ioPtr = tmpPtr;

	return err;
}

/******************************************************************************/


DL_ERR _hexstr_to_bytes(const char *hexStr,
                     	DL_UINT8 *output,
                    	DL_UINT16 *outputLen) 
{
	size_t len = strlen(hexStr);
	if (len % 2 != 0) {
		return -1;
	}
	size_t finalLen = len / 2;
	*outputLen = finalLen;
	for (size_t inIdx = 0, outIdx = 0; outIdx < finalLen; inIdx += 2, outIdx++) {
		if ((hexStr[inIdx] - 48) <= 9 && (hexStr[inIdx + 1] - 48) <= 9) {
		goto convert;
		} else {
		if ((hexStr[inIdx] - 65) <= 5 && (hexStr[inIdx + 1] - 65) <= 5) {
			goto convert;
		} else {
			*outputLen = 0;
			return -1;
		}
		}
	convert:
		output[outIdx] =
			(hexStr[inIdx] % 32 + 9) % 25 * 16 + (hexStr[inIdx + 1] % 32 + 9) % 25;
	}
	output[finalLen] = '\0';
	return 0;
}

DL_ERR _bytes_to_hexstr(const DL_UINT8 *bytes,
                     	DL_UINT8 *output,
                    	DL_UINT16 *outputLen) 
{	
	DL_UINT8 ch;
	size_t len = strlen(bytes);
	if (len % 2 != 0) {
		return -1;
	}
	size_t finalLen = len * 2;
	*outputLen = finalLen;

	while ( finalLen-- > 0 )
		{
			ch = (*bytes >> 4) & 0xf;
			*output++ = DL_NIBBLE_2_ASCHEX(ch);
			ch = *bytes & 0xf;
			*output++ = DL_NIBBLE_2_ASCHEX(ch);
		}
	return 0;
}

DL_ERR _bcd_to_asc(	DL_UINT8 * BcdBuf,
					DL_UINT8 * AscBuf,
					DL_UINT16 Len)
{
    int i;

    for (i = 0; i < Len; i++)
    {
        *AscBuf = ((i % 2) ? (*BcdBuf & 0x0f) : ((*BcdBuf >> 4) & 0x0f));
		*AscBuf += ((*AscBuf > 9) ? ('A' - 10) : '0');
		AscBuf++;
		if (i % 2) {
			BcdBuf++;
		}
    }

    return 0;
}


/******************************************************************************/