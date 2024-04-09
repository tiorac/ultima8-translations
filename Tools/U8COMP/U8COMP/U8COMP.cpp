// U8COMP.cpp : Defines the entry point for the console application.
//
// U8 Compile Tool 
// ===============
// 
// After having the .txt.new files (pressumably with translated text and
// calculated addresses, we will need now to compile them again in
// Ultima VIII Pagan format, and after that, we can recreate
// EUSECODE.FLX file with U8FT tool.
//
// This utility will create a bunch of nnnn.cls.new files with changed
// code. This includes HEADER and somehow "parsing" the new data
// into binary file.
//
// Usage:
//
// u8comp -a| <class .txt file>
// -a       Will compile all the nnnn.txt.new text based class files into
//			nnnn.cls.new files as per index.txt file.
//			MANDATORY TO HAVE ORIGINAL nnnn.cls class file.
// <class .txt file>    Compile unique class <file>. We don't need index.txt but will need
//						original nnnn.cls class file.
//
// Output: 
// - xxxx.cls.new file(s)	Each one is a new .cls binary class file with its addresses
//							changed.
//
// Sample:
// u8comp -c 0044.txt.new  Creates new 0044.cls.new binary class file with the things changed.
//						   We need original 0044.cls binary class file.
// u8comp -a               Creates a bunch of nnnn.cls.new binary class files with the things
//						   changed.
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "io.h"

#define MAX_SZ_STR		 8192  // Some textbooks surpases 7000bytes.
#define MAX_LEN			  256  // Max length for files and chars table.
#define MAX_NRECORDS	65535  // Let's suppouse a max of 65535 records.
#define BUFFER			65535  // Buffer for reading/writing if needed.
#define IDX_FILE		"index.txt"
#define MAX_HEXCLASS		5  // lenght for var with the class in string format (hexa value).
#define MAX_EVENTS		   32

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
// 10|ADDRESS|SHIFT(FOREACH LIST)

struct DATA_DECOMP_RECORD {
	short type;
	long address;
	char string[MAX_SZ_STR];
	long str_length;
	long str_length_new;
	int shift;
	int u8class;
	int u8func;
	int u8sishift;
};

struct CLS_HDR {
	long offset;
	long size;
	long maxoffset;
	long events[MAX_EVENTS];
};

struct CLS_HDR cls_hdr;
struct DATA_DECOMP_RECORD data_rec[MAX_NRECORDS];
FILE *ftxtin_fp, *fcls_fp, *fileidx_fp;
char ftxt_in[MAX_LEN], fcls_in[MAX_LEN], fcls_out[MAX_LEN], fidx_in[MAX_LEN];
char bufmem_cls[2000000];
char buf[BUFFER], buf_aux[BUFFER], s_buf[MAX_SZ_STR];
long table_counter, displacement, original_size, pos_buffer;


