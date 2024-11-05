
//#define	


#define	TRACE	1

#ifdef	TRACE
#define	trace_printf(fmt, v)	{ printf(fmt, v); }	
#endif
#ifndef	TRACE
#define	trace_printf(fmt, v)
#endif



/*	manual BASIC 260 6-4 
Volume label	1-8		' \00\FF,:'	ignored
						:F2,1,0

*/


#include <windows.h>

	const int i = 1;
#define is_bigendian() ( (*(char*)&i) == 0 )

/*
HP9845

The predefined file types are:
File Type 	File Type Code* 	Purpose
BKUP 	0000 	
File type for backups (used with the DBMS ROM)
DATA 	0400 	
General pupose data container (this is the standard data file type for BASIC programs, and as well used for portable BASIC programs saved in ASCII format)
PROG 	0800 	
Standard file type for storing BASIC programs in a compact, but system specific format
KEYS 	0C00 	
File type for saving special function key definitions/macros
BDAT 	1000 	
File type for packed binary data as used by the FREAD and FWRITE statements (available with the Mass Storage ROM)
ALL 	1400 	
File type for saving complete system dumps (including loaded programs, variables, key definitions, binary routines, and display content)
BPRG 	1800 	
File type for saving binary routines (all currently loaded binaries are saved into one file)
OPRM 	1C00 	
General purpose container for all other option ROMs, the data structure is defined by the option specific ROM (note: the object code file produced by the assembler is of this type when stored on tape instead of disc)
ROOT 	1C80 	
File type for data base root file (same code as ASMB above, used with the DBMS ROM)
DSET 	1D00 	
File type for data base file containing a data set (used with the DBMS ROM)
WORK 	1D80 	
Use unknown
ASMB 	1E00 	
File type for saving object code routines produced with the Assembly Execution and Development ROM
FORM 	1E80 	
Use unknown
TEXT 	1F00 	
Use unknown
TYP1 	1F80 	
Use unknown

*hexadecimal result of masking word 6 in disc directories with hex 1F80. File type codes for tape directories are the same, however only the high byte is used as a single byte code.

DATA  	2400  	General pupose data container (this is the standard data file type for BASIC programs, and as well used for portable BASIC programs saved in ASCII format)
PROG 	4800 	Standard file type for storing BASIC programs in a compact, but system specific format
KEYS 	4C00 	File type for saving special function key definitions/macros
ALL 	5400 	File type for saving complete system dumps (including loaded programs, variables, key definitions, binary routines, and display content)
BPRG 	5800 	File type for saving binary routines (all currently loaded binaries are saved into one file)
BDAT 	5000 	File type for FREAD and FWRITE statements (available with the Mass Storage ROM)
ROOT 	? 	File type for data base root file (used with the DBMS ROM)
BKUP 	4000 	File type for data base backup file (used with the DBMS ROM)
DSET 	5D00 	File type for data base file containing a data set (used with the DBMS ROM)
ASMB 	5E00 	File type for saving object code routines produced with the Assembly Execution and Development ROM
OPRM 	5C00 	General purpose container for all other option ROMs, the data structure is defined by the option specific ROM (note: the object code file produced by the assembler is of this type when stored on tape instead of disc)
TEXT 	5F00 	not used, but reserved

260

SYST	7200
DROM	7600
PROG	6800	
FORM	7A00	FORM
DSET	7C00	+ PRO='01', '02' etc
ROOT	7E00

BPRG	7800
DATA	2400	
KEYS	?		4C00 or 6C00 ?
COMM	?			data communication file
BKUP	?			used for image

RO		0001
*		XXXX
		00C4	Protected by HP, hash code? (first 6 chars)

IMAGE/260

MEDIARECLEN * ENTRIES / 256 <=65534 SECTORS

*/


// The basic entity is the record, a 256 byte block of data.

//#define	SYS_MAGIC	0x0500	// 16 bits allocation table
//#define				0x0600	// 32 bits allocation table used by HP250
#define	END_MARKER	0xFFFF

//#define	HP_INIT_PATTERN		{0x00, 0x00, 0x00, 0xFF}

typedef	unsigned short		word;
typedef unsigned int		word32;


word	swap_word_16	(word	*w)
{
byte	*b = (byte *)w;
	
	*w = ((word)b[0] << 8) | (word)b[1];
}

