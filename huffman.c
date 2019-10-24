#include <stdio.h>
#include "sort.h"

void char_count(unsigned int *,int freq[],FILE *);

void sort_node(int *,int parent[],int freq[],int node[],int *);

void encode_t(int *,int parent[],int left[],FILE *);

void encode_b(FILE *,FILE *);

int main(int argc,char *argv[])
{
	FILE *infile, *outfile, *outfile_b;
	unsigned int count = 0;
	int i, j, chr;
	int left[512], right[512], parent[512], freq[512], nsize = 0, size = 256, node[256];
	char bits[11];
	
	/*オプションとしてファイル名だけが指定されているか確認する。*/
	if(argc == 2){
			infile = fopen(argv[1],"r");
			if(infile == NULL){
				printf("ファイルを開けませんでした。\n");
				return -1;
			}
	}else{
			printf("プログラム起動時に一つだけのコマンドラインオプションとしてファイル名を指定してください。\n");
			
			return 0;
	}

	/*木構造用の配列を初期化する。*/
	for(i = 0;i < 512;i++){
		left[i] = -1;
		right[i] = -1;
		parent[i] = -1;
		freq[i] = 0;
	}

	/*ファイル中の文字ごとの数を計算して、freq[]の該当する要素に格納する。*/
	char_count(&count,freq,infile);
	fclose(infile);

	/*freq[]に格納した値を二つずつ取り出して、親になる節の値をfreq[]の後方の要素に格納して、freq[]中の値の親になる値の、左の子になる値の、右の子になる値の要素番号を当該する配列parent[], left[], right[]に格納する。ハフマン符号化用の木構造になるように値の昇順、または配列の先頭から近い順に値を取り出す。*/
	sort_node(&size, parent, freq, node, &nsize);/*親のない値の要素番号をnode[]に昇順に格納する。*/
	
	while(nsize >= 2){
		i = node[0];
		j = node[1];
		left[size] = i;
		right[size] = j;
		freq[size] = freq[i] + freq[j];
		parent[i] = size;
		parent[j] = size;
		++size;
		sort_node(&size, parent, freq, node, &nsize);
	}
	
	/*ハフマン木を使ってワークファイル（txt）で符号化します。*/
	infile = fopen(argv[1],"r");
	outfile = fopen("huffman_out.txt","w");

	while((chr = fgetc(infile)) != EOF){
		encode_t(&chr, parent, left, outfile);
	}

	fclose(infile);
	fclose(outfile);
	
	/*バイナリーファイルを作成します。*/
	outfile = fopen("huffman_out.txt","r");
	outfile_b = fopen("huffman_out.dat","wb");

	encode_b(outfile, outfile_b);

	fclose(outfile);
	fclose(outfile_b);

	/*成功メッセージの準備をします。*/
	infile = fopen(argv[1],"r");
	outfile = fopen("huffman_out.txt","r");
	chr = fgetc(infile);
	for(i = 0;i < 10;i++){
		bits[i] = fgetc(outfile);
	}
	fclose(infile);
	fclose(outfile);

	/*成功メッセージ*/
	printf("%s の符号化が完了しました。%d文字中の最初の文字は %c で、符号化ファイルの最初の１０ビットはは%sです。\n",argv[1],count,chr,bits);
	

	/*終了*/
	return 0;
}

void char_count(unsigned int *count,int freq[],FILE *infile)
{
	int chr, chr_last;

	while((chr = fgetc(infile)) != EOF){
		(*count)++;
		freq[chr]++;
		chr_last = chr;
	}
	/*最後の文字が改行文字の場合、その文字を符号化しないようにします。*/
	if(chr_last == 10){
		(*count)--;
		freq[chr_last]--;
	}
	
	return;
}

void sort_node(int *size,int parent[],int freq[],int node[],int *nsize)
{
	int i;
	
	*nsize = 0;

	for(i = 0;i < *size;i++){
		if(parent[i] < 0 && freq[i] != 0){
			node[*nsize] = i;
			(*nsize)++;
		}
	}
	
	bubble_sort_node(freq, nsize, node);

	return;
}

void encode_t(int *k,int parent[],int left[],FILE *outfile)
{
	if(parent[*k] > 0){
		encode_t(&parent[*k],parent,left,outfile);

		if(left[parent[*k]] == *k){
			fprintf(outfile,"0");
		}else{
			fprintf(outfile,"1");
		}
	}

	return;
}

void encode_b(FILE *outfile,FILE *outfile_b)
{
	int i = 0, binary = 0, chr;
	
	while((chr = fgetc(outfile)) != EOF){
		binary = binary << 1;

		if(chr == 0x31){
			binary = binary + 1;
		}	
		
		if(++i % (sizeof(int) * 8) == 0){
			fwrite(&binary,sizeof(binary),1,outfile_b);
			binary = 0;
		}
	}

	if(binary != 0){
		binary = binary << ((sizeof(int) * 8) - (i % (sizeof(int) * 8)));
		fwrite(&binary,sizeof(binary),1,outfile_b);
		binary = 0;
	}

	return;
}

