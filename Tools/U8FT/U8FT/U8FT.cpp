// U8FT.cpp : Defines the entry point for the console application.
//
// U8 Flex Tool (Import/Extract)
// =============================
// 
// Well, one tool more. This tools extracts and imports (creates .FLX) classes
// from EUSECODE.FLX.
// 
// Usage:
//
// u8ft -x
// Extract (no parameters, assume the file is always EUSECODE.FLX)
//
// Output: 
// - 'n' count of .cls files (class)
// - index.txt file (list -number- of each class extracted, this is for import).
//
// u8ft -i
// Import (new EUSECODE.flx.new file, index.txt with list of classes, 'n' count of .cls files)
//
// Output:
// - EUSECODE.FLX.new file
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_RECORDS		  2048
#define BUFFER		   1572864
#define HDR_SIZE		   128
#define MAX_LEN			   256
#define EUSECODE_RECORDS 0x599
#define VOICE_RECORDS     0x40
//CHANGE
//#define UCFILE        "EUSECODE.FLX"
//#define UCFILE        "D:\\U8PAGAN\\_Tools\\U8FT\\Debug\\EUSECODE.FLX"
//#define UCFILENEW     "EUSECODE.FLX.new"
//#define UCFILENEW     "D:\\U8PAGAN\\_Tools\\U8FT\\Debug\\EUSECODE.FLX.new"
#define IDXTXT        "index.txt"
//#define IDXTXT        "D:\\U8PAGAN\\_Tools\\U8FT\\Debug\\index.txt"

// Table for the HEADER of EUSECODE.FLX
struct FLX_HEADER{
	char hdr_1A[82];
	char hdr_zero_2[2];
	long hdr_nclss;
	long hdr_v;
	long hdr_fs;
	long hdr_crc;
	char hdr_zero_28[28];
};

struct FLX_RECORDS{
	unsigned long offset_cls;
	unsigned long size_cls;
};

struct IDX_TABLE{
	char fname[MAX_LEN];
	long pos;
	long cls;
};

struct FLX_HEADER flx_hdr;
struct FLX_RECORDS flx_rec[MAX_RECORDS];
struct IDX_TABLE idx_table[MAX_RECORDS];
FILE *ucflxfile, *idxfile, *clsfile;
char bufmem_cls[BUFFER], fext[4];
char flxname[MAX_LEN], clsfname[MAX_LEN];
long N_RECORDS;

