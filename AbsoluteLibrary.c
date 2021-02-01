#include<stdio.h>
#include<string.h>
#include<time.h>
#include<ctype.h>
#include<stdlib.h>


/*-----------------------------------------------------------------
Structs used in the program
------------------------------------------------------------------
There are two class of subscribers: 
Elite (classA) and Regular (classB)
and definition of a book
*/

//Structure of Book
struct Book
{
  char name[100];
  int ID;
  int copies;  
  char author[40];
};

//Class B Subscriber Structure
struct ClassBSubscriber
{
  char name[50];
  int Category;
  int ID;
  int bookID[3];
  int bookDueDate[3];
  int status; // Active (!0) or Inactive (0)
  int fineboundary;//tuple to have dependence on availability
};

//Class A Subscriber Structure
struct ClassASubscriber
{
  char name[50];
  int Category;
  int ID;
  int bookID[4];
  int bookDueDate[4];
  int status; // Active (!0) or Inactive (0)
  int fineboundary;//tuple to have dependence on availability
};

/*-----------------------------------------------------------------
END
-------------------------------------------------------------------
*/





/*
-------------------------------------------------------------------
Adding a book to the database
-------------------------------------------------------------------
bookID generator generates from 10001 to ...
add_book - Adds a new book to the database BookDatabase
*/

//Generating book ID of a newly added book
int generate_bookID()
{
  int newID;
  int counter = 10000;
  struct Book BookGenID;
  FILE * BDB = fopen("BookDatabase", "rb");//read

  if (BDB != NULL)
    {
      while(fread(&BookGenID, sizeof(struct Book),
		  1, BDB))
	{
	  if (BookGenID.ID > counter)
	    {
	      counter = BookGenID.ID; //Take largest ID
	    }
	}
      fclose(BDB);
    }
  newID = counter + 1; //Add 1 to the largest ID
  return(newID);
}


//Adds a Book in the Database
//Accessible only through admin_mode
void add_book()
{

  //Stores the info required for inputting in the database
  char nameOfbook[50];
  //  char nameOfbookLower[50];
  char authorName[50];
  //  char authorNameLower[50];
  int newBookID;
  int number_of_copies;

  printf("\n\n-------------------------------------");
  printf("\nAdding Book to the library\n");
  printf("--------------------------------------\n");
  printf("Book Name : ");
  scanf(" %[^\n]*s", nameOfbook);//reads whole line before \n
  
  printf("Auth. Name : ");
  scanf(" %[^\n]*s", authorName);//reads whole line before \n
  
  printf("Number of copies : ");
  scanf("%d", &number_of_copies);
  
  FILE * bookDB = fopen("BookDatabase", "ab"); //Appending new book
  
  if (bookDB != NULL)
    {
      newBookID = generate_bookID();//Stores the generated new ID
      
      struct Book *newbook=
	malloc(sizeof(struct Book));//Allocates size for newbook

      //Copying the inputted values inside the struct Book
      strcpy(newbook->name, nameOfbook);
      newbook->ID = newBookID;
      newbook->copies = number_of_copies;
      strcpy(newbook->author, authorName);

      //Writting the struct into the opened file
      fwrite(newbook, sizeof(struct Book), 1, bookDB);
      fclose(bookDB);
      printf("\n-----------------------------------------\n");
      printf("Book - %s - added to Database with book ID %d",
	     nameOfbook, newBookID);
      printf("\n-----------------------------------------\n\n\n");
    }

}
/*
-------------------------------------------------------------------
END
-------------------------------------------------------------------
*/



/*
-------------------------------------------------------------------
Update the count of books
-------------------------------------------------------------------
add_bookcopy - adds incr number of books in the database.
iterates through the database while matching ID, 
then seeks after matching and changes the variable copies.
*/


/*
UpdateBookCount is used both in 
add_bookcopy (for admin) and
borrow_book (for subscribers)
*/
int UpdateBookCount(int bookID, int incr)
{
  int flag = 1; //Changes value as soon as book ID matches
  struct Book bookid_check;//Finds the position in which the book is
  FILE * BDBCount = fopen("BookDatabase", "rb");//read only
  int SeekCount = 0; //Seeker
  /*
  Finding the position of the struct corresponding to the given book
  ID in the file
  */
  if (BDBCount != NULL)
    {
      while (fread(&bookid_check, sizeof(struct Book), 1, BDBCount))
	{
	  if (bookid_check.ID == bookID)
	    {
	      flag = 0;
	    }
	  if (flag==1)//Seeker increments till bookID is not found
	    {
	      SeekCount = SeekCount + 1; 
	    }
	}
      fclose(BDBCount);
      if (flag==1) //returns negative number if book not found
	{
	  printf("\nBook ID does not exist. Check Again!\n\n");
	  return(-10);
	}
    }

  
  FILE * BDBUpdate = fopen("BookDatabase", "rb+");//read/update
  struct Book book_update; //Modifies the book copies
  int Copies; //Return value - number of copies of the book
  /*
    Updating the entry of the struct using 
    SeekCount - position of the struct in the file
   */
  if (BDBUpdate != NULL)
    {
      fseek(BDBUpdate,
	    SeekCount*sizeof(struct Book), SEEK_SET);
      fread(&book_update, sizeof(struct Book), 1, BDBUpdate);
      
      if (book_update.copies + incr < 0) 
	{
	  printf("Book out of stock!\n");
	  fclose(BDBUpdate);
	  return(-100); //returns negative number if book count < 0
	}
      book_update.copies = book_update.copies + incr;
      fseek(BDBUpdate,
	    SeekCount*sizeof(struct Book), SEEK_SET);
      fwrite(&book_update, sizeof(struct Book), 1, BDBUpdate);
      Copies = book_update.copies;
      fclose(BDBUpdate);
      return(Copies); //returns the number of copies left
      //This return can be used to set variable fineboundaries
      }
}


