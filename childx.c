#include <sys/types.h>

int sum(int x, int y){
    return x + y;
}


int mul(int x, int y){
    return x * y;
}

int64_t a = 2, b = 3;

main(){
    printf("number are %d and %d \n", a, b);
    printf("sum is : %d\n",sum(a, b));
    printf("mul is : %d\n",mul(a, b));
}
