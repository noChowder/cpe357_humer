#include <stdio.h>

void write_tree(int height, char *file_name){
    int max_height = 15, min_height = 3;
    int max_width = 1;
    int spacing;
    int temp_height, temp_space, temp_width;

    FILE *fp;
    fp = fopen(file_name, "w");

    /* Erroneous input handling */
    while(height < 4){
        fprintf(stderr, "Error: height is too small, re-enter height \n");
        scanf("%d", &height);
    }
    while(height > 15){
        fprintf(stderr, "Error: height is too tall, re-enter height \n");
        scanf("%d", &height);
    }

    /* writing tree */
    if(height > 3 && height < 16){
        temp_height = height - min_height;
        while(temp_height > 1){
            max_width += 2;
            temp_height--;
        }

        for(spacing = (max_width-1) / 2 ; spacing > -1 ; spacing--){
            temp_space = spacing;
            while(temp_space > 0){
                putchar(' ');
                fputc(' ', fp);
                temp_space--;
            }
            temp_width = 2 * spacing;
            while(temp_width < max_width){
                putchar('*');
                fputc('*', fp);
                temp_width++;
            }
            putchar('\n');
            fputc('\n', fp);
        }
        while(min_height > 0){
            temp_space = (max_width-1) / 2;
            while(temp_space > 0){
                putchar(' ');
                fputc(' ', fp);
                temp_space--;
            }
            putchar('*');
            fputc('*', fp);
            putchar('\n');
            fputc('\n', fp);
            min_height--;
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[]){
    int max_height = 15, min_height = 3;
    int height = 0;
    int max_width = 1;
    int spacing;
    int temp_height, temp_space, temp_width;

    /* Retrieve user input */
    if(argc == 1){
        printf("How tall is this tree? \n");
        int check = scanf("%d", &height);

        /* Erroneous input handling */
        if(!check){
            fprintf(stderr, "Error: invalid height, input must be a number \n");
            return -1;
        }
    }

    /* command-line arguments */
    else if(argc > 1){
        height = atoi(argv[1]);
        if(argc > 2){
            if(!(strcmp(argv[2], "file.txt"))){
                write_tree(height, argv[2]);
                return 0;
            }
        }
    }
    
    /* Erroneous input handling */
    while(height < 4){
        fprintf(stderr, "Error: height is too small, re-enter height \n");
        scanf("%d", &height);
    }
    while(height > 15){
        fprintf(stderr, "Error: height is too tall, re-enter height \n");
        scanf("%d", &height);
    }

    /* printing tree */
    if(height > 3 && height < 16){
        temp_height = height - min_height;
        while(temp_height > 1){
            max_width += 2;
            temp_height--;
        }

        for(spacing = (max_width-1) / 2 ; spacing > -1 ; spacing--){
            temp_space = spacing;
            while(temp_space > 0){
                putchar(' ');
                temp_space--;
            }
            temp_width = 2 * spacing;
            while(temp_width < max_width){
                putchar('*');
                temp_width++;
            }
            putchar('\n');
        }
        while(min_height > 0){
            temp_space = (max_width-1) / 2;
            while(temp_space > 0){
                putchar(' ');
                temp_space--;
            }
            putchar('*');
            putchar('\n');
            min_height--;
        }
    }

    return 0;
}