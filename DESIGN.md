for this assignment, I use the demo better_hash a reference. To start things off, I create the Customer struct which has the variables and the Customertable struct which holds the customers in a double pointer and the num buckets.

typedef struct Customer {
    char *email; //the key
    char *name;   
    char *fav_food;  // values
    int shoe_size; // values
    struct Customer *next;
} Customer;

typedef struct CustomerTable {
    // the actual buckets
    // an array of linked lists.
    Customer **buckets;
    // num_buckets
    size_t num_buckets;
} CustomerTable;

I use Dan bernstien's hash function cause it was in the demo and it's easy to use

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while (*str != '\0') {
        c = *str;
        hash = ((hash << 5) + hash) + (unsigned char)c; /* hash * 33 + c */
        str++;
    }
    return hash;

The build customertable function takes in the num_buckets and allocates space to create the table the buckets.

CustomerTable *build_customer_table(size_t num_buckets) {
    CustomerTable* output = calloc(1, sizeof(CustomerTable));
    output->num_buckets = num_buckets;

    // allocate space for num_buckets FriendNode pointers.
    // THEY ARE ALREADY NULL POINTERS.
    output->buckets = calloc(num_buckets, sizeof(Customer *));

    return output;
}

the add function has two functions. The add function finds out which bucket the email goes to and the add_customer_to_list assigns the values to the linked list which allocating memory for the linked list which is then returned and added to the bucket in the add function. 

Customer *add_customer_to_list(char *email, char *name, int shoe_size, char *food, Customer *bucket) {
    Customer* new_customer;

    new_customer = calloc(1, sizeof(Customer));
    new_customer->email = strdup(email);
    new_customer->name = strdup(name);
    new_customer->fav_food = strdup(food);
    new_customer->shoe_size = shoe_size;
    new_customer->next = bucket;

    return new_customer;
}

void add(char *email, char *name, int shoe_size, char *food, CustomerTable *table) {
    unsigned long hashvalue = hash(email);
    size_t which_bucket = hashvalue % table->num_buckets;
    Customer *linkedlist = table->buckets[which_bucket];
    table->buckets[which_bucket] = add_customer_to_list(email, name, shoe_size, food, linkedlist);
    return;
}

The change function is there incase the email already exists in the hash table. The main function checks is the linkedlist already exists. Might or might not work. Hasn't been tested yet.

UPDATE: the function works and has been tested.

void change(Customer *linkedlist, char *name, int shoe_size, char *food) {
    
    linkedlist->name = strdup(name);
    linkedlist->shoe_size= shoe_size;
    linkedlist->fav_food = strdup(food);
    return;
}

lookup essentially loops the specific bucket to see if the email exists. If so the Customer is returned or else NULL is returned.