/////////////////////////////////////////////////////////////////////////////////////////
// This function will help for do the export.
//
// Inputs : - EUSECODE.FLX.
//
// Outputs: - 'n' hdr_nclss number NNNN.cls files for each class.
//          - index.txt file with the number of classes
/////////////////////////////////////////////////////////////////////////////////////////
int _extract() {
	long cnt = 0, cnt_loop = 0, pos = 0, size_cls = 0, error = 0;
	unsigned long offset_cls = 0;
	char buf[BUFFER];

	// Ok, we open first EUSECODE.FLX file.
	if ((ucflxfile = fopen(flxname, "rb")) == NULL) {
		printf ("Error opening flex file (%s) for reading.\n", flxname);
		return -1;
	}

	// If ok, let's open index.txt file for writing class number in TXT FILE format.
	// EACH LINE is an original nnnn.cls file. ONLY is written the number class.
	if ((idxfile = fopen (IDXTXT, "wt")) == NULL ) {
		printf ("Error opening index file (%s) for writing.\n", IDXTXT);
		return -1;
	}

	// We read HDR of .flx file.
	// We will need number of records.
	fread (&flx_hdr, sizeof (flx_hdr), 1, ucflxfile);
	N_RECORDS = flx_hdr.hdr_nclss;

	// Read the Table of RECORDS (offsets to classes/shapes). We need this to know
	// the relative position of nnnn.cls/nnnn.shp file in the CLASSES/SHAPES block.
	fread (&flx_rec, sizeof (flx_rec), 1, ucflxfile);

	// We loop until the end of MAX_RECORDS reading offsets and sizes
	// for each record, and extract them in NNNN.cls file.
	while (cnt < N_RECORDS) {
		offset_cls = flx_rec[cnt].offset_cls;
		size_cls = flx_rec[cnt].size_cls;

		// Let's check if offset (or size) is zero. This is a NULL class.
		// We will write a line in index.txt file with 0 (zero) in it.
		// After that, let's go for next record.
		if (offset_cls != 0) {
			// First let's write index.txt class, with filename nnnn.cls/nnnn.shp
			// AND position in RECORDS table.

			// HERE we need to know the real position of CLASS in the block.
			// We must do a recursive loop until we know this position.
			while (cnt_loop < N_RECORDS) {
				if ((offset_cls > flx_rec[cnt_loop].offset_cls) && (flx_rec[cnt_loop].offset_cls != 0)) pos++;
				cnt_loop++;
			}

			// Write class number and its position in table records.
			// If it is EUSECODE.FLX we should check GLOBALS & CLASSES files.
			// This are not counted for recompilation purposes as the CALLs in usecode are direct.
			// We will write class/shape number in HEX.
			if (N_RECORDS == EUSECODE_RECORDS) {
				if (cnt > 1) fprintf (idxfile, "%04X.cls;%04d\n", cnt - 2, pos);
				else fprintf (idxfile, "%04X.cls;%04d\n", cnt, pos);

				// Put in var nnnn.cls file for write buffer.
				// The same for nnnn.cls.
				if (cnt > 1) sprintf (clsfname, "%04X.cls", cnt - 2);
				else sprintf (clsfname, "%04X.cls", cnt);
			} else {
				if (N_RECORDS == VOICE_RECORDS) {
					// This is another thing.
					// This is a shape file
					// We put plain number and position in shapes. 
					fprintf (idxfile, "%04X.u8c;%04d\n", cnt, pos);

					// Put in var nnnn.shp file for write buffer.
					// The same for nnnn.shp.
					sprintf (clsfname, "%04X.u8c", cnt);
				} else {
					// This is a shape file
					// We put plain number and position in shapes. 
					fprintf (idxfile, "%04X.shp;%04d\n", cnt, pos);

					// Put in var nnnn.shp file for write buffer.
					// The same for nnnn.shp.
					sprintf (clsfname, "%04X.shp", cnt);
				}
			}

			// Let's write now nnnn.cls/nnnn.shp file.
			if ((clsfile = fopen (clsfname, "wb")) == NULL ) {
				printf ("Error opening class/shp file %s for writing.\n", clsfname);
				return -1;
			}

			// Let's get data from EUSECODE.FLX to write it in nnnn.cls file
			fseek (ucflxfile, offset_cls, 0);
			fread (&buf, sizeof(char), size_cls, ucflxfile);

			// Let's write data into nnnn.cls file
			fwrite (&buf, sizeof(char), size_cls, clsfile);
			fclose (clsfile);

			// Reset some vars.
			pos = 0; cnt_loop = 0;
		}

		cnt++;

		// Ok, let's position the pointer of FLEX FILE for the next record.
		fseek (ucflxfile, HDR_SIZE + (cnt * 8), 0);
	}

	// We close the other files also.
	fclose (idxfile);
	fclose (ucflxfile);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// This function will help for do the import (creation) of a new EUSECODE.FLX.new file.
// It's a bit more complicated than _extract... xD
//
// Inputs : - 'n' hdr_nclss number NNNN.cls files for each class.
//          - index.txt file with the number of classes
//
// Outputs: - EUSECODE.FLX.new file with new (or not) classes imported.
//
/////////////////////////////////////////////////////////////////////////////////////////
int prepare_hdr() {
	int i = 0;

	// Struct for the HEADER of EUSECODE.FLX
	// Let's create a default header. We will need flx_hdr here, Global variable.
	// First 1A bytes.
	for (i = 0; i < 82; i++) flx_hdr.hdr_1A[i] = 26;
	for (i = 0; i < 2; i++) flx_hdr.hdr_zero_2[i] = 0;
	flx_hdr.hdr_nclss = N_RECORDS;
	flx_hdr.hdr_v = 1;
	flx_hdr.hdr_fs = 0;

	if (strcmp (fext, "cls") == 0) flx_hdr.hdr_crc = 71128435; // original value
	else flx_hdr.hdr_crc = 0;
	
	for (i = 0; i < 28; i++) flx_hdr.hdr_zero_28[i] = 0;
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// This function will read index.txt and put in memory struct idx_table
// First field:  nnnn.cls filename
// Second field: n_pos    relative position of nnnn.cls in CLASSES block of
//                        EXUSECODE.FLX
/////////////////////////////////////////////////////////////////////////////////////////
int read_idx(){
	char fname[MAX_LEN];
	char fnum[5], fpos[5], *p;
	long n_cls = 0, n_clspos = 0;
	int result;

	if ((idxfile = fopen (IDXTXT, "rt")) == NULL ) {
		printf ("Error opening index file (%s) for reading.\n", IDXTXT);
		return -1;
	}

	// Init number of records.
	N_RECORDS = 0;

	// Let's get first nnnn.cls class name.
	fgets (fname, MAX_LEN, idxfile);

	while (!feof(idxfile)) {
		// Get class number we are working with. It is readed from index.txt
		p = strstr (fname, ".");
		result = (int)(p - fname + 1);
		fnum[0] = fname[result-5]; fnum[1] = fname[result-4]; fnum[2] = fname[result-3]; fnum[3] = fname[result-2]; fnum[4] = NULL;
		fext[0] = fname[result]; fext[1] = fname[result+1]; fext[2] = fname[result+2]; fext[3] = NULL;

		// Let's find the position of class in RECORDS table.
		p = strstr (fname, ";");
		result = (int)(p - fname + 1);
		fpos[0] = fname[result]; fpos[1] = fname[result+1]; fpos[2] = fname[result+2]; fpos[3] = fname[result+3]; fpos[4] = NULL; 

		// We put null to get nnnn class number filename.
		fname[result-1] = NULL;

		// Let's put numerical class in var.
		n_cls = strtol (fnum, NULL, 16);

		// Let's put numerical position in var.
		n_clspos = strtol (fpos, NULL, 10);

		// Ok, we must complete this idx table in memory from index.txt values.
		strcpy (idx_table[N_RECORDS].fname, fname);
		idx_table[N_RECORDS].pos = n_clspos;

		if (strcmp (fext, "cls") == 0) {
			if (n_cls > 1) idx_table[N_RECORDS].cls = n_cls + 2;
			else idx_table[N_RECORDS].cls = n_cls;
		} else idx_table[N_RECORDS].cls = n_cls;

		// Let's get next nnnn.cls class name.
		fgets (fname, MAX_LEN, idxfile);

		N_RECORDS++;
	}

	// Mark the first not used item to check end of table in further process.
	idx_table[N_RECORDS].pos = -1;

	fclose (idxfile);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// This function will help for do the import (creation) of a new EUSECODE.FLX.new file.
// It's a bit more complicated than _extract... xD
//
// Inputs : - 'n' hdr_nclss number NNNN.cls files for each class.
//          - index.txt file with the number of classes
//
// Outputs: - EUSECODE.FLX.new file with new (or not) classes imported.
//
/////////////////////////////////////////////////////////////////////////////////////////
int _create() {
	unsigned long filesize = 0, x = 0, n_pos = 0, n_cls = 0, offset_pos = 0, total_filesize = 0;
	char *bufmem_cls_pos;
	char buf[BUFFER];
	bool e_idxtable = false;

	// Ok, we need to:
	//   1- Read index.txt table and put it in table memory.
	//   2- Search the class in ascendant order beginning by class 0
	//   3- Load class from nnnn.cls file in buffer.
	//   4- Modify record table at n_clspos position (offset and size).
	//   5- Add to memory block CLASS nnnn.cls.
	//   6- Repeat 2-5 until no more nnnn.cls files.
	//   7- Finally write HEADER, RECORDS and memory buffer with CLASSES.

	// We begin by reading index.txt file and put it in table memory
	// by the order taken from second value in the file.
	read_idx();

	// Ok, if it is EUSECODE what we are doing, then
	// N_RECORDS MUST BE EUSECODE_RECORDS. MANDATORY
	if (strcmp (fext, "cls") == 0) N_RECORDS = EUSECODE_RECORDS;
	if (strcmp (fext, "u8c") == 0) N_RECORDS = VOICE_RECORDS;

	// Prepare memory buffer.
	bufmem_cls_pos = bufmem_cls;

	// Prepare offset_pos to calculate offsets for each nnnn.cls we read.
	// First offset will be HDR_SIZE + RECORDS TABLE.
	offset_pos = HDR_SIZE + N_RECORDS * 8;

    // Well, let's read every CLASS nnnn.cls file in specific
	// idx_table order.
	while (n_cls < N_RECORDS && !e_idxtable) {
		// Ok, we must search every file in "pos" order in index table
		// to get class "cls" number of the table.
		// Then read it, and put nnnn.cls file in this position.
		// Adecuate also in records table the offset and size of this nnnn.cls.
		// Ok, if size of class (or pos) is zero, we pass to next class.
		// First treated is 0000.cls. We should not have problems with
		// this concrete class.

		// Search class by the .pos variable "order".
		// First record. Always should be 1 record (0 record) at least in a FLX file.
		n_pos = 0;
		while (n_cls != idx_table[n_pos].pos && idx_table[n_pos].pos != -1) n_pos++;

		if (idx_table[n_pos].pos != -1) {

			// Now open the nnnn.cls file and add it to buffer.
			// First we open the file.
			if ((clsfile = fopen (idx_table[n_pos].fname, "rb")) == NULL ) {
				printf ("Error opening class/shp file (%s) for reading.\n", idx_table[n_pos].fname);
				return -1;
			}

			// Read the file into buffer.
			fseek(clsfile, 0, SEEK_END);
			filesize = ftell (clsfile);
			fseek(clsfile, 0, SEEK_SET);
			fread (&buf, sizeof(char), filesize, clsfile);
			fclose (clsfile);
			//buf[filesize] = NULL;

			// Let's count total_filesize for voice classes.
			total_filesize = total_filesize + filesize;

			// Add the class to buffer memory of classes.
			memcpy(bufmem_cls_pos, buf, filesize);
			bufmem_cls_pos = bufmem_cls_pos + filesize;

			// Set in record block data for offset and size.
			flx_rec[idx_table[n_pos].cls].offset_cls = offset_pos;
			flx_rec[idx_table[n_pos].cls].size_cls = filesize;

			// Prepare offset for this class.
			offset_pos = offset_pos + filesize;
		} else e_idxtable = true;

		// Next class.
		n_cls++;

		// Search class by the .pos variable "order".
		// Next record.
		n_pos = 0;
		while (n_cls != idx_table[n_pos].pos && idx_table[n_pos].pos != -1) n_pos++;
	}

	// We add null for the writing of memory buffer.
//	offset_pos++;
//	buf[offset_pos] = NULL;

	// We have to mount right now the new EUSECODE.FLX.new file.
	if ((ucflxfile = fopen (flxname, "wb")) == NULL ) {
		printf ("Error opening flex file (%s) for writing.\n", flxname);
		return -1;
	}

	// Ok, we write, HEADER, RECORDS struct and buffer memory of classes.
	// Let's try to create first FLEX FILE in PC MEMORY.
	// First thing to do is prepare a default HEADER.
	prepare_hdr();

	// Voice FLX, we add some things into header.
	if (N_RECORDS == VOICE_RECORDS) {
		flx_hdr.hdr_fs = total_filesize + (HDR_SIZE + (N_RECORDS * 8));
		flx_hdr.hdr_nclss = VOICE_RECORDS;
		flx_hdr.hdr_v = 9;
	}

	// Write flex file.
	fwrite (&flx_hdr, sizeof (flx_hdr), 1, ucflxfile);
 	fwrite (flx_rec, sizeof(FLX_RECORDS) * N_RECORDS, 1, ucflxfile);
	fwrite (bufmem_cls, sizeof(char), (offset_pos - (HDR_SIZE + N_RECORDS * 8)), ucflxfile);

	// Closing...
	fclose (ucflxfile);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Let's print some info about the usage of the tool.
/////////////////////////////////////////////////////////////////////////////////////////
int _Usage() {
	printf ("U8FT - Ultima 8 Flex Tool v1.0\n");
	printf ("==============================\n\n");
	printf ("Usage:\n");
	printf ("u8ft -x|-c <flx file>\n\n");
	printf ("  -x     Extracts objects from <flx file> file in nnnn.cls/nnnn.shp file\n");
	printf ("         format and creates index.txt file with the number (decimal) of\n");
	printf ("         objects and the position relative of each object in <flx file>.\n");
	printf ("\n");
	printf ("  -c     Creates a new <flx file> file from all nnnn.cls/nnnn.shp object\n");
	printf ("         files which number and position are readed from index.txt file.\n");
	printf ("\n");
	printf (" <flx file>   Any .FLX file in U8 format.\n");
	printf ("\n");
	printf ("NOTE:     An object can be compiled data or a shape.\n");
	printf ("\n");
	printf ("Samples:   u8ft -x EUSECODE.FLX   or    u8ft -c EUSECODE.FLX.new\n");
	printf ("\n");

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


// Main program... why I write always something like that here...
int main(int argc, char* argv[])
{
	// First we test if we can continue with this thing.
	// Let's check parameters.
	if (argc != 3) {
		// Print Usage
		_Usage();
		return (0);
	}

	// Prepare the filename
	strcpy (flxname, argv[2]);

	// We do here Exctraction or Import. Let's do...
	if (strcmp(argv[1], "-x") == 0) {
		if (!_extract()) printf ("Extration of classes/shapes from %s... Completed.\n", flxname);
	}
	else if (strcmp(argv[1], "-c") == 0) {
			 if (!_create()) printf ("Creation of %s flex file... Completed.\n", flxname);
	    } else _Usage();

	return 0;
}

