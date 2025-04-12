#include "main.h"




int main() {

    initialise_monitor_handles();  // Chỉ cần với newlib
    printf("Hello via semi-hosting!\n");  // Hiển thị trên console debugger
    
    FILE *f = fopen("host_file.txt", "w");
    fprintf(f, "Writing to host file");
    fclose(f);



    struct Dummy* dummy2 = (struct Dummy*)malloc(sizeof(struct Dummy)); // Heap variable


    dummy2->member1 = 3;
    dummy2->member2 = 4;

    free(dummy2);



    function1();
    //function2();

    while (1) {
        
    }

    return 0;
}