word32	swap_word_32	(word32	*w)
{
byte	*b = (byte *)w;
	
	*w = ((word32)b[0] << 24) | ((word32)b[1] << 16) | ((word32)b[2] << 8) | (word32)b[3];
}


// Big endian


#define	TYPE_INISYS_HP9835	0x1
#define	TYPE_INISYS_HP9831	0x2
#define	TYPE_INISYS_HP260	0x3
#define	TYPE_INISYS_HP9845	0x4
#define	TYPE_INISYS_HP9825	0x6

#define TYPE_FILE_BKUP		0x0
#define TYPE_FILE_CHAR		0x1
#define TYPE_FILE_DATA		0x2
#define TYPE_FILE_PROG		0x4
#define TYPE_FILE_KEYS		0x6
#define TYPE_FILE_COMM		0x7
#define TYPE_FILE_SYST		0x9
#define TYPE_FILE_DROM		0xB
#define TYPE_FILE_BPRG		0xC
#define TYPE_FILE_FORM		0xD
#define TYPE_FILE_DSET		0xE
#define TYPE_FILE_ROOT		0xF


#pragma pack(1)

typedef struct _main_rec {
	word	wSysId;
	word	wSectors;
	word	wTracks;
	word	wSpareTrackNum;
	word	wMainDirRec;
	word	wAvlTblRec;
	word	wAfterAvlTblRec;
	word	wDataTrackStart;
	word	wNumDataTracks;
	word	wInterleave;
	union {
		struct {
			byte	bFlagsHD:1;
			byte	bFlagsPR:1;
			word	wUnused:13;
			byte	bFlagsSY:1;
		} wFlagsBits;
		word 	wFlags;
	} uFlags;
	word	wHDStartMSB;
	word	wHDStartLSB;
	union {
		struct {
			byte	bFlagsDA:1;
			word	wUnused:15;
		} wAvlTblLengthBits;
		word wAvlTblLength;
	} uAvlTblLength;
	word	wVolID;
	word	wLabelLen;
	byte	sLabel[8];
	word	wFill[108];
} main_rec;


typedef struct _disk_layout {
	word32	dwSectors;
	word32	dwTracks;
	word32	dwMainDirStr;
	word32	dwMainDirLen;
	word32	dwAvlTableStr;
	word32	dwAvlTableLen;
	word32	dwSpareMainRec;
	word32	dwSpareMainDirStr;
	word32	dwSpareAvlTableStr;
	word32	dwFileAreaStr;
	word32	dwFileAreaLen;
	bool	fBigAvlTable;
} disk_layout;


char	*sSys[8] = {		"",
							"HP 9835", 
							"HP 9831", 
							"HP 260",
							"HP 9845",
							"",
							"HP 9825",
							"",
					};
char	*sTypes[8][16] = {	{}, 
							{				// HP 9835
								"????",
								"????",
								"DATA",
								"????",
								"PROG",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????" 
							},
							{				// HP 9831
								"????",
								"????",
								"DATA",
								"????",
								"PROG",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????" 
							},
							{				// HP 260
								"BKUP",
								"CHAR",
								"DATA",
								"OTHR",
								"PROG",
								"OTHR",
								"KEYS",
								"COMM",
								"OTHR",
								"SYST",
								"OTHR",
								"DROM",
								"BPRG",
								"FORM",
								"DSET",
								"ROOT" 
							},
							{				// HP 9845
								"BKUP",
								"BKUP",
								"DATA",
								"DATA",
								"PROG",
								"PROG",
								"KEYS",
								"KEYS",
								"BDAT",
								"BDAT",
								"ALL ",
								"ALL ",
								"BPRG",
								"BPRG",
								"OPRM",
								"ASMB" 
							},
							{},
							{				// HP 9825
								"????",
								"????",
								"DATA",
								"????",
								"PROG",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????",
								"????" 
							},
							{},
						};


typedef struct _dir_entry {
	union {
		word	wName[3];
		byte	sName[6];
	} uName;
	word	wStartAddrLsb;
	word	wFileLen;
	word	wWordsPerRecord;
	union {
		struct {
			word	wStartMsb:7;
			byte	bFlagBU:1;		// Backed up
			byte	bUnused:1;
			byte	bTypeFile:4;
			byte	bIniSys:3;
		} wTypeBits;
		word wType;
	} uType;
	word	wProtectCode;
} dir_entry;