/////////////////////////////////////////////////////////////////////////////////////////
// Helper function to clean data_rec for a new file.
/////////////////////////////////////////////////////////////////////////////////////////
int _empty_data_rec() {
	int a;

	for (a = 0; a < MAX_NRECORDS; a++) {
		data_rec[a].type			= 0;
		data_rec[a].address			= 0;
		data_rec[a].shift			= 0;
		data_rec[a].str_length		= 0;
		data_rec[a].str_length_new  = 0;
		data_rec[a].u8class			= 0;
		data_rec[a].u8func			= 0;
		data_rec[a].u8sishift		= 0;
		data_rec[a].string[0]		= NULL;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Helper function that will put from buf string "count" chars from "init" position
// into s_buf string.
/////////////////////////////////////////////////////////////////////////////////////////
int _str_helper(int init, int count) {
	int pos = 0, counter = 0;

	// Let's check count. -1 it's a string.
	while (pos < count && buf[init+pos] != NULL) {
		s_buf[pos] = buf[init+pos];
		pos++;

		counter--;
	}

	s_buf[pos] = NULL;

	// Let's check if pos-1 is a new_line (<NL>) char '0x0A'.
	// This should not be counted because it is not from original line.
	if (s_buf[pos-1] == 0x0A) s_buf[pos-1] = NULL;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function that will load nnnn.txt file strings into table records for work with.
/////////////////////////////////////////////////////////////////////////////////////////
int _prep_strtable() {
	table_counter = 0;

	// Ok, first thing to do is open original nnnn.txt file.
	if ((ftxtin_fp = fopen (ftxt_in, "rt")) == NULL ) {
		printf ("Error opening class %s for reading.\n", ftxt_in);
		return (-1);
		fcloseall();
	}

	// We read first line.
	fgets (buf, BUFFER, ftxtin_fp);

	// Now until eof, read each line.
	while (!feof(ftxtin_fp)) {
		// OK, let's check type. Depending on type, we will do one thing or another.
		_str_helper(0, 2);
		data_rec[table_counter].type = strtol (s_buf, NULL, 10);

		// Let's put address also in table of records.
		_str_helper(3, 4);
		data_rec[table_counter].address = strtol (s_buf, NULL, 16);

		// Now for each string (TYPE 2), we will recalculate new length...
		switch (data_rec[table_counter].type) {
		case 1:
			// It's a FUNCTION
			// We put function address also in its value.
			_str_helper(8, 4);
			data_rec[table_counter].u8func = strtol (s_buf, NULL, 16);
			break;

		case 2:
			// It's a STRING instruction
			// First we put original string length.
			_str_helper(8, 4);
			data_rec[table_counter].str_length = strtol (s_buf, NULL, 16);

			// Now we put the string.
			_str_helper(13, MAX_SZ_STR);
			strcpy (data_rec[table_counter].string, s_buf);

			// AND finally, we put the new length of the string.
			data_rec[table_counter].str_length_new = strlen (s_buf);

			// No need to do conversion.
			// _cnv_chars();
			break;

		case 3:
		case 4:
			// Here goes the special case for for JMP or JNE OUTSIDE CLASS BOUNDARIES
			// This feature is UNIQUE in 0x040A CLASS. We will treat them the same.
		case 8:
		case 9:
		case 10:
			// It's a SHIFT-JMP or SHIFT JNE instruction
			// Let's put shifting in var of the table.
			_str_helper(8, 4);
			data_rec[table_counter].shift = strtol (s_buf, NULL, 16);
			break;

		case 5:
		case 6:
			// It's a CALL/SPAWN instruction.
			// We put own class and external class addresses anyway.
			_str_helper(8, 4);
			data_rec[table_counter].u8class = strtol (s_buf, NULL, 16);

			// Here we put relative address for this function
			// of CALL/SPAWN instruction.
			_str_helper(13, 4);
			data_rec[table_counter].u8func = strtol (s_buf, NULL, 16);
			break;

		case 7:
			// SPAWN INLINE instruction.
			// We put own class and external class addresses anyway.
			// I decided to treat it apart in the case we need to do something
			// with a displacement that there is afeter u8func value in
			// disassembled code. Sample:
			//     16A3: 58	spawn inline	0583:12DF+03E5=16C4 02 00 (FIRESPEL::12DF+03E5)
			_str_helper(8, 4);
			data_rec[table_counter].u8class = strtol (s_buf, NULL, 16);

			// Here we put relative address for this function
			// of CALL/SPAWN instruction.
			_str_helper(13, 4);
			data_rec[table_counter].u8func = strtol (s_buf, NULL, 16);

			// And here we goes the displacement.
			_str_helper(18, 4);
			data_rec[table_counter].u8sishift = strtol (s_buf, NULL, 16);
			break;

		default:
			printf ("This should not happen.\n");
			break;
		}

		// We read next line and put counter in the next table record.
		fgets (buf, BUFFER, ftxtin_fp);
		table_counter++;
	}

	fclose (ftxtin_fp);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Process to obtain displacement and original_size for all the strings
// in class.
/////////////////////////////////////////////////////////////////////////////////////////
int _get_strsizes() {
	int x;

	displacement = 0;
	original_size = 0;

	for (x = 0; x < table_counter; x++)
		if (data_rec[x].type == 2) {
			original_size = original_size + data_rec[x].str_length;
			displacement = displacement + data_rec[x].str_length_new;
		}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Help function to check if the FUNCTION corresponding in pos_table
// exists in hdr and if it has changed, update it.
/////////////////////////////////////////////////////////////////////////////////////////
int _update_event_hdr() {
	int x = 0, z = 0;
	bool found = false;

	while (z < table_counter) {
		x = 0;

		if (data_rec[z].type == 1) {
			while (x < MAX_EVENTS && !found) {
				if (cls_hdr.events[x] == data_rec[z].address && data_rec[z].address != data_rec[z].u8func) {
					cls_hdr.events[x] = data_rec[z].u8func;
					found = true;
				}
				x++;
			}
		}

		z++; found = false;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Help function to copy data from data between buffers by given length
/////////////////////////////////////////////////////////////////////////////////////////
int _copy_buf(long l_cnt) {
	int x;

	x = 0;

	for (x = 0; x < l_cnt; x++) {
		bufmem_cls[pos_buffer] = buf[x];
		pos_buffer++;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Help function to copy string from record into buffer by given length
/////////////////////////////////////////////////////////////////////////////////////////
int _copy_str(long l_cnt, int pos) {
	int x;

	x = 0;

	for (x = 0; x < l_cnt; x++) {
		bufmem_cls[pos_buffer] = data_rec[pos].string[x];
		pos_buffer++;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Help function to copy long into memory buffer
/////////////////////////////////////////////////////////////////////////////////////////
int _put_bufmem_cls(long length) {

	bufmem_cls[pos_buffer] = length & 0xFF;
	pos_buffer++;
	bufmem_cls[pos_buffer] = length >> 8;
	pos_buffer++;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Process main body to do the compilation.
/////////////////////////////////////////////////////////////////////////////////////////
int _comp() {
	long pos_table = 0,  l_count = 0;
	unsigned char byte;
	unsigned short i_value;

	// Let's begin...
	// First we need to put into table of records the data of the nnnn.txt.new file.
	pos_buffer = 0;
	_prep_strtable();

	// Ok, we need now do some preparations and this will be the process:
	//   1.- We need to open nnnn.cls class file.
	//   2.- Read HDR from original nnnn.cls class file.
	//   3.- We will check type of record and work with it.
	//   4.- Read Data from nnnn.cls file until the address of one of the table records
	//       into buffer.
	//   5.- Write this data readed into buffer memory.
	//   6.- Repeat 3-5 points until end of nnnn.cls file.
	//   7.- Prepare HDR (calculate displacements -original and new- for the strings
	//       and add or substract this displacement).
	//       We update also events AND filesize.
	//   8.- Write new nnnn.cls.new file (write HDR and after BUFMEM_CLS).
	//   

	////////////////////////////////////////////////////////////
	//  1.- We need to open nnnn.cls class file.
	////////////////////////////////////////////////////////////
	if ((fcls_fp = fopen(fcls_in, "rb")) == NULL) {
		printf ("Error opening %s class file for reading.\n", fcls_in);
		return (-1);
	}

	////////////////////////////////////////////////////////////
	//  2.- Read HDR from original nnnn.cls class file.
	////////////////////////////////////////////////////////////
	// We will work with that at the end when writing .txt file.
	fread (&cls_hdr, sizeof(cls_hdr), 1, fcls_fp);

	// Ok. Now we must read nnnn.cls file from the beggining of data (normally 0x8C).
	// Go there, Put file pointer at the beginning of data section.
	fseek (fcls_fp, 0x8C, 0);

	// Let's mount in memory buffer the new cls.new class file.
	while (pos_table < table_counter) {
		// To process this, we will use addresses in the table of records.

		////////////////////////////////////////////////////////////
		//   3.- We will check type of record and work with it.
		////////////////////////////////////////////////////////////
		// Checking type we will know what to do in each case.
		switch (data_rec[pos_table].type) {
		case 1:
			// FUNCTION case
			// We process now nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 2;
			break;

		case 2:
			// STRING case
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Now we should put in buffer size of string (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].str_length_new); 

			// Now the string (we read it also)
			fread (buf_aux, sizeof(unsigned char), data_rec[pos_table].str_length, fcls_fp);
			_copy_str(data_rec[pos_table].str_length_new, pos_table);

			// Now there will go a NULL. I prefer not to put this in the string.
			// Let's put it here.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;			
			
			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 4 + data_rec[pos_table].str_length;
			break;

		case 3:
		case 4:
		case 8:
		case 9:
			// JMP & JNE case
			// We process now ASM instruction of nnnn.cls.
			// We add here as usual JMP & JNE out of boundaries instructions.
			// for 0x040A class.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Now we should put in buffer size of string (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].shift); 

			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 3;
			break;

		case 5:
			// CALL case
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Now we should put in buffer class number (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(i_value); 

			// Now we should put in buffer function address for the class number (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].u8func); 

			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 5;
			break;

		case 6:
			// CALL SPAWN case
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Two more bytes not used right now.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Now we should put in buffer class number (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(i_value); 

			// Now we should put in buffer function address for the class number (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].u8func); 

			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 7;
			break;

		case 7:
			// Special SPAWN INLINE case
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Now we should put in buffer class number (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(i_value); 

			// Now we should put in buffer function address for the class number (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].u8func); 

			// We treat the SPAWN INLINE function shifting.
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].u8sishift); 

			// Two ending bytes.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 9;
			break;
		
		case 10:
			// FOREACH LIST case
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;
			// We process now ASM instruction of nnnn.cls.
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;

			// Now we should put in buffer size of shift (2bytes)
			fread (&i_value, sizeof(unsigned short), 1, fcls_fp);
			_put_bufmem_cls(data_rec[pos_table].shift); 

			// Number of bytes which we had worked/readed.
			// This will be useful for next steps.
			l_count = 5;
			break;

		default:
			break;
		}

		// Ok, solved the table record. Now we must now where is the next address.
		// Let's increment table.
		pos_table++;

		////////////////////////////////////////////////////////////
		//   4.- Read Data from nnnn.cls file until the address of one of the table records
		//       into buffer.
		////////////////////////////////////////////////////////////
		// We now can read all the data from nnnn.cls file until this address.
		l_count = data_rec[pos_table].address - (data_rec[pos_table-1].address + l_count);

		// Let's check if what we must read is > 0.
		// If it is, 
		if (l_count > 0)
			fread (buf, sizeof(char), l_count, fcls_fp);

		////////////////////////////////////////////////////////////
		//   5.- Write this data readed into buffer memory.		
		////////////////////////////////////////////////////////////
		_copy_buf(l_count);

	}

	// Here we have ended the process of the table.
	// Let's finish reading the rest of bytes until end of nnnn.cls file.
	fread (&byte, sizeof(unsigned char), 1, fcls_fp);
	while (!feof(fcls_fp)) {
			bufmem_cls[pos_buffer] = byte;
			pos_buffer++;
			fread (&byte, sizeof(unsigned char), 1, fcls_fp);
	}

	////////////////////////////////////////////////////////////
	//   7.- Prepare HDR (calculate displacements -original and new-
	//       for the strings and add or substract this displacement).
	////////////////////////////////////////////////////////////
	// We need displacement and original_size vars.
	_get_strsizes();

	cls_hdr.offset = cls_hdr.offset - original_size + displacement;
	cls_hdr.maxoffset = cls_hdr.maxoffset - original_size + displacement;

	// Now we can updte events table.
	_update_event_hdr();

	// Now we have the size. This would be pos_buffer + HDR (0x8C)
	cls_hdr.size = pos_buffer + 0x8C;

	// Now we can close nnnn.cls original file.
	fclose (fcls_fp);


	////////////////////////////////////////////////////////////
	//   8.- Write new nnnn.cls.new file (write HDR and after BUFMEM_CLS).
	////////////////////////////////////////////////////////////

	//  We will open new nnnn.cls.new class file for work with.
	if ((fcls_fp = fopen(fcls_out, "wb")) == NULL) {
		printf ("Error opening .%s class file for writing.\n", fcls_out);
		return (-1);
	}

	// Write HDR
	fwrite (&cls_hdr, sizeof(CLS_HDR), 1, fcls_fp);

	// Write data.
	fwrite (bufmem_cls, sizeof(unsigned char), pos_buffer, fcls_fp);

	// Done!!
	fclose (fcls_fp);
	printf ("Compiled %s new class file... Done!\n", fcls_out);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Let's print some info about the usage of the tool.
/////////////////////////////////////////////////////////////////////////////////////////
int _Usage() {
	printf ("U8COMP - Ultima 8 Compile Tool v1.0\n");
	printf ("===================================\n\n");
	printf ("Usage:\n");
	printf ("u8comp <nnnn.txt.new file> | -a\n\n");
	printf (" - <nnnn.txt.new file>: One of the files that has recalculated\n");
	printf ("                  addresses with U8CALC tool.We need original\n"),
	printf ("                    nnnn.cls class file.\n");
	printf (" -a              :  Compiles ALL the files that have recalculated\n");
	printf ("                    addresses with U8CALC tool. We need original\n"),
	printf ("                    nnnn.cls class files.\n");
	printf ("                    Each file is known by index.txt file.\n");
	printf ("\n");
	printf ("Output:\n");
	printf ("  <nnnn.cls.new file>   This creates nnnn.cls.new binary file.\n");
	printf ("\n");
	printf ("Samples:   u8comp 0044.txt.new    (door class)\n");
	printf ("           u8comp -a\n");
	printf ("\n");

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


// Main body of tool...
int main(int argc, char* argv[])
{
	char *p;
	int result;

	// First we test if we can continue with this thing.
	// Let's check parameters.
	if (argc != 2) {
		// Print Usage
		_Usage();
		return (0);
	}

	// Let's check arguments with argc.
	if (argv[1] != NULL) {
		if (strcmp(argv[1], "-a") == 0) {
			// Compile all...
			// We will read each file from index.txt file.
			// CLASSES for GLOBALS & CLASS NAME should be avoid.

			// We open here the index.txt file for read each class.
			if ((fileidx_fp = fopen(IDX_FILE, "rt")) == NULL) {
				printf ("Error opening index.txt file for reading.\n", fileidx_fp);
				return (-1);
			}

			// Read first and second lines (GLOBALS & CLASSES), and
			// a third line which would be the useful line.
			fgets (fidx_in, MAX_LEN, fileidx_fp);
			fgets (fidx_in, MAX_LEN, fileidx_fp);
			fgets (fidx_in, MAX_LEN, fileidx_fp);

			// For each line, do the process.
			while (!feof(fileidx_fp)) {
				// If file not exists, let's print something bug continue
				// with process. Ok, only file here, no function position.
				p = strstr (fidx_in, ";");
				result = (int)(p - fidx_in + 1);			
				fidx_in[result-1] = NULL;

				if ((_access(fidx_in, 0)) == -1) {
					// File not exists
					// Let's print something...
					printf ("Original %s class file... No exists.\n", fidx_in);
				} else {
					strcpy (ftxt_in, fidx_in);
					strcpy (fcls_in , fidx_in);
					strcpy (fcls_out , fidx_in);

					// We change .cls for .txt in ftxtname file
					p = strstr (fcls_in, ".cls");
					result = (int)(p - fcls_in + 1);
					ftxt_in[result] = 't'; ftxt_in[result+1] = 'x'; ftxt_in[result+2] = 't'; ftxt_in[result+3] = '.'; 
					ftxt_in[result+4] = 'n'; ftxt_in[result+5] = 'e'; ftxt_in[result+6] = 'w'; ftxt_in[result+7] = NULL; 
					//fcls_in[result] = 'c'; fcls_in[result+1] = 'l'; fcls_in[result+2] = 's'; fcls_in[result+3] = NULL;
					fcls_out[result] = 'c'; fcls_out[result+1] = 'l'; fcls_out[result+2] = 's'; fcls_out[result+3] = '.'; 
					fcls_out[result+4] = 'n'; fcls_out[result+5] = 'e'; fcls_out[result+6] = 'w'; fcls_out[result+7] = NULL; 

					if ((_access(ftxt_in, 0)) == -1) {
						// There is no modified nnnn.txt.new file. No need to do anything either.
						// Let's print something...
						printf ("Modified %s text file... No exists.\n", ftxt_in);
					} else {
						// We call the _decomp process...
						_comp();

						// Let's make a bit of cleaning.
						_empty_data_rec();
					}

				}
				// Next class file of index.txt.
				fgets (fidx_in, MAX_LEN, fileidx_fp);
			}

			// We've done with index.txt file
			fclose (fileidx_fp);

		} else {
			// Compile unique file...
			// Extract only concrete file.
			strcpy (ftxt_in, argv[1]);
			strcpy (fcls_in , argv[1]);
			strcpy (fcls_out , argv[1]);

			// We change .cls for .txt in ftxtname file
			p = strstr (fcls_in, ".txt");
			result = (int)(p - fcls_in + 1);
			fcls_in[result] = 'c'; fcls_in[result+1] = 'l'; fcls_in[result+2] = 's'; fcls_in[result+3] = NULL;
			fcls_out[result] = 'c'; fcls_out[result+1] = 'l'; fcls_out[result+2] = 's'; fcls_out[result+3] = '.'; 
			fcls_out[result+4] = 'n'; fcls_out[result+5] = 'e'; fcls_out[result+6] = 'w'; fcls_out[result+7] = NULL; 

			// We call the _decomp process...
			_comp();
		}
	} else _Usage();

	return 0;
}