Customer* lookup(char *email, CustomerTable *table){
    unsigned long hashvalue = hash(email);
    size_t which_bucket = hashvalue % table->num_buckets;

    Customer *temp = table->buckets[which_bucket];
    while(temp){
        printf("email: %s\n", temp->email);
        if((strcmp(temp->email, email) == 0)){
            printf("found!\n");
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}

The delete function also has two functions. The function delete first checks if the email exists in it's own bucket. If not, then false is returned. If it does, then it goes into delete_customer_from_list which is a recurisve function which looks for the email and removes and frees it which changing the next value and reassigning the linked list. Hasn't been tested yet.

UPDATE: has been tested and needed to change linkedlist->name in strcmp(email, linkedlist->name) to linkedlist->email in the strcmp.

Customer* delete_customer_from_list(char *email, Customer *linkedlist) {
    if (!linkedlist) {
        return NULL;
    } else if (strcmp(email, linkedlist->email) == 0) {
        // match!!
        // need to free this node!
        Customer *next = linkedlist->next;
        // now delete this node.
        free(linkedlist->name);
        free(linkedlist->email);
        free(linkedlist->fav_food);
        free(linkedlist);
        return next;
    } else {
        // recursive case!!
        linkedlist->next = delete_customer_from_list(email, linkedlist->next);
        return linkedlist;
    }
}

bool delete(char *email, CustomerTable *table) {
    unsigned long hashvalue = hash(email);
    size_t which_bucket = hashvalue % table->num_buckets;

    Customer *linkedlist = table->buckets[which_bucket];

    bool found = false;
    Customer *here = linkedlist;
    while(here) {
        if (strcmp(here->email, email) == 0) {
            found = true;
            break;
        }
        here = here->next;
    }
    if (!found) {
        return false;
    } 
    table->buckets[which_bucket] = delete_customer_from_list(email, linkedlist);
    return true;
}

pretty much loops through the hash table and the bucket printing all the customers.

void list(CustomerTable *table){
    for(size_t i = 0; i < table->num_buckets; i++){
        Customer *here = table->buckets[i];
        while(here){
            printf("email: %s\n", here->email);
            printf("name: %s\n",  here->name);
            printf("shoesize: %d\n",  here->shoe_size);
            printf("food: %s\n", here->fav_food);
            printf("\n");
            here = here->next;
        }
    }
}

rewrites the customers.tsv file and saves the customer's info with a specified format. Currently having issues with newlines with being outputted in places not intended.

UPDATE: make the function much easier to read now additionally, the issue isn't with save but with the main function which essentially taken in a newline which messes up everything.

void save(CustomerTable *table){
    FILE *infile = fopen("customers.tsv", "w");
    const char* FORMAT = "%s\t%s\t%d\t%s\n";
    for(size_t i = 0; i < table->num_buckets; i++){
        Customer *here = table->buckets[i];
        while(here){
            fprintf(infile,FORMAT,here->email, here->name, here->shoe_size, here->fav_food);
            here = here->next;
        }
    }
    fclose(infile);

}

Cleans the table and frees everything. I usually just loop throuhg the buckets and the hash table. Currently having an issue with free(freethis). Almost fixed.

UPDATE: good to go. Quit function works properly

void quit(CustomerTable *table) {
    // for each bucket, delete everything in that bucket
    for(size_t i=0; i < table->num_buckets; i++) {
        Customer *here = table->buckets[i];
        while(here){
            free(here->name);
            free(here->fav_food);
            free(here->email);
            Customer *freethis = here;
            here = here->next;
            free(freethis);
        }
    }
    free(table->buckets);
    free(table);
}

The meat of the assignment. I first start by creating a table and add the customers from the file onto the hash table. Then I set up a while loop that scans user input and checks what the user wants to do. I use if statments and strcmp to see what the user wants and if they enter the command, they would need to try again. Pretty sure it mostly works. Almost done.
UPDATE: finished with the main function. I made sure to remove the newline when taking scanf in the beginning. Essentially the main function first creates a table and then pulls the information from customers.tsv which making sure to remove the newline at the end of fav_food. The data is kept ina customer struct which is added to the table used the add function. After that base setup, the command: begins. I use scanf() to read command and have if statements to check which command is used. If quit is typed, then the loop quits and quit() is called which frees everything. While all the other commands are simple, the add is the hardest as I have to first check if an instance exists which I do using hash and a loop. If an instance doesn't exist, then I use the add() function.The other commands are easy as it's either a simple call or there is little extra information that needs to be outputted. Finally after freeing, I clsoe the file and the main function ends.

int main(void){
    // creating table
    CustomerTable* table = build_customer_table(50);

    // opening file
    char s[100];
    FILE *infile = fopen("customers.tsv", "r");
    // adding the contents from file to the hash table
    while(fgets(s, sizeof(s), infile)){
        char *token = strtok(s, "\t");
        int shoe;
        char *email;
        char *name;
        char *food;
        int i = 0;

    /* walk through other tokens */
        while( token != NULL ) {
            if(i== 0){
                email = strdup(token);
            }

            if(i == 1){
                name = strdup(token);
            }

            if(i == 2){
                shoe = atoi(token);
            }

            if(i == 3){
                food = strdup(token);
                int h = strcspn(food, "\n");
                food[h] = 0;
            }
            i++;
            token = strtok(NULL, "\t");
        }
        add(email, name, shoe, food, table);
        free(email);
        email = NULL;
        free(name);
        name = NULL;
        free(food);
        food = NULL;
    }

    printf("command: ");
    scanf("%s", s);
    while(strcmp(s, "quit") != 0){
        if(strcmp(s, "add") == 0){
            // getting data

            printf("email address? ");
            char email[50];
            scanf("%s",  email);
            while(getchar() != '\n');

            printf("name? ");
            char name[20];
            scanf("%[^\n]s", name);
            while(getchar() != '\n');

            printf("shoe size? ");
            int shoe_size;
            scanf("%d",  &shoe_size);
            while(getchar() != '\n');

            printf("favorite food? ");
            char food[50];
            scanf("%[^\n]s",  food);
            while(getchar() != '\n');
        
            // checking if email already exists

            unsigned long hashvalue = hash(email);
            size_t which_bucket = hashvalue % table->num_buckets;

            bool state = true;
            Customer *linkedlist = table->buckets[which_bucket];
            while(linkedlist){
                if(strcmp(linkedlist->email, email) == 0){
                    change(linkedlist, name, shoe_size, food);
                    state = false;
                }
                linkedlist = linkedlist->next;
            }

            if(state){
                add(email, name,shoe_size, food, table);
            }

        } else if(strcmp(s, "lookup") == 0){
            printf("email address? ");
            char email[20];
            scanf("%s",  email);
            Customer *temp = lookup(email, table);
            
            if(temp){
                printf("email: %s\n", temp->email);
                printf("name: %s\n", temp->name);
                printf("shoesize: %d\n", temp->shoe_size);
                printf("food: %s\n", temp->fav_food);
                printf("\n");
            }
            else{
                printf("user not found!\n");
            }

        } else if(strcmp(s, "delete") == 0){
            printf("email address? ");
            char email[20];
            scanf("%s",  email);
            bool k = delete(email, table);
            if(!k){
                printf("user not found!\n");
            }
        } else if(strcmp(s, "list") == 0){
            list(table);
        } else if(strcmp(s, "save") == 0){
            save(table);
        } else {
            printf("unknown command\n");
        }
        printf("command: ");
        scanf("%s", s);
    }
    quit(table);
    fclose(infile);
}

06/2/2022: While this design doc doesn't have explanation, it is because I am currently finishing up the assignment. When I am done with the assignment, I will update the assignment and the design doc.

UPDATE: changed business.c to businessdb cause I don't what you guys want the name of the file to be.