typedef struct _dir_entries {
	dir_entry	zDirEntries[16];
} dir_entries;


struct sys_block {			// 7910
	word	wMagic;			// 005 00
	word	wSectors;		// 32
	word	wTracks;		// 1470
	word	wUnk;			// 1			Sector size in 256 ?
	word	wDirStart;		// 1
	word	wAllocStart;	// 144
	word	wAllocEnd;		// 191
	word	wSysTracks;		// 12
	word	wDataTracks;	// 1458
	word	wInterleave;	// 1
	
};


// Word 0: FF FF = Fin de liste

typedef struct _free_entry_16 {
	word	wStart;
	word	wLen;
} free_entry_16;


typedef struct _free_entry_32 {
	word32	wStart;			
	word32	wLen;
} free_entry_32;


typedef struct _free_entries {
	union {
		free_entry_16	zAvlTbl16[64];
		free_entry_32	zAvlTbl32[32];
	} uAvlTbl;
} free_entries;


/*
Each entry in the allocation table identifies position and length of one continuous free block on the medium. 
The last entry in the allocation table is followed by an end marker ($FFFF hexadecimal).

Each entry in the directory table holds the file name, the file position and length, the logical block size 
(also known as defined record size), the file type and a protection attribute. Again, the last directory enty is 
followed by an end marker.
*/

union word_swap {
	word	w;
	byte	b[2];
};

/*
7908P
7911/7912P
7941A
7942A
7945A
7946A
9133D
9134D

9895A
7906H

7957A/B
7958A/B
9133L/H
9134L/H
9153B/C
9154B/C

Tape
9144A

*/

//#define 



int		create_hd_file	()
{
}


int		init_hd_file	()
{
}

//class C
#include <stdio.h>
using namespace std;



void	read_main_rec		(FILE		*fdir,
							main_rec	*m,
							disk_layout	*dl)
{
	fseek(fdir, 256 * 0, SEEK_SET);
	fread(m, 1, sizeof(main_rec), fdir);
	swap_word_16(&m->wSysId);
	swap_word_16(&m->wSectors);
	swap_word_16(&m->wTracks);
	swap_word_16(&m->wSpareTrackNum);
	swap_word_16(&m->wMainDirRec);
	swap_word_16(&m->wAvlTblRec);
	swap_word_16(&m->wAfterAvlTblRec);
	swap_word_16(&m->wDataTrackStart);
	swap_word_16(&m->wNumDataTracks);
	swap_word_16(&m->wInterleave);
	swap_word_16(&m->uFlags.wFlags);
	swap_word_16(&m->wHDStartMSB);
	swap_word_16(&m->wHDStartLSB);
	swap_word_16(&m->uAvlTblLength.wAvlTblLength);
	swap_word_16(&m->wVolID);
	swap_word_16(&m->wLabelLen);

	dl->dwSectors = m->wSectors;
	dl->dwTracks = m->wTracks;
	dl->dwMainDirStr = m->wMainDirRec;
	dl->dwMainDirLen = m->wAvlTblRec - m->wMainDirRec;
	dl->dwAvlTableStr = m->wAvlTblRec;
	dl->dwAvlTableLen = m->wAfterAvlTblRec - m->wAvlTblRec;
	dl->dwSpareMainRec = (word32)m->wSpareTrackNum * dl->dwSectors;
	dl->dwSpareMainDirStr = dl->dwSpareMainRec + dl->dwMainDirStr;
	dl->dwSpareAvlTableStr = dl->dwSpareMainRec + dl->dwAvlTableStr;
	dl->dwFileAreaStr = (word32)m->wDataTrackStart * dl->dwSectors;
	dl->dwFileAreaLen = (word32)m->wNumDataTracks * dl->dwSectors;
	dl->fBigAvlTable = m->uAvlTblLength.wAvlTblLengthBits.bFlagsDA == 1;
}


void	read_directory_rec	(FILE		*fdir,
							dir_entries	*de,
							word32		rec)
{
int		i;

	fseek(fdir, 256 * rec, SEEK_SET);
	fread(de, 1, sizeof(dir_entries), fdir);
	for (i = 0; i < 16; i++) {
		swap_word_16(&de->zDirEntries[i].wStartAddrLsb);
		swap_word_16(&de->zDirEntries[i].wFileLen);
		swap_word_16(&de->zDirEntries[i].wWordsPerRecord);
		swap_word_16(&de->zDirEntries[i].uType.wType);
		swap_word_16(&de->zDirEntries[i].wProtectCode);
	}
}


