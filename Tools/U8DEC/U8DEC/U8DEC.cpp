// U8DEC.cpp : Defines the entry point for the console application.
//
// U8 Decompile Tool (Only extract)
// ================================
// 
// Here it is the tool to extract useful information from .cls files
// extracted from EUSECODE. I say it decompile, but well, maybe it is not
// a decompilation at all.
//
// Extracts the important part for translation, and posterior compilation
// from each .cls file. This includes, jumps, pointers to another
// classes (calls/spawns) and classes itself to check between files.
// 
// Usage:
//
// u8dec <file>
// - Decompiles xxxx.cls file (more or less) and creates xxxx.txt file with
// important data.
//
// Output: 
// - xxxx.txt files (each one is a class with its functions)
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "io.h"

#define MAX_EVENTS		   32
#define MAX_SZ_STR		 8192  // Some textbooks surpases 7000bytes.
#define MAX_LEN			  256  // Max length for files.
#define MAX_NRECORDS	65535  // Let's suppouse a max of 65535 records.
#define IDX_FILE		"index.txt"

struct CLS_HDR {
	long offset;
	long size;
	long maxoffset;
	long events[MAX_EVENTS];
};


// Format in extracted .txt file for decompilation
// 01|ADDRESS|(FUNCTION/ORIGINAL ADDRESS)
// 02|ADDRESS|LENGTH|STRING
// 03|ADDRESS|SHIFT(JMP)
// 04|ADDRESS|SHIFT(JNE)
// 05|ADDRESS|CLASS:FUNC(CALL)
// 06|ADDRESS|CLASS:FUNC(SPAWN)
// 07|ADDRESS|CLASS:FUNC(SPAWNINLINE)
// 08|ADDRESS|SHIFT(JMP) OUTSIDE CLASS BOUNDARIES
// 09|ADDRESS|SHIFT(JNE) OUTSIDE CLASS BOUNDARIES
// 10|ADDRESS|SHIFT(FOREACH)

struct DATA_DECOMP_RECORD {
	int type;
	long address;
	char string[MAX_SZ_STR];
	long str_length;
	int shift;
	int u8class;
	int u8func;
	int u8sishift;
};

struct CLS_HDR cls_hdr;
struct DATA_DECOMP_RECORD data_rec[MAX_NRECORDS];
FILE *file_fp;
char fclsname[MAX_LEN], ftxtname[MAX_LEN];
long x, total_filesize;


