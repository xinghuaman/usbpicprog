#ifdef TEST
#include <stdio.h>
#include "prog.h"
#include "prog_lolvl.h"
//#include "test.h"
#include <string.h>

extern PICFAMILY picfamily;
extern PICTYPE pictype;

unsigned char set_pictype( PICTYPE pictype );
void test( PICTYPE pictype );
int main( int argc, char *argv[] )
{
	// very dumb interface if there are arguments then pure numbers are interpreted as pictype enums otherwise as a string pictype
	// no arguments do all pictypes

//	printf( "devices[UPP_INVALID_PICTYPE].flags.family = %d (%s)\n", devices[UPP_INVALID_PICTYPE].flags.family,
//			picfamilyName[devices[UPP_INVALID_PICTYPE].flags.family] );
	if( argc <= 1 )
		for( pictype = 0; pictype < UPP_INVALID_PICTYPE; ++pictype )
			test( pictype );
	else
	{
		while( ++argv, --argc > 0 )
		{
			if( '0' <= argv[0][0] && argv[0][0] <= '9' )
				pictype = (PICTYPE) atoi( argv[0] );
			else
			{
				for( pictype = 0; pictype != UPP_INVALID_PICTYPE; ++pictype )
					if( strcasecmp( pictypeName[pictype], argv[0] ) == 0 )
						break;
				if( pictype == UPP_INVALID_PICTYPE ) {
					fprintf( stderr, "Unknown pictype %s\n", argv[0] );
					continue;
				}
			}
			test( pictype );
		}
	}
}
#define FIRST 1
#define LAST 2

void test( PICTYPE pt )
{
	int i;
	unsigned char result;
	unsigned long address;
	unsigned char data[1024];
	char blocksize;

	pictype = pt;
	if( pictype >= UPP_INVALID_PICTYPE )
	{
		fprintf( stderr, "Unknown pictype %d\n", pictype );
		return;
	}
	for( i = 0; i < UPP_INVALID_PICTYPE; i++ ) {
		if( devices[i].flags.type == pictype ) {
			currDevice = devices[i];
			break;
		}
	}
	if( pictype == UPP_INVALID_PICTYPE )
	{
		fprintf( stderr, "Unknown pictype %d\n", pictype );
		return;
	}
	picfamily = currDevice.flags.family;

	address = 0x40;
	blocksize = 1;

	memset( data, 0, sizeof( data ) );

	printf( " Type %s, Family %s\n", pictypeName[pictype], picfamilyName[picfamily] );
	printf( ">>>>>> set_vdd_vpp\n" );
	set_vdd_vpp( pictype, picfamily, 1 );

	printf( ">>>>>> bulk_erase( %s, %s, 0 )\n", picfamilyName[picfamily], pictypeName[pictype] );
	result = bulk_erase( picfamily, pictype, 0 );
	printf( " returns( %d )\n", result );

	printf( ">>>>>> write_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0L, 32, BLOCKTYPE_FIRST );
	result = write_code( picfamily, pictype, 0x00, data, 32, BLOCKTYPE_FIRST );
	printf( " returns( %d )\n", result );
	printf( ">>>>>> write_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 256-32, 32, 0 );
	result = write_code( picfamily, pictype, 256-32, data, 32, 0 );
	printf( " returns( %d )\n", result );


	printf( ">>>>>> write_data( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0L, 9, BLOCKTYPE_FIRST );
	result = write_data( picfamily, pictype, address, data, 9, BLOCKTYPE_FIRST );
	printf( " returns( %d )\n", result );

	printf( ">>>>>> write_config_bits( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0x2000L, 3, BLOCKTYPE_FIRST );
	result = write_config_bits( picfamily, pictype, 0x2000, data, 3, BLOCKTYPE_FIRST );
	printf( " returns( %d )\n", result );

	printf( ">>>>>> read_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0L, 3, BLOCKTYPE_FIRST );
	result = read_code( picfamily, pictype, 0, data, 3, BLOCKTYPE_FIRST );
	printf( " returns( %d )\n", result );
	printf( ">>>>>> read_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0xF80000L, 3, BLOCKTYPE_FIRST|BLOCKTYPE_CONFIG );
	result = read_code( picfamily, pictype, 0xF80000, data, currDevice.flags.type==PIC10? 1: 3, BLOCKTYPE_FIRST|BLOCKTYPE_CONFIG );
	printf( " returns( %d )\n", result );

	printf( ">>>>>>  read_data( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0x02L, 9, BLOCKTYPE_FIRST );
	result = read_data( picfamily, pictype, 2, data, 9, BLOCKTYPE_FIRST );
	printf( " returns( %d )\n", result );

	printf("done\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n");
}
#else
// needs at least 1 line for MCC18
extern unsigned char timerRunning;
#endif