//Modifies the number of prints of an existing book
//Accessible only by admin
void add_bookcopy()
{
  int bookid;
  int increment;
  int res;
  printf("\n\n--------------------------------------------");
  printf("\nUpdating Book Database");
  printf("\n--------------------------------------------\n");
  printf("Book ID : ");
  scanf("%d", &bookid);
  printf("\nNumber of copies added : ");
  scanf("%d", &increment);

  res = UpdateBookCount(bookid, increment);
  if (res == -100)
    {
      printf("\nNumber of books cannot be less than 0\n");
    }
  if (res > 0)
    {
      printf("\nBook count updated to %d!\n", res);
    }
  if (res == 0)
    {
      printf("\nNo books left!\n");
    }
}
/*-----------------------------------------------------------------
END
-----------------------------------------------------------------*/




/* ----------------------------------------------------------------
Class A subscribers have ID ranging from 0 to 99
Class B subscribers have ID ranging from 100 to ...
generate1_subscriberID, generate2_subscriberID are the ID generators
add_subscriber - Adds a subscriber of either classA or classB in the 
subscriber database.
-----------------------------------------------------------------*/


//Generates ID for subscriber of class A
int generate_sub1ID()
{
  int counter = 0;
  struct ClassASubscriber SubGenID;
  int newID;
  FILE * subADB;
  subADB = fopen("SubscriberDatabaseA", "rb");//read
  if (subADB != NULL)
    {
      while(fread(&SubGenID, sizeof(struct ClassASubscriber),
		  1, subADB))
	{
	  if (SubGenID.ID > counter)
	    {
	      counter = SubGenID.ID;//Takes the highest ID 
	    }
	}
      fclose(subADB);
    }
  newID = counter + 1; //Increases the highest ID by 1
  return(newID);
}

//Generates ID for subscriber of class B
int generate_sub2ID()
{
  int counter = 100;
  struct ClassBSubscriber SubGenID;
  int newID;
  FILE * subBDB;
  subBDB = fopen("SubscriberDatabaseB", "rb");//read
  if (subBDB != NULL)
    {
      while(fread(&SubGenID, sizeof(struct ClassBSubscriber),
		  1, subBDB))
	{
	  if (SubGenID.ID > counter)
	    {
	      counter = SubGenID.ID;//Takes the highest ID
	    }
	}
      fclose(subBDB);
    }
  newID = counter + 1;//Increases the highest ID by 1
  return(newID);
};


//Adds a subscriber to either of the subscriber databases
//Accessible only by admnins
int add_subscriber()
{
  //Placeholders for the new subscriber
  char subscriber_name[25];
  int newSubID;
  int subCategory;
  
  FILE * subDB;
  printf("\n\n------------------------------------------\n");
  printf("Adding new subscriber");
  printf("\n------------------------------------------\n");
  printf("Name of new Subscriber : ");
  scanf(" %[^\n]*s", subscriber_name);
  
  printf("Subscriber Category (1 or 2) : ");
  scanf("%d", &subCategory);
  
  switch (subCategory)
    {

      //If subscriber category is A
    case 1:
      subDB = fopen("SubscriberDatabaseA", "ab");//append to database
      
      if (subDB != NULL)
	{
	  newSubID = generate_sub1ID();
	  	  
	  struct ClassASubscriber *newsub=
	    malloc(sizeof(struct ClassASubscriber));

	  //Initializing the entries of the newly created struct
	  strcpy(newsub->name, subscriber_name);
	  newsub->ID = newSubID;	  
	  newsub->status = 0;
	  newsub->Category = 1;
	  newsub->fineboundary = 1800;//30 minutes of time allowed
	  int iterator = 0;
	  while (iterator < 4) //The max number of books allowed is 4
	    {
	      newsub->bookID[iterator] = 0;
	      newsub->bookDueDate[iterator] = 0;
	      iterator = iterator + 1;
	    }

	  //Writes the struct in the file
	  fwrite(newsub, sizeof(struct ClassASubscriber), 1, subDB);
	  fclose(subDB);
	  printf("\n-----------------------------------------------");
	  printf("\nSubscriber %s added to Database A with ID %d\n",
		 subscriber_name, newSubID);
	  printf("-------------------------------------------------");
	  printf("\n\n");
	  return(1);
	}
      break;

      
      //If subscriber category is B
    case 2:
      subDB = fopen("SubscriberDatabaseB", "ab");//append to database

      if (subDB != NULL)
	{
	  newSubID = generate_sub2ID();
	  	  
	  struct ClassBSubscriber *newsub=
	    malloc(sizeof(struct ClassBSubscriber));

	  //Initializing the entries of the newly created struct
	  strcpy(newsub->name, subscriber_name);
	  newsub->ID = newSubID; 
	  newsub->status = 0;
	  newsub->Category = 2;
	  newsub->fineboundary = 900;//15 minutes allowed
	  int iterator = 0;
	  while (iterator < 3)//The max number of books allowed is 3
	    {
	      newsub->bookID[iterator] = 0;
	      newsub->bookDueDate[iterator] = 0;
	      iterator = iterator + 1;
	    }
	  
	  //Writes the struct in a file
	  fwrite(newsub, sizeof(struct ClassBSubscriber), 1, subDB);
	  fclose(subDB);
	  printf("\n----------------------------------------------");
	  printf("\nSubscriber %s added to Database B with ID %d\n",
		 subscriber_name, newSubID);
	  printf("----------------------------------------------");
	  printf("\n\n");
	  return(1);
	}
      break;
    }

}
/*---------------------------------------------------------------
END
---------------------------------------------------------------*/




/* -------------------------------------------------------------
1. borrow_book - Borrow a book by mentioning the bookID and subID
The count in book is reduced
The bookID and time of borrowing is updated in the subscriber database

2. return_book - Return a book by mentioning the bookID and subID
The count in book is increased
The bookID and time of borrowing is reset in the subscriber database
---------------------------------------------------------------*/