/////////////////////////////////////////////////////////////////////////////////////////
// Function to determine if the active record should be written or not...
/////////////////////////////////////////////////////////////////////////////////////////
bool _write_record(int pos) {
	bool b_write_record = false;
	int c = 0;
	long t_address;

	// Ok, for TYPEs, 1, 2, 5, 6, 7, at this stage there is no need to do anything.
	// It should be written anyway.
	switch (data_rec[pos].type) {
		// This is what we can avoid, some JMP and JNE instructions that its shifting
		// will not be modified after recompilation.
		// This shifting can be modified by:
		// - the instruction points after or before
		//   text (or various text) or functions (which they can be modified also).
		case 3:
		case 4:
			// We search if there is any text between the JMP & JNE instruction
			// and the shifting.
			// The shifting can be added or substracted.
			// So we have here from 32767(0x7FFF) to -32767(0xFFFF).
			// There is another possibility, where shifting surprasses
			// address size of file itself OR it is 0 -zero-... WHAT? Yes.
			// We write them anyway (about zero is solved, but what happens
			// with shift bigger than the size of file??
			if (data_rec[pos].shift > 0) {
				// Ok, begin counting from actual address position.
				c = pos;

				if (data_rec[pos].shift <= 0x7FFF) {
					// Positive shifting
					if ((data_rec[pos].shift + data_rec[pos].address) < total_filesize) {

						t_address = data_rec[pos].address + data_rec[pos].shift;

						while ((data_rec[c].address < t_address) && !b_write_record) {
							if (data_rec[c].type == 2 && data_rec[c].str_length > 0) b_write_record = true;
							c++;
						}
					} else b_write_record = true;
				}
				else {
					// Negative shifting
					if ((data_rec[pos].address - (0xFFFF - data_rec[pos].shift)) > 0) {

						t_address = data_rec[pos].address - (0xFFFF - data_rec[pos].shift);

						while ((data_rec[c].address > t_address) && !b_write_record) {
							if (data_rec[c].type == 2 && data_rec[c].str_length > 0) b_write_record = true;
							c--;
						}
					} else b_write_record = true;
				}
			} //else b_write_record = true; FOR NOW LET'S NOT PRINT THIS.

			if (data_rec[pos].shift == 0) printf ("JMP or JNE shifting is: [0000].\n");
			else
				if (data_rec[pos].shift <= 0x7FFF) {
					if ((data_rec[pos].shift + data_rec[pos].address) > total_filesize) {
						printf ("JMP or JNE shifting ADRESSES file at: [%04X] by +[%04X].\n", data_rec[pos].address, total_filesize - (data_rec[pos].shift + data_rec[pos].address));
						if (data_rec[pos].type == 3) data_rec[pos].type = 8;
						else data_rec[pos].type = 9;
					}
				} else if ((data_rec[pos].address - (0xFFFF - data_rec[pos].shift)) < 0) {
						printf ("JMP or JNE shifting ADRESSES file at: [%04X] by -[%04X].\n", data_rec[pos].address, data_rec[pos].address - (0xFFFF - data_rec[pos].shift));
						if (data_rec[pos].type == 4) data_rec[pos].type = 8;
						else data_rec[pos].type = 9;
					}
		break;

		case 10:
			// If the shifting in FOREACH LIST has text, we must write it into decompiled file.
			if (data_rec[pos].shift > 0) {
				// Ok, begin counting from actual address position.
				c = pos;

				// Positive shifting
				if ((data_rec[pos].shift + data_rec[pos].address) < total_filesize) {

					t_address = data_rec[pos].address + data_rec[pos].shift;

					while ((data_rec[c].address < t_address) && !b_write_record) {
						if (data_rec[c].type == 2 && data_rec[c].str_length > 0) b_write_record = true;
						c++;
					}
				} else b_write_record = true;
			}
			break;

		default:
			b_write_record = true;
			break;
	}

	return b_write_record;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Process for write into txt file the table of records...
/////////////////////////////////////////////////////////////////////////////////////////
int write_txt() {
	int cnt = 0;

	// We open here the .txt file for output.
	if ((file_fp = fopen(ftxtname, "wt")) == NULL) {
		printf ("Error opening .txt file %s for writing.\n", ftxtname);
		return (-1);
		fcloseall();
	}

	// Process...
	while (cnt < x) {
		// TYPE
		// Ok, we should check if this TYPE of line should be written or not.
		// Why? Well, we need to know if there would be any change in the size
		// of the shifting between the code.
		// Normally, JMP, JNE and FUNCTION TYPEs must be checked.
		if (_write_record(cnt)) {

			// Let's get TYPE for further purposes.
			fprintf (file_fp, "%02d", data_rec[cnt].type);

			// Separator
			fputs ("|", file_fp);

			// Address
			fprintf (file_fp, "%04X", data_rec[cnt].address);

			// Separator
			fputs ("|", file_fp);

			// Ok. Depending on type, we should do one or another thing.
			switch (data_rec[cnt].type) {
				case 1:
					// Here goes FUNCTION
					// We put a second time the function. This is for further recompilation.
					fprintf (file_fp, "%04X", data_rec[cnt].u8func);
					break;

				case 2:
					// STRING
					// Let's write LENGTH
					fprintf (file_fp, "%04X", data_rec[cnt].str_length);
					// Separator
					fputs ("|", file_fp);
					// Let's write STRING
					fputs(data_rec[cnt].string, file_fp);
					break;

				case 3:
				case 4:
				case 8:
				case 9:
				case 10:
					// JMP & JNE & FOREACH LIST
					// Let's write SHIFT
					fprintf (file_fp, "%04X", data_rec[cnt].shift);
					break;

				case 5:
				case 6:
					// CALL & SPAWN
					// Let's write CLASS:FUNC
					fprintf (file_fp, "%04X", data_rec[cnt].u8class);
					fputs(":", file_fp);
					fprintf (file_fp, "%04X", data_rec[cnt].u8func);
					break;

				case 7:
					// SPAWN INLINE
					// Let's write CLASS:FUNC|SISHIFT
					fprintf (file_fp, "%04X", data_rec[cnt].u8class);
					fputs(":", file_fp);
					fprintf (file_fp, "%04X", data_rec[cnt].u8func);
					fputs("|", file_fp);
					fprintf (file_fp, "%04X", data_rec[cnt].u8sishift);
					break;

				default:
				break;
			}

			// New Line
			fputs ("\n", file_fp);
		}
		// Increment record table to work with
		cnt = cnt + 1;
	} 

	// Let's close here .txt file.
	fclose(file_fp);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Body of _decomp process...
/////////////////////////////////////////////////////////////////////////////////////////
int _decomp() {
	unsigned short i_value=0;
	char buffer[MAX_SZ_STR];
	unsigned char byte, tmp_byte;
	long cnt_address = 0;

	// We open here the file for input.
	if ((file_fp = fopen(fclsname, "rb")) == NULL) {
		printf ("Error opening .cls file %s for reading.\n", fclsname);
		return (-1);
	}

	// Know filesize of file.
	fseek(file_fp, 0, SEEK_END);
	total_filesize = ftell (file_fp);
	fseek(file_fp, 0, SEEK_SET);

	// Let's read first the .cls file header
	// We will work with that at the end when writing .txt file.
	fread (&cls_hdr, sizeof(cls_hdr), 1, file_fp);

	// Let's decompile... 
	// Read first byte.
	fread (&byte, sizeof(unsigned char), 1, file_fp);
	// Initialize address
	cnt_address = 0x80;

	//We read until the end of file
	while (!feof(file_fp))
	{
		// This if when we found an instruction not checked.
		tmp_byte = byte;

		// Here we try to check every byte to see we're talking about.
		switch (byte)
		{
				case 0x08:
				case 0x12:
				case 0x14:
				case 0x15:
				case 0x16:
				case 0x17:
				case 0x1C:
				case 0x1E:
				case 0x20:
				case 0x22:
				case 0x24:
				case 0x26:
				case 0x28:
				case 0x29:
				case 0x2A:
				case 0x2C:
				case 0x2D:
				case 0x2E:
				case 0x2F:
				case 0x30:
				case 0x32:
				case 0x34:
				case 0x36:
				case 0x39:
				case 0x3A:
				case 0x3B:
				case 0x3C:
				case 0x3D:
				case 0x50:
				case 0x53:
				case 0x59:
				case 0x5D:
				case 0x5E:
				case 0x5F:
				case 0x60:
				case 0x61:
				case 0x6B:
				case 0x6D:
				case 0x73:
				case 0x77:
				case 0x78:
				case 0x79:
					// NOTHING TO READ
				break;
		
				case 0x00:
				case 0x01:
				case 0x02:
				case 0x0A:
				case 0x19:
				case 0x1A:
				case 0x1B:
				case 0x3E:
				case 0x3F:
				case 0x40:
				case 0x41:
				case 0x43:
				case 0x4B:
				case 0x4C:
				case 0x4D:
				case 0x62:
				case 0x63:
				case 0x64:
				case 0x65:
				case 0x66:
				case 0x67:
				case 0x69:
				case 0x6E:
				case 0x6F:
				case 0x74:
					// NOT USED
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+1;
				break;

				case 0x03:
				case 0x0B:
				case 0x0E:
				case 0x38:
				case 0x42:
				case 0x44:
				case 0x45:
				case 0x54:
				case 0x6C:
					// NOT USED
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+2;
				break;


				case 0x09:
				case 0x0F:
				case 0x4E:
				case 0x4F:
				case 0x70:
					// NOT USED
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+3;
				break;

				case 0x0C:
				case 0x76:
					// NOT USED
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+4;
				break;

				// ##############################
				// TYPE 2 (STRING)
				// ##############################
				case 0x0D:
					// Only string
					// Set TYPE
					data_rec[x].type = 2;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// In this case, let's read first the length of the string.
					// We will use "i_value" var anyway.
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].str_length = i_value;
					cnt_address = cnt_address + 2;

					// Ok, now it's time to read string into buffer.
					fread (buffer, sizeof(char), i_value+1, file_fp);
					cnt_address = cnt_address + i_value + 1;

					// Copy buffer into structure.
					strcpy (data_rec[x].string, buffer);

					// Next record in the table for this purpose.
					x++;
				break;

				// ##############################
				// TYPE 5 (CALL)
				// ##############################
				case 0x11:
					// This is an instruction CALL to a class:function we will use.
					// Set TYPE
					data_rec[x].type = 5;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// Read and Set CLASS 
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8class = i_value;

					// Read and Set FUNCTION
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8func = i_value;

					// Let's add the 4bytes to the address counter.
					cnt_address = cnt_address+4;

					// Next record in the table for this purpose.
					x++;
				break;

				// ##############################
				// TYPE 4 (JNE)
				// ##############################
				case 0x51:
					// This is a jump instruction.
					// Set TYPE
					data_rec[x].type = 4;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// Let's read the shift (2bytes)
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].shift = i_value;

					// Let's add the 2bytes to the address counter.
					cnt_address = cnt_address+2;

					// Next record in the table for this purpose.
					x++;
				break;

				// ##############################
				// TYPE 3 (JMP)
				// ##############################
				case 0x52:
					// This is a jump instruction.
					// Set TYPE
					data_rec[x].type = 3;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// Let's read the shift (2bytes)
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].shift = i_value;

					// Let's add the 2bytes to the address counter.
					cnt_address = cnt_address+2;

					// Next record in the table for this purpose.
					x++;
				break;

				// ##############################
				// TYPE 6 (SPAWN)
				// ##############################
				case 0x57:
					// This is a SPAWN instruction that somehow uses a class function.
					// Set TYPE
					data_rec[x].type = 6;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// We need here the CLASS/FUNCTION pair.
					// We read something more until we arrive to them.
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+2;

					// Read and Set CLASS 
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8class = i_value;

					// Read and Set FUNCTION
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8func = i_value;

					// Let's add the 4bytes to the address counter.
					cnt_address = cnt_address+4;

					// Next record in the table for this purpose.
					x++;
				break;

				// ##############################
				// TYPE 7 (SPAWN INLINE)
				// ##############################
				case 0x58:
					// This is a SPAWN INLINE instruction that somehow uses a class function.
					// Set TYPE
					data_rec[x].type = 7;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// We need here the CLASS/FUNCTION pair.
					// Read and Set CLASS 
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8class = i_value;

					// Read and Set FUNCTION
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8func = i_value;

					// Let's add the 4bytes to the address counter.
					cnt_address = cnt_address+4;

					// HERE we goes something like a displacement.
					// I don't know if it is needed.
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].u8sishift = i_value;
					cnt_address = cnt_address+2;

					// Two bytes more, I assume not needed.
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+2;

					// Next record in the table for this purpose.
					x++;
				break;

				// ##############################
				// TYPE 1 (FUNCTION)
				// ##############################
				case 0x5A:
					// This is not a instruction, but we will use it for recompilation
					// purposes. At the extraction, ADDRESS AND FUNCTION are the
					// same but maybe we will change it in future at recompilation.
					// Set TYPE
					data_rec[x].type = 1;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// Set FUNCTION
					// We will use u8func var of structure
					data_rec[x].u8func = cnt_address;

					// End treatement, count bytes not used right now for this
					// code, and read the bytes not used also.
					// We should count cnt_adress also having in mind the
					// bytes readed.
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+1;

					// Next record in the table for this purpose.
					x++;
				break;

			// ##############################
			// TYPE 10 (FUNCTION)
			// ##############################
			case 0x75:
					// This is a FOREACH LIST instruction that somehow makes a shifting.
					// If it has text in the middle, the difference in translated
				    // text must be counted.
					// Set TYPE
					data_rec[x].type = 10;

					// Set ADDRESS
					data_rec[x].address = cnt_address;

					// Two bytes more, I assume not needed.
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					fread (&byte, sizeof(unsigned char), 1, file_fp);
					cnt_address = cnt_address+2;

					// Read and Set SHIFTING
					fread (&i_value, sizeof(unsigned short), 1, file_fp);
					data_rec[x].shift = i_value;
					cnt_address = cnt_address+2;

					// Next record in the table for this purpose.
					x++;
				break;

			default:
				printf ("Function NOT FOUND: 0x%02X\n", tmp_byte);
				break;
		}

		// We read next byte ¿Sure?
		 fread (&byte, sizeof(unsigned char), 1, file_fp);
		 cnt_address = cnt_address+1;
	}

	// Let's close here .cls file.
	fclose(file_fp);

	// We have all the needed info. We will put it into a text file.
	write_txt();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Let's print some info about the usage of the tool.