void	read_avl_table_rec	(FILE			*fdir,
							disk_layout		*dl,
							free_entries	*fe,
							word32			rec)
{
int		i;

	fseek(fdir, 256 * rec, SEEK_SET);
	fread(fe, 1, sizeof(free_entries), fdir);
	if (dl->fBigAvlTable) {
		for (i = 0; i < 32; i++) {
			swap_word_32(&fe->uAvlTbl.zAvlTbl32[i].wStart);
			swap_word_32(&fe->uAvlTbl.zAvlTbl32[i].wLen);
		}
	} else {
		for (i = 0; i < 64; i++) {
			swap_word_16(&fe->uAvlTbl.zAvlTbl16[i].wStart);
			swap_word_16(&fe->uAvlTbl.zAvlTbl16[i].wLen);
		}
	}
}


bool	check_spare        (FILE			*fdir,
							disk_layout		*dl)
{
byte    main[256],
        spare[256];
word32	r, i;
bool    ret = false;
    
	fseek(fdir, 256 * 0, SEEK_SET);
	fread(main, sizeof(byte), sizeof(main), fdir);
	fseek(fdir, 256 * dl->dwSpareMainRec, SEEK_SET);
	fread(spare, sizeof(byte), sizeof(spare), fdir);
    if (memcmp(main, spare, sizeof(main))) {
        printf("Main and spare record 0 are different\n");
        ret |= true;
    }

	for (r = dl->dwMainDirStr, i = 0; i < dl->dwMainDirLen; r++, i++) {
	   fseek(fdir, 256 * r, SEEK_SET);
	   fread(main, sizeof(byte), sizeof(main), fdir);
	   fseek(fdir, 256 * (dl->dwSpareMainDirStr + i), SEEK_SET);
	   fread(spare, sizeof(byte), sizeof(spare), fdir);
        if (memcmp(main, spare, sizeof(main))) {
            printf("Main and spare directory (%d, %d) are different\n", r, dl->dwSpareMainDirStr + i);
            ret |= true;
        }
    }

	for (r = dl->dwAvlTableStr, i = 0; i < dl->dwAvlTableLen; r++, i++) {
	   fseek(fdir, 256 * r, SEEK_SET);
	   fread(main, sizeof(byte), sizeof(main), fdir);
	   fseek(fdir, 256 * (dl->dwSpareAvlTableStr + i), SEEK_SET);
	   fread(spare, sizeof(byte), sizeof(spare), fdir);
        if (memcmp(main, spare, sizeof(main))) {
            printf("Main and spare availability table (%d, %d) are different\n", r, dl->dwSpareAvlTableStr + i);
            ret |= true;
        }
    }

    return ret;
}