//Borrowing a book
//Takes the subscriber class, subscriber ID and bookID as inputs
int borrow_book(int subClass, int subID, int bookID)
{
  printf("\n-------------------------------------");
  printf("\nBorrowing book\n");
  printf("-------------------------------------\n");
  //Class A variables
  FILE * subDB_updA; 
  FILE * subDB_findA;
  int SeekSubCountA = 0;
  struct ClassASubscriber subfindA;
  struct ClassASubscriber subupdA;
  //Class B variables
  FILE * subDB_updB; 
  FILE * subDB_findB;
  int SeekSubCountB = 0;
  struct ClassBSubscriber subfindB;
  struct ClassBSubscriber subupdB;

  switch (subClass)
    {

      
      // If the subscriber belongs to class A
    case 1:

      subDB_findA = fopen("SubscriberDatabaseA", "rb");//read
      subDB_updA = fopen("SubscriberDatabaseA", "rb+");//read/update
      
      while (fread(&subfindA, sizeof(struct ClassASubscriber),
		   1, subDB_findA))
	{
	  if (subfindA.ID == subID) //Subscriber in Database
	    {
	      fseek(subDB_updA,
		    SeekSubCountA*sizeof(struct ClassASubscriber),
		    SEEK_SET);
	      fread(&subupdA, sizeof(struct ClassASubscriber),
		    1, subDB_updA);	      

	      int Flag_subBorrowBook = 0; // Check if sub can borrow
	      int BookCopies; //Stores number of copies left
	      int count = 0; // iterate through entries in sub
	      while (count < 4)//4 books allowed
		{
		  //Find out if the subscriber can borrow book
		  if (subupdA.bookID[count] == 0 &&
		      Flag_subBorrowBook == 0)
		    {
		      Flag_subBorrowBook = 1;
		      //Making changes to the book database
		      BookCopies = UpdateBookCount(bookID, -1);
		      //Checking if enough copies exists
		      if (BookCopies >= 0) 
			{
			  //Store book info in subscriber struct
			  subupdA.bookID[count] = bookID;
			  //store time of borrow
			  subupdA.bookDueDate[count] = time(NULL);
			  //store status
			  subupdA.status = subupdA.status + 1;
			  printf("\nBorrowed book with ");
			  printf("book ID : %d\n",
				 bookID);
			}
		    }
		  //A break statement could have been more efficient
		  count = count + 1;
		}
	      //If no entry is 0 in subscriber struct bookID variable
	      if (Flag_subBorrowBook == 0)
		{
		  printf("Sorry! You have borrowed %d books \n",
			 count);
		  fclose(subDB_findA);
		  fclose(subDB_updA);
		  return(1);
		}
	      fclose(subDB_findA);
	      //Writting the modified structure in the database
	      fseek(subDB_updA,
		    SeekSubCountA*sizeof(struct ClassASubscriber),
		    SEEK_SET);
	      fwrite(&subupdA, sizeof(struct ClassASubscriber),
		     1, subDB_updA);
	      fclose(subDB_updA);
	      return(1);
	    }
	  SeekSubCountA = SeekSubCountA + 1; 
	}
      break;

      
      // If the subscriber is a regular subscriber
    case 2:
      
      subDB_findB = fopen("SubscriberDatabaseB", "rb");//read
      subDB_updB = fopen("SubscriberDatabaseB", "rb+");//read/update
      
      while (fread(&subfindB, sizeof(struct ClassBSubscriber),
		   1, subDB_findB))
	{
	  if (subfindB.ID == subID) //Subscriber in Database
	    {
	      fseek(subDB_updB,
		    SeekSubCountB*sizeof(struct ClassBSubscriber),
		    SEEK_SET);
	      fread(&subupdB, sizeof(struct ClassBSubscriber),
		    1, subDB_updB);	      

	      int Flag_subBorrowBook = 0; // Check if sub can borrow
	      int BookCopies; //Stores number of copies left
	      int count = 0; // iterate through entries in sub
	      while (count < 3)//3 books allowed
		{
		  //Find out if the subscriber can borrow book
		  if (subupdB.bookID[count] == 0 &&
		      Flag_subBorrowBook == 0)
		    {
		      Flag_subBorrowBook = 1;
		      //Making changes to the book database
		      BookCopies = UpdateBookCount(bookID, -1);
		      //Checking if enough copies exists
		      if (BookCopies >= 0) 
			{
			  //Store book info in subscriber struct
			  subupdB.bookID[count] = bookID;
			  //store time of borrow
			  subupdB.bookDueDate[count] = time(NULL);
			  //store status
			  subupdB.status = subupdB.status + 1;
			  printf("\nBorrowed book with ");
			  printf("book ID : %d\n",
				 bookID);
			}
		    }
		  //A break statement could have been more efficient
		  count = count + 1;
		}
	      //If no entry is 0 in subscriber struct bookID variable
	      if (Flag_subBorrowBook == 0)
		{
		  printf("Sorry! You have borrowed %d books \n",
			 count);
		  fclose(subDB_findB);
		  fclose(subDB_updB);
		  return(1);
		}
	      fclose(subDB_findB);
	      //Writting the modified structure in the database
	      fseek(subDB_updB,
		    SeekSubCountB*sizeof(struct ClassBSubscriber),
		    SEEK_SET);
	      fwrite(&subupdB, sizeof(struct ClassBSubscriber),
		     1, subDB_updB);
	      fclose(subDB_updB);
	      return(1);
	    }
	  SeekSubCountB = SeekSubCountB + 1; 
	}
      break;
    }
  //end of switch
}
//end of borrow_book

  
//Returning a book
//Takes subscriber class, subscriber ID and book ID as input
int return_book(int subClass, int subID, int bookID)
{
  printf("\n----------------------------------------");
  printf("\nReturning Book\n");
  printf("----------------------------------------\n");
  FILE * subDB_findA;//read
  FILE * subDB_updA;//seek and update
  int SeekSubCountA = 0;//Used for seeking the position in file
  FILE * subDB_findB;//read
  FILE * subDB_updB;//seek and update
  int SeekSubCountB = 0;//Used for seeking the position in file
  struct ClassASubscriber subfindA;
  struct ClassASubscriber subupdA;
  struct ClassBSubscriber subfindB;
  struct ClassBSubscriber subupdB;

  switch (subClass)
    {
      // If the subscriber belongs to class A
    case 1:

      subDB_findA = fopen("SubscriberDatabaseA", "rb");//read
      while (fread(&subfindA, sizeof(struct ClassASubscriber),
		   1, subDB_findA))
	{
	  if (subfindA.ID == subID)
	    {
	      subDB_updA = fopen("SubscriberDatabaseA", "rb+");
	      fseek(subDB_updA,
		    SeekSubCountA*sizeof(struct ClassASubscriber),
		    SEEK_SET);
	      fread(&subupdA, sizeof(struct ClassASubscriber),
		    1, subDB_updA);
	      
	      int count = 0;//Loops over entries of books borrowed
	      int Flag = 0;//Flag changes value when ID matches
	      while (count < 4)
		{
		  if (subupdA.bookID[count] == bookID)
		    {
		      //Updates the local variables
		      Flag = 1;
		      subupdA.bookID[count] = 0;
		      subupdA.bookDueDate[count] = 0;
		      subupdA.status = subupdA.status - 1;
		      //Making changes to the book database
		      UpdateBookCount(bookID, 1);
		      printf("\nBook with ID %d returned\n", bookID);
		    }
		  count = count + 1;
		}
	      //If the provided bookID does not match 
	      if (Flag == 0)
		{
		  printf("\nYou have not borrowed the book!\n");
		  fclose(subDB_findA);
		  fclose(subDB_updA);
		  return(1);
		}
	      //Write the updates in the database
	      fclose(subDB_findA);
	      fseek(subDB_updA,
		    SeekSubCountA*sizeof(struct ClassASubscriber),
		    SEEK_SET);
	      fwrite(&subupdA, sizeof(struct ClassASubscriber),
		     1, subDB_updA);
	      fclose(subDB_updA);
	      return(1);
	    }
	  //Used for seeking the position in the database
	  SeekSubCountA = SeekSubCountA + 1; 
	}
      //safety fclose statement
      fclose(subDB_findA);
      break;
      
      
      // If the subscriber belongs to class B
    case 2:

      subDB_findB = fopen("SubscriberDatabaseB", "rb");//read
      while (fread(&subfindB, sizeof(struct ClassBSubscriber),
		   1, subDB_findB))
	{
	  if (subfindB.ID == subID)
	    {
	      subDB_updB = fopen("SubscriberDatabaseB", "rb+");
	      fseek(subDB_updB,
		    SeekSubCountB*sizeof(struct ClassBSubscriber),
		    SEEK_SET);
	      fread(&subupdB, sizeof(struct ClassBSubscriber),
		    1, subDB_updB);
	      
	      int count = 0;//Loops over entries of books borrowed
	      int Flag = 0;//Flag changes value when ID matches
	      while (count < 3)
		{
		  if (subupdB.bookID[count] == bookID)
		    {
		      //Updates the local variables
		      Flag = 1;
		      subupdB.bookID[count] = 0;
		      subupdB.bookDueDate[count] = 0;
		      subupdB.status = subupdB.status - 1;
		      //Making changes to the book database
		      UpdateBookCount(bookID, 1);
		      printf("\nBook with ID %d returned\n", bookID);
		    }
		  count = count + 1;
		}
	      //If the provided bookID does not match 
	      if (Flag == 0)
		{
		  printf("\nYou have not borrowed the book!\n");
		  fclose(subDB_findB);
		  fclose(subDB_updB);
		  return(1);
		}
	      //Write the updates in the database
	      fclose(subDB_findB);
	      fseek(subDB_updB,
		    SeekSubCountB*sizeof(struct ClassBSubscriber),
		    SEEK_SET);
	      fwrite(&subupdB, sizeof(struct ClassBSubscriber),
		     1, subDB_updB);
	      fclose(subDB_updB);
	      return(1);
	    }
	  //Used for seeking the position in the database
	  SeekSubCountB = SeekSubCountB + 1; 
	}
      //safety fclose statement
      fclose(subDB_findB);
      break;
    }
  //Subscriber Database updated-------------------------
}
/*---------------------------------------------------------------
END
---------------------------------------------------------------*/



