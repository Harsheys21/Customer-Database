#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>



// struct for users

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

// Thank you Dan Bernstein.
unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while (*str != '\0') {
        c = *str;
        hash = ((hash << 5) + hash) + (unsigned char)c; /* hash * 33 + c */
        str++;
    }
    return hash;
}

//create a database
CustomerTable *build_customer_table(size_t num_buckets) {
    CustomerTable* output = calloc(1, sizeof(CustomerTable));
    output->num_buckets = num_buckets;

    // allocate space for num_buckets FriendNode pointers.
    // THEY ARE ALREADY NULL POINTERS.
    output->buckets = calloc(num_buckets, sizeof(Customer *));

    return output;
}


// add: if the user types add, the program will additionally prompt the
// user for an email address, a display name, a shoe size, and a favorite food.
// This user (with their associated data) will then be added to the hash table.
// If that email address is already present in the hash table, then replace the
// data that was already in the hash table with the new entry.
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

void change(Customer *linkedlist, char *name, int shoe_size, char *food) {
    
    linkedlist->name = strdup(name);
    linkedlist->shoe_size= shoe_size;
    linkedlist->fav_food = strdup(food);
    return;
}

// lookup: prompt the user for an email address, then go find that entry in
// the hash table! If the specified customer is not found (nobody has that
// email address) then display a nice message that the customer was not found.
// Then display all the information we have about that user in a nicely
// formatted way.
Customer* lookup(char *email, CustomerTable *table){
    unsigned long hashvalue = hash(email);
    size_t which_bucket = hashvalue % table->num_buckets;

    Customer *temp = table->buckets[which_bucket];
    while(temp){
        if((strcmp(temp->email, email) == 0)){
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}

// delete: prompt the user for an email address, then go find that entry in
// the hash table (and delete it from the linked list in the bucket). Again, if
// the specified customer is not found (nobody has that email address) then
// display a nice message that the customer was not found.
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

// list: display every user in the database, in whatever order is convenient.
// Format each user the same way we do in lookup.
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

// save: write the current state of the database back to customers.tsv.
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

    
// quit: clean up all of our memory and exit the program.
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


// responsible for running the commands pulling info from file
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

