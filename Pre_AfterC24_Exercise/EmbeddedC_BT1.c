#include<stdint.h>
#include<stdio.h>

int nhap_xuat_mang(int n)
{   
    
    printf("khoi tao mang arr[%d] \n",n);
    printf("Nhap cac phan tu cua mang :\n");
    int array[100];
    for(int i=0;i<n;i++)
    {
        printf("arr[%d] = ",i);
        scanf("%d",&array[i]);
    }


    printf("arr[%d]={",n);       
    for(int i=0;i<n;i++)
    {
        printf("%d ",array[i]);       
    }
    printf("}\n");  


    printf("dia chi cua tung phan tu :\n");
    for(int i=0;i<n;i++)
    {
        printf("&arr[%d] = %d \n",i,&array[i]);       
    }
    return 0;

}
main()
{   int n;

    while(1)
    {
    printf("Nhap so luong phan tu: ");
    scanf("%d",&n);
    if ((n > 0) && (n <=16 ))
    {
        break;
    }
    }
    nhap_xuat_mang(n);
}