/*---------------------------------------------------------------
Search Book 
Search Subscriber
---------------------------------------------------------------*/

//Search Book
//Subscriber and Admin function
void search_book()
{
  printf("\n------------------------------");
  printf("\nBook Search\n");
  printf("------------------------------\n");
  int response;
  int counter = 0;
  int InpSize;
  printf("Press 1 to search by the field of name of book\n");
  printf("Press 2 to search by the field of author\n");
  printf("Proper capitalization and spacing is important!\n");
  printf("Enter more than 1 word.");
  printf("Response : ");
  scanf("%d", &response);
  char book_namematcher[100];
  char book_authormatcher[40];
  
  FILE * BDB = fopen("BookDatabase", "rb");
  struct Book book;
  
  switch(response)
    {
      
    case 1:
      printf("Name of book : ");
      scanf(" %[^\n]*s", book_namematcher);
      InpSize = strlen(book_namematcher);
      while(fread(&book, sizeof(struct Book), 1, BDB))
	{
	  if (strncmp(book.name, book_namematcher, InpSize)==0)
	    {
	      counter = 1;
	      printf("\n----------------\n");
	      printf("Name : %s\n", book.name);
	      printf("Book author : %s\n", book.author);
	      printf("Book ID : %d\n", book.ID);
	      printf("Available copies : %d\n\n", book.copies);

	    }
	}
      printf("---------------\n");
      break;

    case 2:
      printf("Name of author : ");
      scanf(" %[^\n]*s", book_authormatcher);
      InpSize = strlen(book_authormatcher);
      while(fread(&book, sizeof(struct Book), 1, BDB))
	{
	  if (strncmp(book.author, book_authormatcher, InpSize)==0)
	    {
	      counter = 1;
	      printf("\n----------------\n");
	      printf("Name : %s\n", book.name);
	      printf("Book author : %s\n", book.author);
	      printf("Book ID : %d\n", book.ID);
	      printf("Available copies : %d\n\n", book.copies);
	    }
	}
      printf("---------------\n");
      break;
      
    }
  if (counter == 0)
    {
      printf("\nNo matching result found\n");
    }
}


