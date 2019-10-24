/*配列nodeに格納されている、他の配列の要素番号を、その配列の値の昇順に整列する。*/
void bubble_sort_node(int value[],int *nsize,int node[])
{
	int i, j, temp;

	for(i = 0;i < *nsize - 1;i++){
		for(j = 0;j < *nsize - 1;j++){
			if(value[node[j + 1]] < value[node[j]]){
				temp = node[j];
				node[j] = node[j + 1];
				node[j + 1] = temp;
			}
		}
	}
	return;
}
