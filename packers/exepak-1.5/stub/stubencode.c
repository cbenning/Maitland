/* Encodes the stub in a file called __stub.h that can be included in
 * the main EXEPAK binary. */

#include <stdio.h>

int main()
{
	FILE *bfile,*ifile;
	int stubsize;
	unsigned char readbuf[256];
	int n,i,c = 0;
	
	if (!(bfile = fopen("exepak_stub","r"))) {
		printf("Can't open exepak_stub\n");
		exit(0);
	}
	if (!(ifile = fopen("__stub.h","w"))) {
		printf("Can't create __stub.h\n");
		exit(0);
	}
	
	printf("creating __stub.h...\n");
	
	fseek(bfile,0,SEEK_END);
	stubsize = ftell(bfile);
	rewind(bfile);
	//printf("exepak_stub is %d bytes long...\n",stubsize);
	
	fprintf(ifile,"static unsigned char exepak_stub[%d] = {\n\t",stubsize+1);
	while((n = fread(readbuf,1,sizeof(readbuf),bfile))) {
		for(i=0;i<n;i++) {
			fprintf(ifile,"0x%02x,",readbuf[i]);
			if (++c == 13) {
				fprintf(ifile,"\n\t");
				c = 0;
			}
		}
	}
	
	fprintf(ifile,"0 };\n");
	fclose(ifile);
	fclose(bfile);
	printf("done!\n");
	exit(0);
}