/////////////////////////////////////////////////////////////////////////////////////////
int _Usage() {
	printf ("U8DEC - Ultima 8 Decompile Tool v1.0\n");
	printf ("====================================\n\n");
	printf ("Usage:\n");
	printf ("u8dec <nnnn.cls file> | -a\n\n");
	printf (" - <nnnn.cls file>: One of the files extracted with U8FT tool.\n");
	printf (" -a               : Decompiles ALL the files extracted with U8FT\n");
	printf ("                    tool. Each file is known by index.txt file.\n");
	printf ("\n");
	printf ("Output:\n");
	printf ("  <nnnn.txt file>   This creates nnnn.txt file for editing.\n");
	printf ("\n");
	printf ("Samples:   u8dec 0044.cls    (door class)\n");
	printf ("\n");

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
	FILE *idx_fp;
	char fname[MAX_LEN];
	char *p;
	int result;

	// First we test if we can continue with this thing.
	// Let's check parameters.
	if (argc != 2) {
		// Print Usage
		_Usage();
		return (0);
	}

	// Let's see if we have here something for file...
	if (argv[1] != NULL) {
		if (strcmp(argv[1], "-a") == 0) {
			// Decompile all...
			// We will read each file from index.txt file.
			// CLASSES for GLOBALS & CLASS NAME should be avoid.
			
			// We open here the index.txt file for read each class.
			if ((idx_fp = fopen(IDX_FILE, "rt")) == NULL) {
				printf ("Error opening index.txt file for reading.\n", idx_fp);
				return (-1);
			}

			// Read first and second lines (GLOBALS & CLASSES), and
			// a third line which would be the useful line.
			fgets (fname, MAX_LEN, idx_fp);
			fgets (fname, MAX_LEN, idx_fp);
			fgets (fname, MAX_LEN, idx_fp);

			// For each line, do the process.
			while (!feof(idx_fp)) {
				// If file not exists, let's print something bug continue
				// with process. Ok, only file here, no function position.
				p = strstr (fname, ";");
				result = (int)(p - fname + 1);			
				fname[result-1] = NULL;

				if ((_access(fname, 0)) == -1) {
					// File not exists
					// Let's print something...
					printf ("Class %s... No exists.\n", fname);
				} else {
					// Extract class.
					strcpy (fclsname, fname);
					strcpy (ftxtname, fname);

					// We change .cls for .txt in ftxtname file
					p = strstr (ftxtname, ".cls");
					result = (int)(p - ftxtname + 1);
					ftxtname[result] = 't'; ftxtname[result+1] = 'x'; ftxtname[result+2] = 't';

					// We should clear everything in struct previously used.
					// Put 0 in TYPE should be fine.
					// AND initialize x var for initial position of table.
					for (int z = 0; z < MAX_NRECORDS; z++) {
						data_rec[z].type       = 0;
						data_rec[z].address    = 0;
						data_rec[z].shift      = 0;
						data_rec[z].str_length = 0;
						data_rec[z].u8class    = 0;
						data_rec[z].u8func     = 0;
						data_rec[z].u8sishift  = 0;
						data_rec[z].string[0]  = NULL;
					}
					x = 0;

					// We call the _decomp process...
					_decomp();

					// Let's print something...
					printf ("Class %s... Done.\n", fclsname);
				}

				fgets (fname, MAX_LEN, idx_fp);
			}

			// We've done. Close index.txt file
			fclose (idx_fp);

		} else {	
			// Extract only concrete file.
			strcpy (fclsname, argv[1]);
			strcpy (ftxtname, argv[1]);

			// We change .cls for .txt in ftxtname file
			p = strstr (ftxtname, ".cls");
			result = (int)(p - ftxtname + 1);
			ftxtname[result] = 't'; ftxtname[result+1] = 'x'; ftxtname[result+2] = 't';

			// We call the _decomp process...
			_decomp();
		}

	} else _Usage();

	return 0;
}

