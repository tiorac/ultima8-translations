// U8CALC.cpp : Defines the entry point for the console application.
//
// U8 ReCalc Tool 
// ==============
// 
// After having decompiled the code of class .cls files into editable
// .txt files, this tool pretends to recalculate addresses in the class
// (or classes) and creates a new nnnn.txt.new file with all this modified.
//
// This utility will contain this features:
//   a) Recalculate a unique class addresses only modifying JMP, JNE and
//		SPAWN/CALL own instructions. Let's say this is level 0.
//		This will create a nnnn.txt.new file with modified addresses.
//
//   b) Recalculate a unique class addresses with own instructions AND
//		external classes addresses. Here we will need to have extrated
//		all the .cls classes into .txt files. Let's say this is level 1.
//	    This will create a nnnn.txt.new file with all needed addresses
//		modified.
//		
//   c) Recalculate all the addresses for all the classes with all the features.
//		This will create a bunch of nnnn.txt.new files with modified addresses.
//
// Usage:
//
// u8calc -a|-c <file> [-l]
// -a       Recalculates all addresses for all classes. Full feature.
// -c       Recalculates unique class addresses. Only some instructions.
// <file>   nnnn.txt class file to recalculate addresses. Works with -c ONLY.
// -l		Optional. Recalculates unique class addresses. Full feature.
//
// Output: 
// - xxxx.txt.new file(s)	Each one is a new .txt file class with its addresses
//							changed.)
//
// Sample:
// u8calc -c 0044.txt      Creates new 0044.txt.new file with only some own class addresses changed,
//						   like the ones for JMP & JNE and own CALL/SPAWN instructions.
// u8calc -c 0044.txt -l   Creates new 0044.txt.new file with all the addresses needed changed, including
//                         external CALL/SPAWN instructions. Although not needed the external .txt classes
//						   used, this should be done in an ending stage.
// u8calc -a               Creates a bunch of nnnn.txt.new files with all the addresses changed. Needs index.txt
//						   from U8FT tool.
//
// If you want, you can put a TABLE.TXT file with character conversion.
// This helps to change special spanish chars (or maybe from another language),
// into chars defined in u8fonts.
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

struct CHARS_TABLE {
	unsigned char c_subs;
	unsigned char c_ori;
};

struct DATA_DECOMP_RECORD data_rec[MAX_NRECORDS];
struct CHARS_TABLE chars_table[MAX_LEN];
FILE *filein_fp, *fileout_fp, *fileidx_fp, *filerec_fp;
char ftxt_in[MAX_LEN], ftxt_out[MAX_LEN], fidx_in[MAX_LEN], filerec[MAX_LEN];
char buf[BUFFER], s_buf[MAX_SZ_STR];
long table_counter;
int fnum, num_chars;