//Search Subscriber
//Accessible by admins

int calculate_fine(int bookduedates[], int finebound);

void search_sub()
{
  printf("\n------------------------------");
  printf("\nSubscriber Search\n");
  printf("------------------------------\n");


  int response;
  printf("Press 1 to search by the field of ");
  printf("name with class of a subscriber\n");
  printf("Press 2 to search by the field of book borrowed\n");
  printf("Press 3 to search by the field of book fines\n");
  //Add another field for status of a sub.
  printf("Proper capitalization and spacing is important!\n");
  printf("Response : ");
  scanf("%d", &response);
  
  FILE * subADB = fopen("SubscriberDatabaseA", "rb");
  struct ClassASubscriber subA;
  FILE * subBDB = fopen("SubscriberDatabaseB", "rb");
  struct ClassBSubscriber subB;
  
  int subClass;
  char sub_namematcher[100];
  int counter = 0;
  int InpSize;

  int BookID;

  switch(response)
    {

      //Search by Name and class of a subscriber
    case 1:
      printf("Name of subscriber : ");
      scanf(" %[^\n]*s", sub_namematcher);
      printf("Class of subscriber (1 for A, 2 for B): ");
      scanf("%d", &subClass);
      InpSize = strlen(sub_namematcher);
      if (subClass==1)
	{
	  while(fread(&subA, sizeof(struct ClassASubscriber),
		      1, subADB))
	    {
	      int fine;
	      if (strncmp(subA.name, sub_namematcher, InpSize)==0)
		{
		  counter = 1;
		  printf("\n---------------\n");
		  printf("Name : %s\n", subA.name);
		  printf("ID : %d\n", subA.ID);
		  printf("Number of books borrowed : %d\n",
			 subA.status);
		  fine = calculate_fine(subA.bookDueDate,
					subA.fineboundary);
		  printf("Fines due : %d\n", fine);
		}
	    }
	}
      else if(subClass==2)
	{
	  while(fread(&subB, sizeof(struct ClassBSubscriber),
		      1, subBDB))
	    {
	      int fine;
	      if (strncmp(subB.name, sub_namematcher, InpSize)==0)
		{
		  counter = 1;
		  printf("\n---------------\n");
		  printf("Name : %s\n", subB.name);
		  printf("ID : %d\n", subB.ID);
		  printf("Number of books borrowed : %d\n",
			 subB.status);
		  fine = calculate_fine(subB.bookDueDate,
					subB.fineboundary);
		  printf("Fines due : %d\n", fine);
		}
	    }
	}
      else
	{
	  printf("Enter proper subscriber class!");
	}
      break;
      //------
      
      //Search for subscriber who borrowed a particlar book
    case 2:
      printf("Enter ID of book : ");
      scanf("%d", &BookID);
      printf("\nThe following subscribers ");
      printf("have a copy of the book:\n\n");

      while(fread(&subA, sizeof(struct ClassASubscriber), 1, subADB))
	{
	  int iterator = 0;
	  while(iterator < 4)
	    {
	      if (subA.bookID[iterator] == BookID)
		{
		  printf("Subscriber ID %d borrowed a copy on %d\n",
			 subA.ID, subA.bookDueDate[iterator]);
		}
	      iterator = iterator + 1;
	    }
	}
      
      while(fread(&subB, sizeof(struct ClassBSubscriber), 1, subBDB))
	{
	  int iterator = 0;
	  while(iterator < 3)
	    {
	      if (subB.bookID[iterator] == BookID)
		{
		  printf("Subscriber ID %d borrowed a copy on %d\n",
			 subB.ID, subB.bookDueDate[iterator]);
		}
	      iterator = iterator + 1;
	    }
	}
      break;
      //------

      //Search for subscribers who have an impending fine
    case 3:

      printf("The following subscribers have impending fines:\n\n");

      while(fread(&subA, sizeof(struct ClassASubscriber), 1, subADB))
	{
	  int fine = calculate_fine(subA.bookDueDate,
				    subA.fineboundary);
	  if (fine > 0)
	    {
	      printf("Subscriber ID %d has a fine of %d\n",
		     subA.ID, fine);
	    }
	}

      while(fread(&subB, sizeof(struct ClassBSubscriber), 1, subBDB))
	{
	  int fine = calculate_fine(subB.bookDueDate,
				    subB.fineboundary);
	  if (fine > 0)
	    {
	      printf("Subscriber ID %d has a fine of %d\n",
		     subB.ID, fine);
	    }
	}
      break;
      //-------
    }//end of switch
}
/*---------------------------------------------------------------
END
---------------------------------------------------------------*/



/*---------------------------------------------------------------
Subscriber mode functions:
1. Calculate fine 
2. Display borrowed books
3. subscriber functions stored in a definition
   a. submode_A
   b. submode_B
---------------------------------------------------------------*/

//Calculate the total fine of a subscriber
//Takes duedates and the finebounds as inputs
//Use tuple to return multiple fine values
int calculate_fine(int bookduedates[], int finebound)
{
  int count = 0;
  int fines = 0;

  //Counter for number of entries in bookduedates is not required(?)
  while (bookduedates[count] != 0)//Only if there is a book borrowed
    {
      //If the time is pass the deadline of return
      if (time(NULL) - bookduedates[count] > finebound)
	{
	  fines = fines + time(NULL) -
	    (bookduedates[count] + finebound);
	}
      count = count + 1;
    }
  //Fine increases by 1 every 15 minutes after deadline
  return(fines/900);
}


