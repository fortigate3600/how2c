#include<stdio.h>
void print(int arr[], int n){
    for(int i=0;i<n;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
}

void bubble(int arr[], int n){
    int temp;
    for (int i =0; i<n;i++){
        for(int j=0;j<i;j++){
            if(arr[i]<arr[j]){
                temp=arr[j];
                arr[j]=arr[i];
                arr[i]=temp;
            }
        }
    }
}

void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int leftArr[n1], rightArr[n2];
    //load elements into temporary arrays
    for (i = 0; i < n1; i++)
        leftArr[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        rightArr[j] = arr[mid + 1 + j];
    //order the temporary arrays and merge them into the original array
    for(i=0, j=0, k=left; i < n1 && j < n2; k++) {
        if (leftArr[i] <= rightArr[j])
            arr[k] = leftArr[i++];
        else
            arr[k] = rightArr[j++];
    }
    // remaining elements of arrays
    while (i < n1)
        arr[k++] = leftArr[i++];
    while (j < n2) 
        arr[k++] = rightArr[j++];
}
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void swap(int *x, int *y){
    int temp=*x;
    *x=*y;
    *y=temp;
}

void swapWithoutTemp(int *x, int *y){
    *x+=*y;
    *y=*x-*y;
    *x-=*y;
}

int main (){
    int arr[10]={2,1,5,3,4,7,9,1,5,7};
    print(arr,10);
    mergeSort(arr,0,9);
    print(arr,10);

    int a=10;
    int b=20;

    //printf("a: %d, b: %d\n",a,b);
    //swapWithoutTemp(&a,&b);
    //printf("a: %d, b: %d\n",a,b);

    printf("pointer: %p\n",&a);
    printf("pointer: %p\n",(&a)+1);
}