/////////////////////////////////////////////////////////////////////////////////////////
// Helper function to clean data_rec for a new file.
/////////////////////////////////////////////////////////////////////////////////////////
int _empty_data_rec() {
	int a;

	for (a = 0; a < MAX_NRECORDS; a++) {
		data_rec[a].type = 0;
		data_rec[a].address = 0;
		data_rec[a].shift = 0;
		data_rec[a].str_length = 0;
		data_rec[a].str_length_new  = 0;
		data_rec[a].u8class = 0;
		data_rec[a].u8func = 0;
		data_rec[a].u8sishift = 0;
		data_rec[a].string[0] = NULL;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Helper function that will put from buf string "count" chars from "init" positionç
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
// Function that will load in memory the table of character conversion
// in table.txt.
/////////////////////////////////////////////////////////////////////////////////////////
int _load_chartable() {
	num_chars = 0;
	char code[3];

	// Ok, first thing to do is open original nnnn.txt file.
	if ((filein_fp = fopen ("table.txt", "rt")) == NULL ) {
		printf ("Error opening table.txt for reading (optional).\n", ftxt_in);
		return (-1);
		fcloseall();
	}

	fgets (buf, 256, filein_fp);

	while (!feof(filein_fp)) {
		// Format is A0=á; A2=é. Treat this...
		code[0] = buf[0]; code[1] = buf[1]; code[2] = NULL;

		chars_table[num_chars].c_subs = strtol(code, NULL, 16);
		chars_table[num_chars].c_ori = buf[3];

		// Read next line.
		fgets (buf, 256, filein_fp);
		num_chars++;
	}

	fclose (filein_fp);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function to convert chars in a string as per table conversion
// We will do this for all the table strings at once. We need to call it separately in
// -c, -c with -l and -a parameters of the tool.
/////////////////////////////////////////////////////////////////////////////////////////
int _cnv_chars() {
	int y = 0, z = 0;
	long l_table_counter = 0;
	bool found = false;

	while (l_table_counter < table_counter) {
		if (data_rec[l_table_counter].type == 2)
			for (z = 0; z < data_rec[l_table_counter].str_length_new; z++) {
				// For each char, we should see if it must be changed.
				while (y < num_chars && !found) {
					if (chars_table[y].c_ori == (unsigned char)data_rec[l_table_counter].string[z]) {
						data_rec[l_table_counter].string[z] = chars_table[y].c_subs;
						found = true;
					}	
					y++;
				}
				y = 0; found = false;
			}
		l_table_counter++;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function that will load nnnn.txt file strings into table records for work with.
/////////////////////////////////////////////////////////////////////////////////////////
int _prep_strtable() {
	table_counter = 0;

	// Ok, first thing to do is open original nnnn.txt file.
	if ((filein_fp = fopen (ftxt_in, "rt")) == NULL ) {
		printf ("Error opening class %s for reading.\n", ftxt_in);
		return (-1);
		fcloseall();
	}

	// We read first line.
	fgets (buf, BUFFER, filein_fp);

	// Now until eof, read each line.
	while (!feof(filein_fp)) {
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
			break;

		case 3:
		case 4:
			// Here goes also the special case for for JMP or JNE OUTSIDE CLASS BOUNDARIES
			// This feature is UNIQUE in 0x040A CLASS.
			// Yes, it is the same as the JMP & JNE instructions in fact of writing
			// a Calculated .txt.new file.
		case 8:
		case 9:
		case 10:
			// It's a SHIFT-JMP, SHIFT JNE or FOREACH LIST instruction
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
			// of SPAWN INLINE instruction.
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
		fgets (buf, BUFFER, filein_fp);
		table_counter++;
	}

	fclose (filein_fp);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Function that will recalculate only own FUNC, JNE, JMP, CALL & SPAWN own addresses
// of the own class file. Here we do not recalculate external CALL & SPAWN adreeses.
/////////////////////////////////////////////////////////////////////////////////////////
int _recalc_partial() {
	int to_address = 0, original_length = 0, displacement = 0;
	long pos_table = 0, work_record = 0;
	unsigned long to_address_outofbounds = 0, future_address = 0, address_displacement = 0, address_original_length = 0;

	// Ok, first thing to do is open original nnnn.txt file and get all the strings.
	// We will put them into a table. We will load also into a table in memory
	// the list of conversion characters (if any table.txt file exists).
	_load_chartable();
	_prep_strtable();

	// All right, now we have table with records of nnnn.txt file in memory.
	// Let's work with them. Also, we have updated the new length for the
	// strings when reading the table.

	// The process here is to look for JMP (3), JNE (4) and CALL/SPAWN (5,6,7)
	// instructions with the same function number as the class we are working
	// with.
	// We will seek for each of these instructions if it need to be modified.
	// Instruction begins in X address and goes to X+SHIFT address (or X-SHIFT)
	// address.
	// We must check all the strings between this addresses and add (or substract)
	// diference in shifting. The same for CALL/SPAWN instructions.
	// We must do three iterations:
	//     1.- We will change JMP/JNE shiftings displacements
	//     2.- We will change FUNCTIONS addresses having in mind new strings lengths.
	//     3.- We will change finally CALL/SPAWN instructions addresses having in mind
	//         new addresses changed in point 2.
	//	   3b.- This special condition is for SPAWN INLINE instructions.

	//////////////////////////////////////////////////////////////////////////////
	//
	//    1.- We will change JMP/JNE/FOREACH LIST shiftings displacements
	//    2.- We should also change FUNCTIONs offsets if there are any
	//        any displacements.
	//
	//////////////////////////////////////////////////////////////////////////////
	while (work_record < table_counter) {
		// Let's see we have in the record.
		switch (data_rec[work_record].type) {
		// Case for normal JMP & JNE instructions.
		case 3:
		case 4:
			// We have here JMP or JNE instruction. It does not matter which,
			// the process is the same.
			// Let's check as per shift, if there is any string record.
			// We should know if shift if going up or down.
			if (data_rec[work_record].shift < 0x7FFF) {
				// We must go down (add)
				pos_table = work_record+1;
				// Prepare var with address until go.
				to_address = data_rec[work_record].address + data_rec[work_record].shift;

				// Check if there is any string until address+shift.
				while (data_rec[pos_table].address < to_address && pos_table < table_counter) {
					if (data_rec[pos_table].type == 2) {
						// We found string. Let's increment the displacement count.
						displacement = displacement + data_rec[pos_table].str_length_new;
						original_length = original_length + data_rec[pos_table].str_length;
					}				

					pos_table++;
				}

				// Ok, we know the displacement. We can update shifting in table.
				data_rec[work_record].shift = data_rec[work_record].shift - original_length + displacement;

			} else {
				// We must go up (substract)
				pos_table = work_record-1;

				// Prepare var with address until go.
				to_address = data_rec[work_record].address - (0xFFFF - data_rec[work_record].shift + 2); // For 2byte offset of JMP/JNE;

				// Check if there is any string until address-shift.
				while (data_rec[pos_table].address > to_address ) {
					if (data_rec[pos_table].type == 2) {
						// We found string. Let's increment the displacement count.
						displacement = displacement + data_rec[pos_table].str_length_new;
						original_length = original_length + data_rec[pos_table].str_length;
					}

					pos_table--;
				}

				// Ok, we know the distance. We can update shifting in table.
				data_rec[work_record].shift = data_rec[work_record].shift + original_length - displacement;
			}
			break;

		// Case for OFF BOUNDARIES for JMP & JNE instructions. Particular case for CLASS 0x040A.
		case 8:
		case 9:
			// Samples: 08|B989|6711 ->> Displacement to: 0x(1)209D address.
			//                          (this would be 0xB989+0x6711+0x03 -for instruction)
			//          08|20AE|98DB ->> Displacement to: 0x(FFFF)B98C address.
			//          09|20CE|98BB ->> Displacement to: 0x(FFFF)B98C address
				
			// We can begin from 0 address record for 0x7FFF shiftings
			// We need to know 2 things:
			// 1. displacement for new future address.
			// 2. displacement for new shifting.
			if (data_rec[work_record].shift < 0x7FFF) {
				/////// 1. new future address
				// Let's know displacement for new future address.
				pos_table = 0;
				to_address = data_rec[work_record].address;

				// Check if there is any string until address+shift.
				while (data_rec[pos_table].address < to_address && pos_table < table_counter) {
					if (data_rec[pos_table].type == 2) {
						// We found string. Let's increment the displacement count.
						address_displacement = address_displacement + data_rec[pos_table].str_length_new;
						address_original_length = address_original_length + data_rec[pos_table].str_length;
					}				

					pos_table++;
				}

				/////// 2. new shifting
				// Prepare var with address until go.
				pos_table = 0;
				to_address_outofbounds = (data_rec[work_record].address + data_rec[work_record].shift + 0x03) - 0x10000;

				// Check if there is any string until address+shift.
				while (data_rec[pos_table].address < to_address_outofbounds && pos_table < table_counter) {
					if (data_rec[pos_table].type == 2) {
						// We found string. Let's increment the displacement count.
						displacement = displacement + data_rec[pos_table].str_length_new;
						original_length = original_length + data_rec[pos_table].str_length;
					}				

					pos_table++;
				}

				// Ok, we know the displacement. We can update shifting in table.
				address_displacement = data_rec[work_record].address - address_original_length + address_displacement;
				address_displacement = address_displacement - data_rec[work_record].address;

				data_rec[work_record].shift = (data_rec[work_record].shift - original_length + displacement) - address_displacement;
			} else {
				/////// 1. new future address
				// Let's know displacement for new future address.
				pos_table = 0;
				to_address = data_rec[work_record].address;

				// Check if there is any string until address+shift.
				while (data_rec[pos_table].address < to_address && pos_table < table_counter) {
					if (data_rec[pos_table].type == 2) {
						// We found string. Let's increment the displacement count.
						address_displacement = address_displacement + data_rec[pos_table].str_length_new;
						address_original_length = address_original_length + data_rec[pos_table].str_length;
					}				

					pos_table++;
				}

				/////// 2. new shifting
				// Prepare var with address until go.
				pos_table = 0;
				to_address_outofbounds = data_rec[work_record].address + data_rec[work_record].shift;

				// Check if there is any string until address+shift.
				while (data_rec[pos_table].address < to_address_outofbounds && pos_table < table_counter) {
					if (data_rec[pos_table].type == 2) {
						// We found string. Let's increment the displacement count.
						displacement = displacement + data_rec[pos_table].str_length_new;
						original_length = original_length + data_rec[pos_table].str_length;
					}				

					pos_table++;
				}

				// Ok, we know the displacement. We can update shifting in table.
				address_displacement = data_rec[work_record].address - address_original_length + address_displacement;
				address_displacement = address_displacement - data_rec[work_record].address;

				data_rec[work_record].shift = (data_rec[work_record].shift - original_length + displacement) - address_displacement;
			}
			break;

		case 10:
			// We have here FOREACH LIST instruction. It does not matter which,
			// the process is the same.
			// Let's check as per shift, if there is any string record.
			// We should know if shift if going up or down.
			// AFAIK all shifts are < 0x7FFF
			
			pos_table = work_record+1;
			// Prepare var with address until go.
			to_address = data_rec[work_record].address + data_rec[work_record].shift;

			// Check if there is any string until address+shift.
			while (data_rec[pos_table].address < to_address && pos_table < table_counter) {
				if (data_rec[pos_table].type == 2) {
					// We found string. Let's increment the displacement count.
					displacement = displacement + data_rec[pos_table].str_length_new;
					original_length = original_length + data_rec[pos_table].str_length;
				}				
				pos_table++;
			}

			// Ok, we know the displacement. We can update shifting in table.
			// In this case we MUST add +1 to this lenght for U8 code purposes.
			data_rec[work_record].shift = data_rec[work_record].shift - original_length + displacement;

			break;

		default:
			// Nothing else.
			break;
		}

		// Next record in table
		work_record++;

		// Init some vars
		displacement = 0; address_displacement = 0;
		original_length = 0; address_original_length = 0;

	}

	//////////////////////////////////////////////////////////////////////////////
	//
	//    2.- We will change FUNCTIONS addresses having in mind new strings lengths.
	//		  We will put this address in the second address on the Function record.
	//		  This only will help in part 3. There is no more intention for this.
	//
	//////////////////////////////////////////////////////////////////////////////
	work_record = 0;
	while (work_record < table_counter) {
		// The best choice to do this is search to the function, then,
		// add the displacement found with the different strings previous
		// to this function.

		// Init some vars
		displacement = 0;
		original_length = 0;

		// Let's see what we have in the record.
		if (data_rec[work_record].type == 1) {
			// We have here a function.
			// Ok, let's change the second address for the function searching
			// the strings from the beginning.
			pos_table = 0;

			while (pos_table < work_record) {
				// Let's check if it is a string
				if (data_rec[pos_table].type == 2) {
					// It is. We must count displacement and original size.
					displacement = displacement + data_rec[pos_table].str_length_new;
					original_length = original_length + data_rec[pos_table].str_length;
				}

				pos_table++;
			}

			// Ok, we have it. Now we must update the function address.
			data_rec[work_record].u8func = data_rec[work_record].u8func - original_length + displacement;
		}

		// Next record in table
		work_record++;
	}

	//////////////////////////////////////////////////////////////////////////////
	//
	//     3.- We will change finally CALL/SPAWN instructions addresses
	//         having in mind new addresses changed in point 2.
	//
	//////////////////////////////////////////////////////////////////////////////
	work_record = 0;
	while (work_record < table_counter) {
		// Here we only need to search CALL/SPAWN instructions and change
		// u8func address of the class with the ones updated in point 2.
		// We well do this only for own class functions.

		// The best choice here is to search a function and check all the
		// records of the table in search of a CALL/SPAWN instruction that uses
		// the function class.

		// Function found.
		if (data_rec[work_record].type == 1) {
			// If address and u8func vars are not changed that means that
			// the function address has not been changed. Let's check if changed.
			if (data_rec[work_record].address != data_rec[work_record].u8func) {
				// We put here changed address of function.
				original_length = data_rec[work_record].address;

				// For each record search its type (CALL/SPAWN) and change address
				// if u8class is the same class of the file.
				for (pos_table = 0; pos_table < table_counter; pos_table++)
					if ((data_rec[pos_table].type > 4) && (data_rec[pos_table].type < 8))
						// Let's check if it is own class and the same u8func.
						if (data_rec[pos_table].u8class == fnum && data_rec[pos_table].u8func == original_length)
							// Found. 
							data_rec[pos_table].u8func = data_rec[work_record].u8func;
			}
		}

		// Next record in table
		work_record++;
	}

	//////////////////////////////////////////////////////////////////////////////
	//
	//     3b.- We will change in the special case of SPAWN INLINE instruction
	//          the displacement.
	//
	//////////////////////////////////////////////////////////////////////////////
	work_record = 0;
	while (work_record < table_counter) {
		// Here we only need to search for the SPAWN INLE instruction and change
		// u8sishift shift of the instruction calculating the displacement.
		// We well do this only for own class functions (I have not seen
		// any case with external function).

		// The best choice here is to:
		//	  1. Search the type 7 SPAWN INLINE instruction.
		//    2. 
		// records of the table in search of a CALL/SPAWN instruction that uses
		// the function class.

		// Function found.
		if (data_rec[work_record].type == 7) {
			// Init vars.
			displacement = 0; original_length = 0;

			// Let's position first at the record in the table where the
			// relative address is the address of function CLASS:ADDFUNC<---.
			pos_table = 0;
			while (data_rec[pos_table].address < data_rec[work_record].u8func) pos_table++; 

			// Ok, now we must count displacement having in mind the shifting from this adress
			// until address+shifting.
			while (data_rec[pos_table].address < (data_rec[work_record].u8func + data_rec[work_record].u8sishift)) {
				// Let's check if it is a string
				if (data_rec[pos_table].type == 2) {
					// It is. We must count displacement and original size.
					displacement = displacement + data_rec[pos_table].str_length_new;
					original_length = original_length + data_rec[pos_table].str_length;
				}

				pos_table++;
			}

			// Apply difference
			data_rec[work_record].u8sishift = data_rec[work_record].u8sishift - original_length + displacement;
		}

		work_record++;
	}


	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Process for write into txt file the table of records...
/////////////////////////////////////////////////////////////////////////////////////////
int write_txt() {
	int work_record = 0;

	// We open here the .txt file for output.
	if ((fileout_fp = fopen(ftxt_out, "wt")) == NULL) {
		printf ("Error opening .txt file %s for writing.\n", ftxt_out);
		return (-1);
		fcloseall();
	}

	// Process...
	while (work_record < table_counter) {
		// TYPE
		// Ok, we should check if this TYPE of line should be written or not.
		// Why? Well, we need to know if there would be any change in the size
		// of the shifting between the code.
		// Normally, JMP, JNE and FUNCTION TYPEs must be checked.

		// Let's get TYPE for further purposes.
		fprintf (fileout_fp, "%02d", data_rec[work_record].type);

		// Separator
		fputs ("|", fileout_fp);

		// Address
		fprintf (fileout_fp, "%04X", data_rec[work_record].address);

		// Separator
		fputs ("|", fileout_fp);

		// Ok. Depending on type, we should do one or another thing.
		switch (data_rec[work_record].type) {
			case 1:
				// Here goes FUNCTION
				// We put a second time the function. This is for further recompilation.
				fprintf (fileout_fp, "%04X", data_rec[work_record].u8func);
				break;

			case 2:
				// STRING
				// Let's write LENGTH
				fprintf (fileout_fp, "%04X", data_rec[work_record].str_length);
				// Separator
				fputs ("|", fileout_fp);

				// Let's write STRING
				fputs(data_rec[work_record].string, fileout_fp);
				break;

			case 3:
			case 4:
			case 8:
			case 9:
			case 10:
				// JMP & JNE / FOREACH LIST / JMP & JNE OFFLIMITS instructions for 0x040A class.
				// Let's write SHIFT
				fprintf (fileout_fp, "%04X", data_rec[work_record].shift);
				break;

			case 5:
			case 6:
				// CALL/SPAWN
				// Let's write CLASS:FUNC
				fprintf (fileout_fp, "%04X", data_rec[work_record].u8class);
				fputs(":", fileout_fp);
				fprintf (fileout_fp, "%04X", data_rec[work_record].u8func);
				break;

			case 7:
				// SPAWN INLINE
				// Let's write CLASS:FUNC|SISHIFT
				fprintf (fileout_fp, "%04X", data_rec[work_record].u8class);
				fputs(":", fileout_fp);
				fprintf (fileout_fp, "%04X", data_rec[work_record].u8func);
				fputs("|", fileout_fp);
				fprintf (fileout_fp, "%04X", data_rec[work_record].u8sishift);
				break;

			default:
				break;
		}

		// New Line
		fputs ("\n", fileout_fp);

		// Increment record table to work with
		work_record++;
	} 

	// Let's close here .txt file.
	fclose(fileout_fp);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Function that will update addresses for CALL/SPAWN instructions which are
// external to the clase which we are working with.
// We will need each nnnn.txt.new file of this external class.
// If it does not exists, we will jump the process and go for the next.
/////////////////////////////////////////////////////////////////////////////////////////
int _recalc_external() {
	// Well, for each record we will see if it is CALL/SPAWN instruction,
	// and if it is external. If so, we will have to read this function
	// address and update it in the working class.
	int work_record = 0;
	bool found = false;
	char ext_func[MAX_HEXCLASS], ext_address[MAX_HEXCLASS];
	long n_ext_func, n_ext_address;

	while (work_record < table_counter) {
		// There's no change for Classes 0 and 1.
		if ((data_rec[work_record].type > 4 && data_rec[work_record].type < 8) &&
		    (data_rec[work_record].u8class != fnum)) {
			// One CALL/SPAWN function.
			// Now let's get class number in hexa.

			sprintf (filerec, "%04X.txt.new", data_rec[work_record].u8class);

			// Let's check if this file Class exists.
			if ((_access(filerec, 0)) == -1) {
				// File not exists
				// Let's print something...
				printf ("Recalculated Class file %s....: Not Found.\n", filerec);
			} else {
				// Open file for reading.
				// We open here the .txt file for output.
				if ((filerec_fp = fopen(filerec, "rt")) == NULL) {
					printf ("Error opening external class file %s.\n", filerec);
				}

				// Let's search the function for the given class.
				// We read first line of external class file.
				fgets (buf, MAX_SZ_STR, filerec_fp);

				// Let's search...
				while (!feof(filerec_fp) && !found){

					// We need to know the type of record. We can use address or type.
					// Let's do it with original address for function.
					ext_address[0] = buf[3]; ext_address[1] = buf[4]; ext_address[2] = buf[5];
					ext_address[3] = buf[6]; ext_address[4] = NULL; n_ext_address = strtol (ext_address, NULL, 16);

					if (n_ext_address == data_rec[work_record].u8func) {
						// We found the function we are searching for.
						// We must update this function address in the working class.
						ext_func[0] = buf[8]; ext_func[1] = buf[9]; ext_func[2] = buf[10];
						ext_func[3] = buf[11]; ext_func[4] = NULL; n_ext_func = strtol (ext_func, NULL, 16);

						data_rec[work_record].u8func = n_ext_func;
						found = true;
					}

					// Read next line.
					fgets (buf, MAX_SZ_STR, filerec_fp);
				}

				fclose (filerec_fp);
			}
		}

		// Next table record.
		work_record++; found = false;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Let's print some info about the usage of the tool.
/////////////////////////////////////////////////////////////////////////////////////////
int _Usage() {
	printf ("U8CALC - Ultima 8 ReCalculate Tool v1.0\n");
	printf ("=======================================\n\n");
	printf ("Usage:\n");
	printf ("u8calc -a|-c <nnnn.txt file> [-l]\n\n");
	printf (" -a               : Recalculates ALL the addresses for ALL the .txt files\n");
	printf ("                    decompiled with U8DEC tool. Original index.txt file needed.\n");
	printf (" -c               : Recalculates the addresses for a concrete .txt file\n");
	printf ("                    decompiled with U8DEC tool. If used with -l parameter\n");
	printf ("                    index.txt file may be needed.\n");
	printf (" - <nnnn.txt file>: One of the files decompiled with U8DEC tool.\n");
	printf (" -l               : Parameter which indicates if recalculate external addresses to\n");
	printf ("                    CALL/SPAWN instructions.\n");
	printf ("\n");
	printf ("Output:\n");
	printf ("  <nnnn.txt.new file>   This file(s) is created with new addresses changed.\n");
	printf ("\n");
	printf ("Samples:   u8calc -c 0044.txt -l   (door class)\n");
	printf ("           u8calc -c 0044.txt\n");
	printf ("           u8calc -a\n");

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
// Function body for partial address calculation.
/////////////////////////////////////////////////////////////////////////////////////////
int _prep_filenames(char *file){
	char *p, s_num[MAX_HEXCLASS];
	int result;

	// Recalculate addresses for one unique file.
	// Only recalculate own CALL/SPAWN instructions addresses.
	// Ok, let's put nnnn.txt filename in input variable
	// and nnnn.txt.new filename in output variable.
	strcpy (ftxt_in, file);
	strcpy (ftxt_out, file);
	p = strstr (ftxt_out, "txt");
	result = (int)(p - ftxt_out + 1);			
	ftxt_out[result+2] = '.'; ftxt_out[result+3] = 'n'; ftxt_out[result+4] = 'e';
	ftxt_out[result+5] = 'w'; ftxt_out[result+6] = NULL;

	// Let's put number class into var.
	s_num[0] = ftxt_out[result-6]; s_num[1] = ftxt_out[result-5]; s_num [2] = ftxt_out[result-4];
	s_num[3] = ftxt_out[result-3]; s_num[4] = NULL;

	// Here we will get the function number to check CALLs & SPAWNs own class.
	fnum = strtol(s_num, NULL, 16);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


// Main body of tool...
int main(int argc, char *argv[])
{
	char s_fidx[MAX_LEN];
	char *p;
	int result;

	// Let's check arguments with argc.
	switch (argc) {
	case 2:
		// One argument, probably -a.
		if (strcmp (argv[1], "-a") == 0) {
			// Recalculate addresses for all the files.
			// We will need read each file, so let's get index.txt for do it.
			if ((fileidx_fp = fopen ("index.txt", "rt")) == NULL ) {
				printf ("Error opening index.txt for reading.\n");
				return (-1);
			}

			// Let's do the job for every file.
			// I've decided make all the calculations in two phases.
			//   1.- Make only partial address calculations for each file.
			//   2.- Make external address calculations for each file.
			// So, yes, we have to read all the files twice.

			/////////////////////////////////////////////////////////////
			// 1.- Make only partial address calculations for each file.
			/////////////////////////////////////////////////////////////

			// Read first line.
			fgets (s_fidx, MAX_LEN, fileidx_fp);

			while (!feof(fileidx_fp)) {
				// Prepare readed line from index.txt
				p = strstr (s_fidx, ";");
				result = (int)(p - s_fidx + 1);			
				s_fidx[result-1] = NULL;
				s_fidx[result-4] = 't'; s_fidx[result-3] = 'x'; s_fidx[result-2] = 't';
				_prep_filenames(s_fidx);

				if ((_access(ftxt_in, 0)) == -1) {
					// File class not exists
					// Let's print something...
					printf ("PARTIAL: Class %s... No exists.\n", ftxt_in);
				} else {
					// If Class is 0 or 1, that will be not treated.
					if (fnum > 1) {
						// Let's do the recalculation process WITHOUT external
						// CALL/SPAWN functions.
						_empty_data_rec();
						_recalc_partial();

						// Process to recalculate partial new addresses and shiftings ended.
						// Now we have to write nnnn.txt.new file with this recalculated data.
						write_txt();

						printf ("Partial proccess done for %04X class file.\n", fnum);
					} else printf ("Partial process for %04X class file not needed.\n", fnum);
				}

				// Read next line.
				fgets (s_fidx, MAX_LEN, fileidx_fp);
			}

			//////////////////////////////////////////////////////////////
			// Ok, done partial files recalculation.
			// Now the issue here is to do all recalculations for all the files.
			// That's why we need two passes in the files.
			// We must have in mind, that now we should get info from
			// new nnnn.txt.new created file. And rewrite it.
			// So file_in and file_out will be the same.
			//
			// 2.- Make external address calculations for each file.
			//
			////////////////////////////////////////////////////////////////

			// First, lets position index.txt pointer to begginning
			fseek (fileidx_fp, 0, SEEK_SET);

			// Read first line.
			fgets (s_fidx, MAX_LEN, fileidx_fp);

			while (!feof(fileidx_fp)) {
				// Prepare readed line from index.txt
				p = strstr (s_fidx, ";");
				result = (int)(p - s_fidx + 1);			
				s_fidx[result-1] = NULL;
				s_fidx[result-4] = 't'; s_fidx[result-3] = 'x'; s_fidx[result-2] = 't';
				_prep_filenames(s_fidx);

				// Here we need to change a bit the filenames.
				p = strstr (ftxt_in, "txt");
				result = (int)(p - ftxt_in + 1);			
				ftxt_in[result+2] = '.'; ftxt_in[result+3] = 'n'; ftxt_in[result+4] = 'e';
				ftxt_in[result+5] = 'w'; ftxt_in[result+6] = NULL;

				if ((_access(ftxt_in, 0)) == -1) {
					// File class not exists
					// Let's print something...
					printf ("EXTERNAL: Class %s... No exists.\n", ftxt_in);
				} else {
					if (fnum > 1) {
						// Here we need to load the records in records table for
						// each file.
						_empty_data_rec();
						_prep_strtable();

						// Let's do the recalculation process WITHOUT external
						// CALL/SPAWN functions.
						_recalc_external();

						// In this case we need to do somehting apart for Spanish (or foreign languages).
						// We must replace/convert some chars.
						_cnv_chars();

						// Process to recalculate partial new addresses and shiftings ended.
						// Now we have to write nnnn.txt.new file with this recalculated data.
						write_txt();

						printf ("External proccess done for %04X class file.\n", fnum);
					}
					else printf ("Partial process for %04X class file not needed.\n", fnum);
				}

				// Read next line.
				fgets (s_fidx, MAX_LEN, fileidx_fp);
			}

			// We close finally index file.
			fclose (fileidx_fp);

		} else _Usage(); // No valid parameter.
		break;

	case 3:
		// Two arguments, probably -c <nnnn.txt>
		if (strcmp (argv[1], "-c") == 0) {
			_prep_filenames(argv[2]);

			// Let's do the recalculation process WITHOUT external
			// CALL/SPAWN functions.
			_recalc_partial();

			// In this case we need to do somehting apart for Spanish (or foreign languages).
			// We must replace/convert some chars.
			_cnv_chars();

			// Process to recalculate partial new addresses and shiftings ended.
			// Now we have to write nnnn.txt.new file with this recalculated data.
			write_txt();

			printf ("Partial proccess done for %04X class file.\n", fnum);
		} else _Usage(); // No valid parameter.
		break;

	case 4:
		// Three arguments. Probably "-c <nnnn.txt> -l"
		if ((strcmp (argv[1], "-c") == 0) && (strcmp (argv[3], "-l") == 0)) {
			// Let's prepare filenames.
			_prep_filenames(argv[2]);

			// Ok, this function is more or less the same than previous one,
			// BUT we need here a fourth process. Recheck internal CALL/SPAWN
			// instructions of the class which hava external classes:functions.

			// Let's do the recalculation process WITHOUT external
			// CALL/SPAWN functions (processes 1-3).
			_recalc_partial();

			//////////////////////////////////////////////////////////////////////////////
			//
			//     4.- Ok, we know will check every CALL/SPAWN functions that are
			//         of external class:function.
			//
			//////////////////////////////////////////////////////////////////////////////
			_recalc_external();

			// In this case we need to do somehting apart for Spanish (or foreign languages).
			// We must replace/convert some chars.
			_cnv_chars();

			// Process to recalculate partial new addresses and shiftings ended.
			// Now we have to write nnnn.txt.new file with this recalculated data.
			write_txt();

			printf ("Full proccess done for %04X class file.\n", fnum);

		} else _Usage(); // No valid parameter.
		break;

	default:
		_Usage();
		break;
	}

	return 0;
}