int		main	(int 	argc,
				char 	*argv[])
{
FILE			*fdir;
main_rec		m;
disk_layout		dl;
dir_entries		de;
free_entries	fe;
word32			r, i;
word			ds;
word32			wLargest, wTotal;
int				x;
bool			fDirEnd;
bool			fAvlEnd;

	if (argc != 2) {
		printf("Usage: hp260dir [filname]\n");
		exit(1);
	}
	if (NULL == (fdir = fopen(argv[1], "rb"))) {
		printf("File '%s' doesn't exist\n", argv[1]);
		exit(1);
	}

	read_main_rec(fdir, &m, &dl);
	if (m.wSysId != 0x0600) {
		fclose(fdir);
		printf("This is not a HP250/260 filesystem\n");
		exit(1);
	}

// Check record 0

    check_spare(fdir, &dl);
    
	trace_printf("Sectors             %10d\n", dl.dwSectors);
	trace_printf("Total Tracks        %10d\n", dl.dwTracks);
	trace_printf("Main Dir Str        %10d\n", dl.dwMainDirStr);
	trace_printf("Main Dir Len        %10d\n", dl.dwMainDirLen);
	trace_printf("Avl Table Str       %10d\n", dl.dwAvlTableStr);
	trace_printf("Avl Table Len       %10d\n", dl.dwAvlTableLen);
	trace_printf("Spare Main Rec      %10d\n", dl.dwSpareMainRec);
	trace_printf("Spare Main Dir Str  %10d\n", dl.dwSpareMainDirStr);
	trace_printf("Spare Avl Table Str %10d\n", dl.dwSpareAvlTableStr);
	trace_printf("File Area Str       %10d\n", dl.dwFileAreaStr);
	trace_printf("File Area Len       %10d\n", dl.dwFileAreaLen);
	trace_printf("Big Avl Table       %s\n", (dl.fBigAvlTable) ? "Y, 4 bytes" : "N, 2 bytes");
	trace_printf("\n", 0);

	printf("NAME   PRO   TYPE    REC/FILE   BYTES/REC   ADDRESS\n");
	printf("%s:?2,?,?\n", m.sLabel);
//	printf("****** ***** **** ********** ********** **********\n");
//NAME  PRO TYPE REC/FILE  BYTES/REC   ADDRESS
//H7           148
//320512 of 1136640 bytes free.
	fDirEnd = false;
	for (r = dl.dwMainDirStr, i = 0; i < dl.dwMainDirLen; r++, i++) {
		read_directory_rec(fdir, &de, r);
		for (x = 0; x < 16; x++) {
			if ((de.zDirEntries[x].uName.wName[0] == 0x0000) && 
				(de.zDirEntries[x].uName.wName[1] == 0xFFFF)) {
				fDirEnd = true;
				break;
			}
			if ((de.zDirEntries[x].uName.wName[0] == 0x0000) && 
				(de.zDirEntries[x].uName.wName[1] == 0x0000) && 
				(de.zDirEntries[x].uName.wName[2] == 0x0000)) {
				continue;
			}
			printf("%-6.6s ", de.zDirEntries[x].uName.sName);
			if ((de.zDirEntries[x].uType.wTypeBits.bIniSys == TYPE_INISYS_HP260) && 
				(de.zDirEntries[x].uType.wTypeBits.bTypeFile == TYPE_FILE_DSET)) {
				ds = de.zDirEntries[x].wProtectCode;
				swap_word_16(&ds);
				printf("%-2.2s    ", &ds);
			} else if (de.zDirEntries[x].wProtectCode == 0) {
				printf("      ");
			} else if (de.zDirEntries[x].wProtectCode == 1) {
				printf("RO    ");
			} else {
				printf(" *    ");
			}
//			printf("***** ",
			printf("%4.4s ", sTypes[de.zDirEntries[x].uType.wTypeBits.bIniSys][de.zDirEntries[x].uType.wTypeBits.bTypeFile]);
			printf("%10d ", de.zDirEntries[x].wFileLen);
			printf("%10d ", de.zDirEntries[x].wWordsPerRecord * 2);
			printf("%10d\n", (word32)de.zDirEntries[x].wStartAddrLsb + (de.zDirEntries[x].uType.wTypeBits.wStartMsb << 8));
		}
		if (fDirEnd)
			break;
	}

	wLargest = wTotal = 0;
	fAvlEnd = false;
	for (r = dl.dwAvlTableStr, i = 0; i < dl.dwAvlTableLen; r++, i++) {
		read_avl_table_rec(fdir, &dl, &fe, r);
		if (dl.fBigAvlTable) {
			for (i = 0; i < 32; i++) {
				if (fe.uAvlTbl.zAvlTbl32[i].wStart == 0xFFFF0000) {
					fAvlEnd = true;
					break;
				}
				wTotal += fe.uAvlTbl.zAvlTbl32[i].wLen;
				if (fe.uAvlTbl.zAvlTbl32[i].wLen > wLargest)
					wLargest = fe.uAvlTbl.zAvlTbl32[i].wLen;
			}
		} else {
			for (i = 0; i < 64; i++) {
				if (fe.uAvlTbl.zAvlTbl16[i].wStart == 0xFFFF) {
					fAvlEnd = true;
					break;
				}
				wTotal += (word32)fe.uAvlTbl.zAvlTbl16[i].wLen;
				if (fe.uAvlTbl.zAvlTbl16[i].wLen > wLargest)
					wLargest = (word32)fe.uAvlTbl.zAvlTbl16[i].wLen;
			}
		}
		if (fAvlEnd)
			break;
	}
	printf("LARGEST AVAIL SPACE %10d (%d bytes)\n", wLargest, wLargest * 256);
	printf("TOTAL AVAIL SPACE   %10d (%d bytes)\n", wTotal, wTotal * 256);

	fclose(fdir);
    return EXIT_SUCCESS;
}