//Displays the books borrowed by a subscriber
/*
Takes all the bookIDs and the number of books an individual
can borrow
*/
void display_borrowed_books(int bookIDs[], int number)
{
  struct Book fetchbook;
  int i = 0;

  while (i < number) 
    {
      /*Initialize file within while loop to reset the seeker to the
      file for every book present in bookIDs*/
      FILE * BookDB;
      BookDB = fopen("BookDatabase", "rb");//read
      if (BookDB != NULL)
	{
	  /*Inefficient since it runs over all the entries 
	    even if there is no book ID stored*/
	  printf("\n---------\n");
	  while (fread(&fetchbook,
		       sizeof(struct Book), 1, BookDB))
	    {
	      if (bookIDs[i] == fetchbook.ID)
		{
		  printf("%s : %d\n",
			 fetchbook.name, fetchbook.ID);
		}
	    }
	  printf("\n---------\n");
	  fclose(BookDB); 
	}
      i = i + 1;
    }
  printf("\n\n");
}
  

void submode_A(int subID)
{

  char name[50]; //Name of subscriber
  int books[4] = {0, 0, 0, 0}; //BookIDs that are borrowed by sub
  int bookduedates[4] = {0, 0, 0, 0}; //Due times of the books 
  int fines; //Calculation of fines
  int finebound;
  int status = 0;
  
  FILE * subDB;
  struct ClassASubscriber sub_read;

  
  /*--------------------------
    Fetching information from the database and storing them into 
    local variables
  ----------------------------*/
  subDB = fopen("SubscriberDatabaseA", "rb");//read
  if (subDB != NULL)
    {
      while (fread(&sub_read, sizeof(struct ClassASubscriber),
		   1, subDB))
	{
	  /*
	    If the ID matches then store all the info in the
	    local variables
	  */
	  if (sub_read.ID == subID)
	    {
	      strcpy(name, sub_read.name);
	      finebound = sub_read.fineboundary;
	      status = sub_read.status;

	      int count = 0;
	      while (count<4)
		{
		  books[count] = sub_read.bookID[count];
		  bookduedates[count] = sub_read.bookDueDate[count];
		  count = count + 1;
		}
	      //Calculate the total fine to be displayed at entry
	      fines = calculate_fine(bookduedates, finebound);
	    }
	}
      fclose(subDB);
    }


  
  /*--------------------------
    The section where the subscriber operates by calling functions 
  ----------------------------*/

  printf("\n----------------------------------------------------");
  printf("\nWelcome %s", name);
  printf("\n----------------------------------------------------\n");

  printf("\nYou have fines due of worth : %d\n", fines);
  if (status > 0)//If the subscriber has borrowed at least one book
    {
      printf("\nYou have borrowed the following books : \n");
      display_borrowed_books(books, 4);
    }
  else//No book borrowed
    {
      printf("You have not borrowed any book\n\n");
    }

  int response2; //Stores response of player after selecting an option
  int response = 0;//Stores the response of player on prompt 

  while (response>=0)
    {
      printf("\n\n\n-------------------------------------\n");
      printf("Enter 1 to borrow book\n");
      printf("Enter 2 to return book\n");
      printf("Enter 3 to search books\n");
      printf("Enter 4 to display borrowed books\n");
      printf("Enter 5 to display total fine\n");
      printf("Enter -1 to exit Library\n");
      printf("Enter your response : ");
      scanf("%d", &response);
      printf("\n");

      //An option for reverting to main menu is available
      switch (response)
      {
	
      case 1:
	printf("Enter 0 to go to main menu\n");
	printf("Enter Book ID : ");
	scanf("%d", &response2);
	if (response2 != 0)
	  {
	    borrow_book(1, subID, response2);
	  }
	break;
	
      case 2:
	printf("Enter 0 to go to main menu\n");
	printf("Enter the ID of book you want to return : ");
	scanf("%d", &response2);
	if (response2 != 0)
	  {
	    return_book(1, subID, response2);	    
	  }
	break;
	
      case 3:
	printf("Enter 0 to go back to main menu\n");
	printf("Enter 1 to search for book : ");
	scanf("%d", &response2);
	if (response2 != 0)
	  {
	    search_book();
	  }
	break;
	
      case 4:
	display_borrowed_books(books, 4);
	break;

      case 5:
	printf("Impending fine : %d\n", fines);
	break;
	
      }
    }

}


