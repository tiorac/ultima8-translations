// U8Q.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string.h"

#define BUFFER			 12248
#define MAX_LEN			   256

FILE *qfile;
char buffer[BUFFER];
char qname[MAX_LEN];

unsigned long filesize;
unsigned char c, d;
unsigned int x, i;

char *p;
int result;

/////////////////////////////////////////////////////////////////////////////////////////
// QUOTE PROCESS.
/////////////////////////////////////////////////////////////////////////////////////////
int _quote() {
	// Ok, we open first text file.
	if ((qfile = fopen(qname, "rb")) == NULL) {
		printf ("Error opening flex file (%s) for reading.\n", qname);
		return -1;
	}

	// Read the file into buffer.
	fseek(qfile, 0, SEEK_END);
	filesize = ftell (qfile);
	fseek(qfile, 0, SEEK_SET);

	buffer[0] = NULL;
	
	for (i = 0; i < filesize; ++i) {
		fread (&c, sizeof(char), 1, qfile);

	
		switch(i) {
	    case 0:
		case 1:
			x = 0; break;
		case 2:
			x = 0xE1; break;
		default:
			x = 0x20 * (i+1) + (i >> 1);
			x += (i % 0x40) * ((i & 0xC0) >> 6) * 0x40;
			break;
		}
		if (c == 0x0A) c = 0;
		d = (c ^ x) & 0xFF;
		buffer[i] = d;
	}
	fclose (qfile);

	p = strstr (qname, ".");
	result = (int)(p - qname);
	qname[result+1] = 'D'; qname[result+2] = 'A'; qname[result+3] = 'T'; qname[result+4] = '.';
	qname[result+5] = 'n'; qname[result+6] = 'e'; qname[result+7] = 'w';
	qname[result+8] = NULL; 

	// If ok, let's open file for writing quotes in .DAT format.
	if ((qfile = fopen (qname, "wb")) == NULL ) {
		printf ("Error opening text file (%s) for writing.\n", qname);
		return -1;
	}

	fwrite (buffer, sizeof(unsigned char), i, qfile);
	fclose (qfile);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// DEQUOTE PROCESS.
/////////////////////////////////////////////////////////////////////////////////////////
int _dequote() {
	// Ok, we open first quoted file.
	if ((qfile = fopen(qname, "rb")) == NULL) {
		printf ("Error opening flex file (%s) for reading.\n", qname);
		return -1;
	}

	// Read the file into buffer.
	fseek(qfile, 0, SEEK_END);
	filesize = ftell (qfile);
	fseek(qfile, 0, SEEK_SET);

	buffer[0] = NULL;
	
	for (i = 0; i < filesize; ++i) {
		fread (&c, sizeof(char), 1, qfile);
		
		switch(i) {
	    case 0:
		case 1:
			x = 0; break;
		case 2:
			x = 0xE1; break;
		default:
			x = 0x20 * (i+1) + (i >> 1);
			x += (i % 0x40) * ((i & 0xC0) >> 6) * 0x40;
			break;
		}
		d = (c ^ x) & 0xFF;
		buffer[i] = d;
	}
	fclose (qfile);

	p = strstr (qname, ".");
	result = (int)(p - qname);
	qname[result+1] = 't'; qname[result+2] = 'x'; qname[result+3] = 't'; qname[result+4] = NULL; 

	// If ok, let's open file for writing quotes in text format.
	if ((qfile = fopen (qname, "wb")) == NULL ) {
		printf ("Error opening text file (%s) for writing.\n", qname);
		return -1;
	}

	fwrite (buffer, sizeof(unsigned char), i, qfile);
	fclose (qfile);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Let's print some info about the usage of the tool.
/////////////////////////////////////////////////////////////////////////////////////////
int _Usage() {
	printf ("U8Q - Ultima 8 Quote Tool v1.0\n");
	printf ("==============================\n\n");
	printf ("Usage:\n");
	printf ("u8q -x <.dat file>|-c <.txt file>\n\n");
	printf ("  -x     Extracts text from <.dat file> into <.txt file>.\n");
	printf ("\n");
	printf ("  -c     Creates a new <.dat file> from <.txt file>.\n");
	printf (" <.dat file>   ECREDITS.DAT or QUOTES.DAT.\n");
	printf ("\n");
	printf ("Sample:   u8q -x ECREDITS.DAT\n");
	printf ("          This creates ECREDITS.txt\n");
	printf ("\n");
	printf ("Sample:   u8q -c ECREDITS.txt\n");
	printf ("          This creates ECREDITS.DAT.new\n");
	printf ("\n");

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{

	if (argc != 3) {
		// Print Usage
		_Usage();
		return (0);
	}

	// Prepare the filename
	strcpy (qname, argv[2]);

	// We do here dequote or quote process. Let's do...
		if (strcmp(argv[1], "-x") == 0) {
		if (!_dequote()) printf ("Dequote of text from %s... Completed.\n", qname);
	}
	else if (strcmp(argv[1], "-c") == 0) {
			 if (!_quote()) printf ("Quote of %s .DAT file... Completed.\n", qname);
	    } else _Usage();

	return 0;
}

