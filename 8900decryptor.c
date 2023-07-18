#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/aes.h>

struct {
	unsigned char magic[ 4 ];
	unsigned char version[ 3 ];
	unsigned char encrypted;
	unsigned char unknownNull[4];
	unsigned int sizeOfData;
	unsigned int footerSignatureOffset;
	unsigned int footerCertOffset;
	unsigned int footerCertLen;
	unsigned char key1[ 32 ];
	unsigned char unknownVersion[ 4 ];
	unsigned char key2[ 16 ];
	unsigned char unknow3[ 1968 ];
} header8900;

char inbuf[ 65536 ];
char outbuf[ 65536 ];

void convert_hex(char *str, uint8_t *bytes, int maxlen) {
  int slen = strlen(str);
  int bytelen = maxlen;
  int rpos, wpos = 0;

  for(rpos = 0; rpos < bytelen; rpos++) {
    sscanf(&str[rpos*2], "%02hhx", &bytes[wpos++]);
  }
}

void print_hex( char* text, uint8_t* data, uint32_t len )
{
	uint32_t ctr;
	char* sep;
	if( len > 64 )
		len = 64;
	printf( "%s", text );
	for( ctr = 0; ctr < len; ctr++ )
	{
		printf( "%02x ", data[ ctr ] );
	}
	printf( "\n" );
}

int main( int argc, char** argv )
{
	FILE *infile, *outfile;
	int encrypted;
	off_t data_begin, data_current, data_end, data_len;
	AES_KEY ctx, aes_decrypt_key;
	uint8_t aes_key[16];

	unsigned char ramdiskKey[32]="188458A6D15034DFE386F23B61D43774";
	unsigned char ramdiskiv[1];

	unsigned char keybuf[ 16 ];
	unsigned char iv[ AES_BLOCK_SIZE ];
	
	if( argc != 3 )
	{
		fprintf( stderr, "Usage: %s infile outfile\n", argv[ 0 ] );
		return 1;
	}

	infile = fopen( argv[ 1 ], "r" );
	if( !infile )
	{
		perror( "infile fopen" );
		return 1;
	}

	outfile = fopen( argv[ 2 ], "w+" );
	if( !outfile )
	{
		perror( "outfile fopen" );
		return 1;
	}

	if( sizeof( header8900 ) != fread( &header8900, 1, sizeof( header8900 ), infile ) )
	{
		fprintf( stderr, "Can't read header\n" );
		return 1;
	}

	if( ( header8900.magic[ 0 ] != 0x38 ) && // 8
	    ( header8900.magic[ 1 ] != 0x39 ) && // 9
	    ( header8900.magic[ 2 ] != 0x30 ) && // 0
	    ( header8900.magic[ 3 ] != 0x30 ) )  // 0
	{
		fprintf( stderr, "Bad magic\n" );
		return 1;
	}

	if( header8900.encrypted == 0x03 )
		encrypted = 1;
	else if( header8900.encrypted = 0x04 )
		encrypted = 0;

	data_begin = sizeof( header8900 );
	data_len = header8900.sizeOfData;
	printf( "[*] filename	\t\t: %s\n", argv[ 1 ] );
	printf( "[*] magic	\t\t: %s\n", header8900.magic );
	printf( "[*] version	\t\t: %s\n", header8900.version );
	printf( "[*] encrypted	\t\t: %d\n", encrypted );
	printf( "[*] start of data    \t\t: 0x%02x\n", data_begin );
	printf( "[*] size of data\t\t: 0x%02x\n", header8900.sizeOfData );
	printf( "[*] footer signature offset\t: 0x%02x\n", header8900.footerSignatureOffset );
	printf( "[*] footer certificate offset\t: 0x%02x\n", header8900.footerCertOffset );
	printf( "[*] footer certificate length\t: 0x%02x\n", header8900.footerCertLen );
	print_hex( "[*] header key 1\t\t: ", header8900.key1, sizeof( header8900.key1 ) );
	print_hex( "[*] header key 2\t\t: ", header8900.key2, sizeof( header8900.key2 ) );

	//memset( keybuf, 0, 32 );
        //memcpy( keybuf, header8900.key1, 32 );
	convert_hex(ramdiskKey, aes_key, 16);
	AES_set_decrypt_key( aes_key, 128, &aes_decrypt_key );
	memset( iv, 0, AES_BLOCK_SIZE );
	//memcpy( iv, header8900.key2, 16 );

	fseek( infile, data_begin, SEEK_SET );
	//printf("dataend: %d\n", data_len);

	data_current=0;
	while( fread( &inbuf, 1, AES_BLOCK_SIZE, infile ) > 0 & data_current<data_len)
	{

	  AES_cbc_encrypt( inbuf, outbuf, AES_BLOCK_SIZE, &aes_decrypt_key, iv, AES_DECRYPT );
	  fwrite( outbuf, 1, AES_BLOCK_SIZE, outfile );
	  data_current = data_current + AES_BLOCK_SIZE;
	}

	if( infile )
		fclose( infile );
	if( outfile )
		fclose( outfile );
	return 0;
}