void submode_B(int subID)
{

  char name[50]; //Name of subscriber
  int books[3] = {0, 0, 0}; //BookIDs that are borrowed by sub
  int bookduedates[3] = {0, 0, 0}; //Due times of the books 
  int fines; //Calculation of fines
  int finebound;
  int status = 0;
  
  FILE * subDB;
  struct ClassBSubscriber sub_read;

  /*--------------------------
    Fetching information from the database and storing them into 
    local variables
  ----------------------------*/
  subDB = fopen("SubscriberDatabaseB", "rb");
  if (subDB != NULL)
    {
      while (fread(&sub_read, sizeof(struct ClassBSubscriber),
				  1, subDB))
	{
	  /*
	    If the ID matches then store all the info in the
	    local variables
	  */
	  if (sub_read.ID == subID)
	    {
	      strcpy(name, sub_read.name);
	      finebound = sub_read.fineboundary;
	      status = sub_read.status;
	      int count = 0;
	      while (count < 3)
		{
		  books[count] = sub_read.bookID[count];
		  bookduedates[count] = sub_read.bookDueDate[count];
		  count = count + 1;
		}
	      //Calculate the total fine to be displayed at entry
	      fines = calculate_fine(bookduedates, finebound);
	    }
	}
      fclose(subDB);
    }


  
  /*----------------------------
    The section where the subscriber operates by calling functions 
  ------------------------------*/
  printf("\n----------------------------------------------------");
  printf("\nWelcome %s", name);
  printf("\n----------------------------------------------------\n");
  
  printf("\nYou have fines due of worth : %d\n", fines);
  if (status > 0)//If the subscriber has borrowed at least one book
    {
      printf("\nYou have borrowed the following books : \n");
      display_borrowed_books(books, 3);
    }
  else//No book borrowed
    {
      printf("You have not borrowed any book\n\n");
    }


  int response2; //Stores response of player after selecting an option
  int response = 0;//Stores the response of player on prompt 
  
  while (response>=0)
    {
      printf("\n\n\n-------------------------------------\n");
      printf("Enter 1 to borrow book\n");
      printf("Enter 2 to return book\n");
      printf("Enter 3 to search books\n");
      printf("Enter 4 to display borrowed books\n");
      printf("Enter 5 to display total fine\n");
      printf("Enter -1 to exit Library\n");
      printf("Enter your response : ");
      scanf("%d", &response);
      printf("\n");

      //An option for reverting to main menu is available
      switch (response)
      {
	
      case 1:
	printf("Enter 0 to go to main menu\n");
	printf("Enter Book ID : ");
	scanf("%d", &response2);
	if (response2 != 0)
	  {
	    borrow_book(2, subID, response2);
	  }
	break;
	
      case 2:
	printf("Enter 0 to go to main menu\n");
	printf("Enter the index of book you want to return : ");
	scanf("%d", &response2);
	if (response2 != 0)
	  {
	    return_book(2, subID, response2);	    
	  }
	break;
	
      case 3:
	printf("Enter 0 to go back to main menu\n");
	printf("Enter 1 to search for book : ");
	scanf("%d", &response2);
	if (response2 != 0)
	  {
	    search_book();
	  }
	  break;
	
      case 4:
	display_borrowed_books(books, 3);
	break;

      case 5:
	printf("Impending fine : %d\n", fines);
	break;
	
      }
    }

}

//-----------------------------------------------------------------





/*----------------------------------------------------------------
ADMIN MODE FUNCTIONS
1. display_allboooks
2. display_allsubs
3. admin_mode
----------------------------------------------------------------*/
//display all books
void display_allbooks()
{
  FILE * BDB;
  BDB = fopen("BookDatabase", "rb");//read
  struct Book booK;
  printf("\n\n----------------\nList of Books\n");
  if (BDB != NULL)
    {
      printf("\n");
      while (fread(&booK, sizeof(struct Book), 1, BDB))
	{
	  printf("---------\n");
	  printf("Book name : %s\n", booK.name);
	  printf("Book author : %s\n", booK.author);
	  printf("Book ID : %d\n", booK.ID);
	  printf("Available copies : %d\n\n", booK.copies);
	}
      printf("---------\n\n");
      fclose(BDB);
    }
}

//Display all subscribers
void display_allsubs()
{

  //Display subscribers of class A
  FILE * SDB;
  SDB = fopen("SubscriberDatabaseA", "rb");//read
  struct ClassASubscriber subA;
  if (SDB != NULL)
    {
      printf("\n---------------");
      printf("\nDatabase A\n");
      while (fread(&subA, sizeof(struct ClassASubscriber), 1, SDB))
	{
	  printf("---------\n");
	  printf("Subscriber name : %s\n", subA.name);
	  printf("Subscriber ID : %d\n", subA.ID);
	  int fines = calculate_fine(subA.bookDueDate,
				     subA.fineboundary);
	  printf("Total fine due : %d\n", fines);
	  printf("Books taken : \n");
	  for (int i = 0; i<4; i++)
	    {
	      if (subA.bookID[i] !=0)
		{
		  printf("%d- %d\n",
			 subA.bookID[i], subA.bookDueDate[i]);
		}
	    }
	}
      printf("---------\n\n");
      fclose(SDB);
    }
  printf("\n----------------\n");

  //Display subscribers of class B
  FILE * SDBp;
  SDBp = fopen("SubscriberDatabaseB", "rb");
  struct ClassBSubscriber subB;
  if (SDBp != NULL)
    {
      printf("\n---------------");
      printf("\nDatabase B\n");
      while (fread(&subB, sizeof(struct ClassBSubscriber), 1, SDBp))
	{
	  printf("---------\n");
	  printf("Subscriber name : %s\n", subB.name);
	  printf("Subscriber ID : %d\n", subB.ID);
	  int fines = calculate_fine(subB.bookDueDate,
				     subB.fineboundary);
	  printf("Total fine due : %d\n", fines);
	  printf("Books taken : \n");
	  for (int i = 0; i<3; i++)
	    {
	      if (subB.bookID[i] !=0)
		{
		  printf("%d- %d\n",
			 subB.bookID[i], subB.bookDueDate[i]);
		}
	    }
	}
      printf("---------\n\n");
      fclose(SDBp);
    }
  printf("\n"); 
}
 

void admin_mode()
{
  int response = 100;
  printf("Welcome Administrator\n");

  while (response!=0)
    {
      printf("\n\n\n-------------------------------------------\n");
      printf("Enter 1 to add new book to the Library database\n");
      printf("Enter 2 to update the number");
      printf(" of available copies in Library database\n");
      printf("Enter 3 to add a new subscriber\n");
      printf("Enter 4 to display all subscribers\n");
      printf("Enter 5 to display all books\n");
      printf("Enter 6 for book search\n");
      printf("Enter 7 for subscriber search\n");	
      printf("Enter 0 to exit Library\n");
      printf("Your response : ");
      scanf("%d", &response);

      switch (response)
      {

      case 1:	//Add a new book to the library
	add_book();
	break;
	
      case 2:	//Update the count of books in the library
	add_bookcopy();
	break;

      case 3:	//Add a new subscriber 
	add_subscriber();
	break;
	
	//Search for subscribers with specific name and subID
      case 4:	//Yet to be done
	display_allsubs();
	break;
	
      case 5:   //Search for books.
	display_allbooks();
	break;

      case 6:
	search_book();
	break;

      case 7:
	search_sub();
	break;
	
      }
      
    }
}

//------------------------------------------------------------------



/*------------------------------------------------------------------
Mode of access:
1. enquire_mode - enquires about your access mode 
2. admin_enq - enquiry (password) for the admin
3. sub1_enq - enquiry (user ID) for subscriber class A
4. sub2_enq - enquiry (user ID) for subscriber class B
5. main - putting it together
------------------------------------------------------------------ */

//access mode
int enquire_mode()
{ 
  int enq = -1;
  int counter = 0;
  printf("\n\n");
  while (enq > 2 || enq < 0) //Consider only the valid responses
    {
      printf("Class A subscriber enter 1 \n");
      printf("Class B subscriber enter 2 \n");
      printf("Mode of access: ");
      scanf("%d", &enq);
      printf("\n");
    }
  
  return(enq);
}


/*
Enquiry for admin access can be entered by pressing 0 at access point
 */  
int admin_enq()
{
  char AdminPWD[20] = "iamadmin";//admin password
  char PWD[25]; //Stores the entered password
  char pwd[25]; //Lowers the entered password

  for (int i=0; i<10; i++)
    {
      printf("Admin Password : "); 
      scanf(" %[^\n]*c", PWD); //Stores entered password
      //pwd = tolower(PWD); //Lowers password

      if (strcmp(PWD,AdminPWD)==0)
	{
	  return(10); //Exits to main if matches with 10
	}

      if (i > 5)
	{
	  printf("Out of tries.\n GOODBYE!\n");
	  return(-10); //Exits to main after trials are exhausted
	}
      printf("\nTry Again! \nPress 0 to exit\n\n"); 
    }
}
//Enquiry for admin access ends



/*
Enquiry for Subsriber class A access can be accessed by pressing 1
at access point
*/
int sub1_enq()
{
  int truthCounter = 0; //Checks if ID is correct
  int iderrCounter = 1; //Stores the number of times an error was made
  int subID; //Stores the entered subscriber ID

  //Instance of subscriber to check the given subscriber ID
  struct ClassASubscriber subMatcher;

  
  while (iderrCounter < 7) //Any number greater than allowed trials
    {
      //Open file to loop over all the subscriber entries
      FILE * subDB = fopen("SubscriberDatabaseA", "rb");//read

      printf("\nPlease Enter Your Subscription ID : ");
      scanf(" %d", &subID); //Stores the entered subscriber ID

      //Use submatcher to loop over all the instances in
      //the classASubscriber database
      while (fread(&subMatcher,
		   sizeof(struct ClassASubscriber), 1, subDB))
	{
	  if (subID == subMatcher.ID)
	    {
	      truthCounter = 1; //Valid ID
	    }
	}
      if (truthCounter==1) 
	{
	  fclose(subDB);
	  return(subID); //Returns the subscriber ID
	}
      if (iderrCounter > 5) //At most 5 trials are allowed
	{
	  printf("\nOut of tries.\n GOODBYE!");
	  fclose(subDB);
	  return(-1); //Returns a number less than 0 to close library
	}
      iderrCounter = iderrCounter + 1;
      fclose(subDB);
    }
  
}
//Enquiry for subsriber class A ends



/*
Enquiry for Subsriber class B access can be accessed by pressing 2
at access point
*/
int sub2_enq()
{
  int truthCounter = 0; //Checks if ID is correct
  int iderrCounter = 0; //Stores the number of times an error was made
  int subID; //Stores the entered subscriber ID

  //Open file to loop over all the subscriber entries
  struct ClassBSubscriber subMatcher;
 
  while (iderrCounter < 7)
    {
      //Instance of subscriber to check the given subscriber ID      
      FILE * subDB = fopen("SubscriberDatabaseB", "rb");
      
      printf("\nPlease Enter Your Subscription ID : ");
      scanf("%d", &subID); //Stores the entered subscriber ID
      
      //Use submatcher to loop over all the instances in
      //the classBSubscriber database
      while (fread(&subMatcher,
		   sizeof(struct ClassBSubscriber), 1, subDB))
	{
	  if (subID == subMatcher.ID)
	    {
	      truthCounter = 1; //Valid ID
	    }
	}
      if (truthCounter==1)
	{
	  fclose(subDB);
	  return(subID); //Returns the subscriber ID
	}
      if (iderrCounter > 5) //At most 5 trials are allowed
	{
	  printf("\nOut of tries.\n GOODBYE!");
	  fclose(subDB);
	  return(-1); //Returns a number less than 0 to close library
	}
      iderrCounter = iderrCounter + 1;
      fclose(subDB);
    }
  
}
//Enquiry for subscriber class B ends



int main()
{

  int mode;
  int ID;
  int response;

  printf("\n\n\n");
  printf("-----------------------------------------------\n");
  printf("-----------------------------------------------\n");    
  printf("WELCOME TO THE LIBRARY\n");
  printf("-----------------------------------------------\n");
  printf("-----------------------------------------------\n");
  
  mode = enquire_mode();
  
  switch (mode)
    {
      
    case 0:
      response = admin_enq();

      if (response > 0)
	{
	  printf("---------------------------------\n");
	  printf("ADMIN MODE\n");
	  printf("---------------------------------\n");
	  admin_mode();
	  return(1);
	}
      else //exit if return is <=0
	{
	  return(1);
	}
      
    case 1:
      ID = sub1_enq();//returns ID of sub
      if (ID < 0) //exit if return is less than 0
	{
	  return(1);
	}
      else
	{
	  submode_A(ID);
	  return(1);
	}
      
    case 2:
      ID = sub2_enq(); // returns ID of sub
      if (ID < 0) //exit if return is less than 0 
	{
	  return(1);
	}
      else
	{
	  submode_B(ID);
	  return(1);
	}      
    }
}

